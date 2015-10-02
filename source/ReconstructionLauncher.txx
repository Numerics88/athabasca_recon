/*
Copyright (C) 2011 Eric Nodwell
enodwell@ucalgary.ca

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_FILESYSTEM_VERSION 3

#include "engines/single_threaded/SingleThreadedReconstructionRunner.hpp"
#include "engines/multi_threaded/MultiThreadedReconstructionRunner.hpp"
#include "io/AnyFormatStreamingProjectionsReader.hpp"
#include "io/StreamingMetaImageWriter.hpp"
#include "io/MetaImageWriter.hpp"
#include "CTCalibration.hpp"
#include "projection_correction/BadPixelCorrector.hpp"
#include "projection_correction/BeamPowerCorrectors.hpp"
#include "filtering/SmoothingFilters.hpp"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/math/constants/constants.hpp>

using namespace athabasca_recon;
using boost::filesystem::path;
using boost::algorithm::to_lower_copy;
using boost::optional;
using boost::math::constants::pi;

namespace athabasca_recon
  {

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  ReconstructionLauncher<TProjectionIn,TProjectionOut,TVolume>::ReconstructionLauncher
    (
    const ReconConfiguration& config
    )
    :
    m_Config(config)
    {}

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void ReconstructionLauncher<TProjectionIn,TProjectionOut,TVolume>::Launch()
    {

    // Seems to be required to remap this->m_Config to config to compile.  Why??
    const ReconConfiguration& config = this->m_Config;

    // Object pointers for optional objects
    boost::scoped_ptr<CTCalibration<TProjectionIn,TProjectionOut> > ctCalibration;
    boost::scoped_ptr<AnyFormatStreamingProjectionsReader<TProjectionIn> > rawProjectionsReader;
    boost::scoped_ptr<AnyFormatStreamingProjectionsReader<TProjectionOut> > attenuationProjectionsReader;
    boost::scoped_ptr<AnyFormatStreamingProjectionsReader<TProjectionOut> > filteredProjectionsReader;
    boost::scoped_ptr<BadPixelCorrector<TProjectionOut> > bad_pixel_corrector;
    boost::scoped_ptr<std::ofstream> attenuation_corrections_file;
    boost::scoped_ptr<BeamPowerCorrector<TProjectionOut> > beamPowerCorrector;
    typedef bonelab::Array<1,TProjectionOutValue,TIndex> TFunction1D;
    boost::scoped_ptr<TransferFunction<TFunction1D> > smoothingTransferFunctionGenerator;
    boost::scoped_ptr<StreamingMetaImageWriter<TProjectionOut> > attenuationProjectionsWriter;
    boost::scoped_ptr<StreamingMetaImageWriter<TProjectionOut> > filteredProjectionsWriter;
    boost::scoped_ptr<StreamingMetaImageWriter<TVolumeSlice> > volumeWriter;

    // Some values we'll need
    bonelab::Tuple<2,TIndex> proj_dims = reverse(config.get<bonelab::Tuple<2,TIndex> >("Projections.Dimensions"));
    int numberOfProjections = config.get<int>("Projections.NumberOfProjections");
    int projectionStride = config.get("Projections.ProjectionStride", 1);
    numberOfProjections = (numberOfProjections + projectionStride - 1)/projectionStride;
    bonelab::Tuple<2,TSpace> pixelsize = reverse(config.get<bonelab::Tuple<2,TSpace> >("Projections.PixelSize"));
    TSpace originU = -pixelsize[1] * config.get<TSpace>("Projections.CenterPixelU");
    TSpace originV = config.get<TSpace>("Projections.OffsetV");
    bonelab::Tuple<2,TSpace> proj_origin(originV, originU);
    // The following aren't required unless we output the volume.
    bonelab::Tuple<3,TIndex> vol_dims;
    bonelab::Tuple<3,TSpace> vol_spacing;
    bonelab::Tuple<3,TSpace> vol_origin;
    if (config.get_optional<std::string>("Output.VolumeFile"))
      {
      vol_dims = reverse(config.get<bonelab::Tuple<3,TIndex> >("Volume.Dimensions"));
      vol_spacing = reverse(config.get<bonelab::Tuple<3,TSpace> >("Volume.VoxelSize"));
      vol_origin = reverse(config.get<bonelab::Tuple<3,TSpace> >("Volume.Origin"));
      }

    // Create necessary streaming reader objects  

    optional<std::string> rawProjectionsFile = config.get_optional<std::string>("Input.RawProjectionsFile");
    optional<std::string> attenuationProjectionsFile = config.get_optional<std::string>("Input.AttenuationProjectionsFile");
    optional<std::string> filteredProjectionsFile = config.get_optional<std::string>("Input.FilteredProjectionsFile");

    if (rawProjectionsFile)
      {
      // Need a calibration object for raw data
      ctCalibration.reset(new CTCalibration<TProjectionIn,TProjectionOut>());
      ctCalibration->SetProjectionsFileName(*rawProjectionsFile);
      if (optional<std::string> fn = config.get_optional<std::string>("Input.DarkFieldFile"))
        { ctCalibration->SetDarkFieldFileName(*fn); }
      if (optional<std::string> fn = config.get_optional<std::string>("Input.BrightFieldFile"))
        { ctCalibration->SetBrightFieldFileName(*fn); }
      if (optional<std::string> fn = config.get_optional<std::string>("Input.PostScanBrightFieldFile"))
        { ctCalibration->SetPostScanBrightFieldFileName(*fn); }
      ctCalibration->LoadCalibrationFields();
    
      // Now construct a streaming reader object for the raw projections.
      rawProjectionsReader.reset(new AnyFormatStreamingProjectionsReader<TProjectionIn>());
      rawProjectionsReader->SetFileName(*rawProjectionsFile);    
      rawProjectionsReader->SetDimensions(proj_dims);
      rawProjectionsReader->SetNumberOfProjections(numberOfProjections);
      rawProjectionsReader->SetProjectionStride(projectionStride);
      rawProjectionsReader->SetOrigin(proj_origin);
      rawProjectionsReader->SetPixelSize(pixelsize);
      rawProjectionsReader->Initialize();

      // If required, create a BadPixelCorrector
      if (config.get("Reconstruction.BadPixelCorrection", std::string("None")) == "Averaging")
        {
        bad_pixel_corrector.reset(new BadPixelCorrector<TProjectionOut>(proj_dims));
        bad_pixel_corrector->SetFlatFieldBadThreshold(config.get<TProjectionInValue>("Reconstruction.FlatFieldBadThreshold"));
        if (boost::optional<TProjectionInValue> dft = config.get_optional<TProjectionInValue>("Reconstruction.DarkFieldBadThreshold"))
          { bad_pixel_corrector->SetDarkFieldBadThreshold(*dft); }
        bad_pixel_corrector->IdentifyBadPixels(ctCalibration->GetDarkField(),
                                               ctCalibration->GetFlatField());
        std::cout << "Identified " << bad_pixel_corrector->GetNumberOfBadPixels() << " bad pixel(s).\n";
        bad_pixel_corrector->ConstructCorrectionTable();
        }

      // If required, create a BeamPowerCorrector
      std::string beam_power_option = config.get("Reconstruction.BeamPowerCorrection", std::string("None"));
      if (beam_power_option == "Manual")
        {
        ManualBeamPowerCorrector<TProjectionOut>* manualBeamPowerCorrector =
                                 new ManualBeamPowerCorrector<TProjectionOut>();
        TProjectionOutValue constantTerm = config.get<TProjectionOutValue>("Reconstruction.BeamPowerDecayConstantTerm");
        manualBeamPowerCorrector->SetBeamDecayConstantTerm(constantTerm);
        TProjectionOutValue linearTerm = config.get<TProjectionOutValue>("Reconstruction.BeamPowerDecayLinearTerm");
        manualBeamPowerCorrector->SetBeamDecayLinearTerm(linearTerm*projectionStride);
        beamPowerCorrector.reset(manualBeamPowerCorrector);
        }
      else if (beam_power_option == "BeforeAndAfterBrightField")
        {
        BeforeAndAfterBrightFieldBeamPowerCorrector<TProjectionOut>* autoBeamPowerCorrector =
              new BeforeAndAfterBrightFieldBeamPowerCorrector<TProjectionOut>();
        autoBeamPowerCorrector->Initialize(ctCalibration.get(),
                                           numberOfProjections,
                                           projectionStride,
                                           bad_pixel_corrector.get());
        std::cout << "Beam Power Decay Linear Term calculated from before and after bright field is "
                  << autoBeamPowerCorrector->GetBeamDecayLinearTerm() << " .\n";
        if (boost::optional<TProjectionOutValue> constantTerm = 
             config.get<TProjectionOutValue>("Reconstruction.BeamPowerDecayConstantTerm"))
          {
          autoBeamPowerCorrector->SetBeamDecayConstantTerm(*constantTerm);
          }
        else
          {
          std::cout << "Beam Power Decay Constant Term estimated to be "
                    << autoBeamPowerCorrector->GetBeamDecayConstantTerm() << " .\n";
          }
        beamPowerCorrector.reset(autoBeamPowerCorrector);
        }
      else if (beam_power_option == "ConstantTotalAttenuation")
        {
        ConstantAttenuationBeamPowerCorrector<TProjectionOut>* constantAttenuationBeamPowerCorrector =
              new ConstantAttenuationBeamPowerCorrector<TProjectionOut>();
        if (boost::optional<TProjectionOutValue> constantTerm = 
             config.get<TProjectionOutValue>("Reconstruction.BeamPowerDecayConstantTerm"))
          {
          constantAttenuationBeamPowerCorrector->SetBeamDecayConstantTerm(*constantTerm);
          }
        beamPowerCorrector.reset(constantAttenuationBeamPowerCorrector);
        }
      else if (beam_power_option == "NullProjectionEdge")
        {
        NullProjectionEdgeBeamPowerCorrector<TProjectionOut>* zeroProjectionEdgeBeamPowerCorrector =
                   new NullProjectionEdgeBeamPowerCorrector<TProjectionOut>();
        TIndex edgeWidth = config.get<TIndex>("Reconstruction.ProjectionBackgroundEdgeWidth");
        zeroProjectionEdgeBeamPowerCorrector->SetEdgeWidth(edgeWidth);
        beamPowerCorrector.reset(zeroProjectionEdgeBeamPowerCorrector);
        }
      if (beam_power_option != "None")
        {
        if (optional<std::string> fn = config.get_optional<std::string>("Output.AttenuationCorrectionsFile"))
          {
          attenuation_corrections_file.reset(new std::ofstream);
          attenuation_corrections_file->exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
          attenuation_corrections_file->open(fn->c_str());
          }
        }

      }   // if (rawProjectionsFile)

    if (attenuationProjectionsFile)
      {
      attenuationProjectionsReader.reset(new AnyFormatStreamingProjectionsReader<TProjectionOut>());
      attenuationProjectionsReader->SetFileName(*attenuationProjectionsFile);    
      attenuationProjectionsReader->SetDimensions(proj_dims);
      attenuationProjectionsReader->SetNumberOfProjections(numberOfProjections);
      attenuationProjectionsReader->SetProjectionStride(projectionStride);
      attenuationProjectionsReader->SetPixelSize(pixelsize);
      attenuationProjectionsReader->SetOrigin(proj_origin);
      attenuationProjectionsReader->Initialize();
      }

    if (filteredProjectionsFile)
      {
      filteredProjectionsReader.reset(new AnyFormatStreamingProjectionsReader<TProjectionOut>());
      filteredProjectionsReader->SetFileName(*filteredProjectionsFile);    
      filteredProjectionsReader->SetDimensions(proj_dims);
      filteredProjectionsReader->SetNumberOfProjections(numberOfProjections);
      filteredProjectionsReader->SetProjectionStride(projectionStride);
      filteredProjectionsReader->SetPixelSize(pixelsize);
      filteredProjectionsReader->SetOrigin(proj_origin);
      filteredProjectionsReader->Initialize();
      }
    else
      {
      // Objects that are only required if filtering will be performed
      std::string filtering_option = config.get("Reconstruction.SmoothingFilter", std::string("None"));
      if (filtering_option == "Gaussian")
        {
        GaussianFilter<TFunction1D>* filter = new GaussianFilter<TFunction1D>();
        filter->SetRadius(config.get<TProjectionOutValue>("Reconstruction.SmoothingFilterRadius"));
        smoothingTransferFunctionGenerator.reset(filter);
        }
      else if (filtering_option == "TaperedCosineWindow")
        {
        TaperedCosineWindowFilter<TFunction1D>* filter = new TaperedCosineWindowFilter<TFunction1D>();
        bonelab::Tuple<2,TProjectionOutValue> filterFrequencies = config.get<bonelab::Tuple<2,TProjectionOutValue> >
                                ("Reconstruction.SmoothingFilterFrequencies");
        filter->SetF1(filterFrequencies[0]);
        filter->SetF2(filterFrequencies[1]);
        smoothingTransferFunctionGenerator.reset(filter);
        }
      else if (filtering_option != "None")
        { throw_athabasca_exception("Unrecognized value for Reconstruction.SmoothingFilter"); }
      }

    // Create necessary output writer objects.

    if (optional<std::string> fn = config.get_optional<std::string>("Output.AttenuationProjectionsFile"))
      {
      attenuationProjectionsWriter.reset(new StreamingMetaImageWriter<TProjectionOut>);
      attenuationProjectionsWriter->SetFileName(*fn);
      attenuationProjectionsWriter->SetSliceDimensions(proj_dims);
      attenuationProjectionsWriter->SetPixelSize(pixelsize);    
      attenuationProjectionsWriter->SetSliceOrigin(proj_origin);
      attenuationProjectionsWriter->Initialize();
      }

    if (optional<std::string> fn = config.get_optional<std::string>("Output.FilteredProjectionsFile"))
      {
      filteredProjectionsWriter.reset(new StreamingMetaImageWriter<TProjectionOut>);
      filteredProjectionsWriter->SetFileName(*fn);
      filteredProjectionsWriter->SetSliceDimensions(proj_dims);
      filteredProjectionsWriter->SetPixelSize(pixelsize);    
      filteredProjectionsWriter->SetSliceOrigin(proj_origin);
      filteredProjectionsWriter->Initialize();
      }

    if (optional<std::string> fn = config.get_optional<std::string>("Output.VolumeFile"))
      {
      volumeWriter.reset(new StreamingMetaImageWriter<TVolumeSlice>);
      volumeWriter->SetFileName(*fn);
      bonelab::Tuple<2,TIndex> slice_dims = bonelab::Tuple<2,TIndex>(vol_dims[1], vol_dims[2]);
      volumeWriter->SetSliceDimensions(slice_dims);
      volumeWriter->SetElementSize(vol_spacing);    
      volumeWriter->SetOrigin(vol_origin);
      volumeWriter->Initialize();
      }

    // Create the runner object and launch it

    boost::scoped_ptr<ReconstructionRunner<TProjectionIn,TProjectionOut,TVolume> > runner;
    std::string engine = config.get<std::string>("Software.Engine");
    if (engine == "SingleThreaded")
      {
      runner.reset(new SingleThreadedReconstructionRunner<TProjectionIn,TProjectionOut,TVolume>());
      }
    else if (engine == "MultiThreaded")
      {
      MultiThreadedReconstructionRunner<TProjectionIn,TProjectionOut,TVolume>* runner_mt =
          new MultiThreadedReconstructionRunner<TProjectionIn,TProjectionOut,TVolume>();
      runner.reset(runner_mt);
      if (config.get<std::string>("Software.Threads") != "Automatic")
        {
        int threads = config.get<int>("Software.Threads");
        runner_mt->SetNumberOfThreads(threads);
        }
      }
    else
      { throw_athabasca_exception("Unrecognized value for Software.Engine"); }

    // Set up the runner (volume parameters postponed until we determine subvolumes).
    runner->SetCalibration(ctCalibration.get());
    runner->SetRawProjectionsReader(rawProjectionsReader.get());
    runner->SetProjectionsAtBothLimits(config.get<std::string>("Projections.ProjectionAt180", std::string("True")) == "True");
    runner->SetReverseRotation(config.get<std::string>("Projections.ReverseRotation", std::string("False")) == "True");
    runner->SetBadPixelCorrector(bad_pixel_corrector.get());
    runner->SetBeamPowerCorrector(beamPowerCorrector.get());
    runner->SetAttenuationProjectionsReader(attenuationProjectionsReader.get());
    runner->SetFilteredProjectionsReader(filteredProjectionsReader.get());
    runner->SetSmoothingFilter(smoothingTransferFunctionGenerator.get());
    std::string pi_option = config.get("Reconstruction.PixelInterpolation", std::string("BilinearWithFallback"));
    if (pi_option == "NearestNeighbor")
      { runner->SetPixelInterpolation(PIXEL_INTERPOLATION_NEAREST); }
    else if (pi_option == "Bilinear")
      { runner->SetPixelInterpolation(PIXEL_INTERPOLATION_BILINEAR); }
    else if (pi_option == "BilinearWithFallback")
      { runner->SetPixelInterpolation(PIXEL_INTERPOLATION_BILINEAR_WITH_FALLBACK); }
    else
      { throw_athabasca_exception("Unrecognized value for Reconstruction.PixelInterpolation\n"); }
    runner->SetAttenuationProjectionsWriter(attenuationProjectionsWriter.get());
    runner->SetAttenuationCorrectionsFile(attenuation_corrections_file.get());
    runner->SetFilteredProjectionsWriter(filteredProjectionsWriter.get());
    runner->SetVolumeWriter(volumeWriter.get());
    runner->SetScalingFactor(config.get<TVolumeValue>("Reconstruction.ScalingFactor", TVolumeValue(1)));
    if (boost::optional<std::string> opt = config.get_optional<std::string>("Software.FilteringModule"))
      {
      if (*opt == "RealSpaceConvolution")
        { runner->SetFilteringModule(FILTERING_MODULE_CONVOLUTION); }
      else if (*opt == "vDSP")
        { runner->SetFilteringModule(FILTERING_MODULE_VDSP); }
      else if (*opt == "FFTW")
        { runner->SetFilteringModule(FILTERING_MODULE_FFTW); }
      else
        { throw_athabasca_exception("Internal Error\n"); }
      }
  
    // Figure out how many chunks to use for the volume
  
    int numberOfVolumeChunks = 1;
    boost::scoped_ptr<Subvolumer<TVolume> > subvolumer;
    if (volumeWriter)
      {
      int64_t maxVolumeMemory = config.ParseMemoryValue("Software.MaximumVolumeMemory");
      int64_t totalVolumeMemory = sizeof(TVolumeValue)*long_product(vol_dims);
      numberOfVolumeChunks = int((maxVolumeMemory + totalVolumeMemory - 1)/maxVolumeMemory);
      // Ensure that numberOfVolumeChunks is not more than the number of volume slices
      numberOfVolumeChunks = std::min(numberOfVolumeChunks, vol_dims[0]);
      std::cout << "Processing the volume in " << numberOfVolumeChunks;
      if (numberOfVolumeChunks == 1)
        { std::cout << " pass.\n"; }
      else
        { std::cout << " passes.\n"; }
      int64_t requiredVolumeMemory = (totalVolumeMemory + numberOfVolumeChunks - 1)/numberOfVolumeChunks;
      int requiredVolumeMemoryMB = int((requiredVolumeMemory + 1024*1024 - 1)/(1024*1024));
      std::cout << "Volume memory usage will be " << requiredVolumeMemoryMB << " MB.\n";
      subvolumer.reset(new Subvolumer<TVolume>(vol_dims, vol_spacing, vol_origin, numberOfVolumeChunks));
      }
  
    // Now loop over the volume chunks, executing the runner on each volume chunk.
    
    for (int pass=0; pass < numberOfVolumeChunks; ++pass)
      {
      if (numberOfVolumeChunks > 1)
        { std::cout << "\n==== PASS " << pass+1 << " ====\n"; }
      if (pass != 0)
        {
        // Only generate processed projection output files on first pass.
        runner->SetAttenuationProjectionsWriter(NULL);
        runner->SetAttenuationCorrectionsFile(NULL);
        runner->SetFilteredProjectionsWriter(NULL);
        // Rewind all the input files if not the first pass.
        if (rawProjectionsReader)
          { rawProjectionsReader->Rewind(); }
        if (attenuationProjectionsReader)
          { attenuationProjectionsReader->Rewind(); }
        if (filteredProjectionsReader)
          { filteredProjectionsReader->Rewind(); }
        }
      if (volumeWriter)
        {
        runner->SetVolumeDims(subvolumer->GetSubvolumeDims(pass));
        runner->SetVoxelSize(vol_spacing);
        runner->SetVolumeOrigin(subvolumer->GetSubvolumeOrigin(pass));
        }
      runner->Execute();
      }
    std::cout << "\nDone.\n";

    }

  }  // namespace athabasca_recon
