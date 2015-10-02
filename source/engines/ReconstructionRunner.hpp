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

#ifndef BONELAB_ReconstructionRunner_HPP_INCLUDED
#define BONELAB_ReconstructionRunner_HPP_INCLUDED

#include "CTCalibration.hpp"
#include "projection_correction/BadPixelCorrector.hpp"
#include "projection_correction/BeamPowerCorrector.hpp"
#include "filtering/TransferFunction.hpp"
#include "io/StreamingProjectionsReader.hpp"
#include "io/StreamingWriter.hpp"
#include <boost/format.hpp>
#include <boost/noncopyable.hpp>
#include <boost/static_assert.hpp>

namespace athabasca_recon
  {

  /** Enumeration of filtering module options. */
  enum filtering_module_t
    {
    FILTERING_MODULE_CONVOLUTION,
    FILTERING_MODULE_FFTW,
    FILTERING_MODULE_VDSP
    };

  /** Enumeration of pixel interpolation options. */
  enum pixel_interpolation_t
    {
    PIXEL_INTERPOLATION_NEAREST,
    PIXEL_INTERPOLATION_BILINEAR,
    PIXEL_INTERPOLATION_BILINEAR_WITH_FALLBACK
    };

  /** An abstract base class for carrying out reconstructions.
    *
    * This class is the maestro of the software.  It has
    * different implementations (single threaded and multi threaded for
    * example).  It doesn't deal with configuration data, but once the
    * configuration is completed, a subclass of this class takes over
    * to launch and manage the actual calculations.  It's main job is
    * to combine a bunch of calculation objects into a sensible calculation
    * pipeline.
    *
    * The flow of the calculations is determined principally by setting
    * the file readers/writers.  Exactly one reader is required (raw,
    * attenuation or filtered), and this establishes at what point the
    * calculations start.  At least one writer is required (attenuation,
    * filtered, reconstructed volume), although multiple writers are
    * permitted.  Calculations stop when the required results for all
    * specified writers are obtained.
    */
  template <class TProjectionIn, class TProjectionOut, class TVolume>
  class ReconstructionRunner : private boost::noncopyable
    {
    public:

      typedef typename TProjectionIn::value_type TProjectionInValue;
      typedef typename TProjectionOut::value_type TProjectionOutValue;
      typedef typename TProjectionOut::space_type TSpace;
      typedef typename TProjectionOut::index_type TIndex;
      typedef CTCalibration<TProjectionIn,TProjectionOut> TCalibration;
      typedef BadPixelCorrector<TProjectionOut> TBadPixelCorrector;
      typedef BeamPowerCorrector<TProjectionOut> TBeamPowerCorrector;
      typedef StreamingWriter<TProjectionOut> TProjectionsWriter;
      typedef typename TVolume::value_type TVolumeValue;
      typedef typename TVolume::index_type TVolumeIndex;
      typedef typename TVolume::space_type TVolumeSpace;
      typedef typename bonelab::Image<2,TVolumeValue,TVolumeIndex,TVolumeSpace> TVolumeSlice;
      typedef StreamingWriter<TVolumeSlice> TVolumeWriter;
      typedef bonelab::Array<1,TProjectionOutValue,TIndex> TFunction1D;

      // Some paranoid checks that TSpace is in fact the same for all the
      // objects.  (Because for simplicity we're going to assume it,
      // although a more general approach could use implemented if necessary.)
      BOOST_STATIC_ASSERT(sizeof(typename TProjectionOut::space_type) == sizeof(TSpace));
      BOOST_STATIC_ASSERT(sizeof(typename TVolume::space_type) == sizeof(TSpace));
      
      ReconstructionRunner();
      virtual ~ReconstructionRunner() {}
      
      /** Set the Calibration object.
        * Required.
        */
      void SetCalibration(const TCalibration* calibration) {this->m_Calibration = calibration;}
      
      /** Set the raw projections reader.
        * Exactly one reader is required (raw, attenuation or filtered).
        */
      void SetRawProjectionsReader(StreamingProjectionsReader<TProjectionIn>* reader) {m_RawProjectionsReader = reader;}
      
      /** Set the attenuation projections reader.
        * Exactly one reader is required (raw, attenuation or filtered).
        */
      void SetAttenuationProjectionsReader(StreamingProjectionsReader<TProjectionOut>* reader) {m_AttenuationProjectionsReader = reader;}

      /** Set the filtered projections reader.
        * Exactly one reader is required (raw, attenuation or filtered).
        */
      void SetFilteredProjectionsReader(StreamingProjectionsReader<TProjectionOut>* reader) {m_FilteredProjectionsReader = reader;}

      /** Set a bad pixel corrector.
        * Optional.
        */
      void SetBadPixelCorrector(TBadPixelCorrector* arg) {m_BadPixelCorrector = arg;}

      /** Set a beam power corrector.
        * Optional.
        */
      void SetBeamPowerCorrector(TBeamPowerCorrector* arg) {this->m_BeamPowerCorrector = arg;}

      /** Set an output Attenuation Corrections file.
        * Optional.  Obviously only sensible to set if beam power correction
        * being performed.
        */
      void SetAttenuationCorrectionsFile(std::ostream* s) {m_AttenuationCorrectionsFile = s;}

      /** Set an attenuation projections writer.
        * Optional.  At least one writer is required; multiple writers are allowed.
        */
      void SetAttenuationProjectionsWriter(TProjectionsWriter* arg)
        {m_AttenuationProjectionsWriter = arg;}

      /** Set a filtered projections writer.
        * Optional.  At least one writer is required; multiple writers are allowed.
        */
      void SetFilteredProjectionsWriter(TProjectionsWriter* arg)
        {m_FilteredProjectionsWriter = arg;}

      /** Set an reconstructed volume writer.
        * Optional.  At least one writer is required; multiple writers are allowed.
        */
      void SetVolumeWriter(TVolumeWriter* writer) {m_VolumeWriter = writer;}

      /** Sets the dimensions of the reconstruction volume.
        * Required if the volume writer is set.  No default.
        */
      void SetVolumeDims(bonelab::Tuple<3,TVolumeIndex> dims) {m_VolumeDims = dims;}

      /** Sets the voxel spacing of the reconstruction volume.
        * Required if the volume writer is set.  No default.
        */
      void SetVoxelSize(bonelab::Tuple<3,TVolumeSpace> spacing) {m_VoxelSize = spacing;}

      /** Sets the origin of the reconstruction volume.
        * Required if the volume writer is set.  No default.
        */
      void SetVolumeOrigin(bonelab::Tuple<3,TVolumeSpace> origin) {m_VolumeOrigin = origin;}

      /** Sets the parameter ProjectionsAtBothLimits, which is equivalent to
        * the configuration parameter Reconstruction.ProjectionAt180.
        * Default is true.
        */
      void SetProjectionsAtBothLimits(bool arg) {this->m_ProjectionsAtBothLimits = arg;}

      /** Specifies whether the rotation should be reversed.  That is, whether
        * the rotation should go from 0 decreasing to -180 instead of from 0
        * increasing to +180.
        * Default is false.
        */
      void SetReverseRotation(bool arg) {this->m_ReverseRotation = arg;}

      /** Sets a scaling factor that is applied to the reconstruction volume.
        * Default is false.
        */
      void SetScalingFactor(TVolumeValue arg) {this->m_ScalingFactor = arg;}

      /** Specifies the filtering module to be used.
        * The default is system-dependent.
        */
      void SetFilteringModule(filtering_module_t arg) {this->m_FilteringModule = arg;}

      /** Specifies the pixel interpolation to be used.
        * Default is PIXEL_INTERPOLATION_BILINEAR.
        */
      void SetPixelInterpolation(pixel_interpolation_t arg) {this->m_PixelInterpolation = arg;}

      /** Specifies the smoothing filter to be used.
        * Optional.
        */ 
      void SetSmoothingFilter(TransferFunction<TFunction1D>* arg) {this->m_SmoothingFilter = arg;}

      /** The method that executes the reconstruction. */
      virtual void Execute() = 0;

    protected:

      void Print(const std::string& arg);
      void Print(const boost::format& arg);
      
      const TCalibration* m_Calibration;
      StreamingProjectionsReader<TProjectionIn>* m_RawProjectionsReader;
      StreamingProjectionsReader<TProjectionOut>* m_AttenuationProjectionsReader;
      StreamingProjectionsReader<TProjectionOut>* m_FilteredProjectionsReader;
      TBadPixelCorrector* m_BadPixelCorrector;
      TBeamPowerCorrector* m_BeamPowerCorrector;
      std::ostream* m_AttenuationCorrectionsFile;
      TProjectionsWriter* m_AttenuationProjectionsWriter;
      TProjectionsWriter* m_FilteredProjectionsWriter;
      TVolumeWriter* m_VolumeWriter;
      bonelab::Tuple<3,TVolumeIndex> m_VolumeDims;
      bonelab::Tuple<3,TVolumeSpace> m_VoxelSize;
      bonelab::Tuple<3,TVolumeSpace> m_VolumeOrigin;
      bool m_ProjectionsAtBothLimits;
      bool m_ReverseRotation;
      TVolumeValue m_ScalingFactor;
      filtering_module_t m_FilteringModule;
      pixel_interpolation_t m_PixelInterpolation;
      TransferFunction<TFunction1D>* m_SmoothingFilter;

    };  // class ReconstructionRunner

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ReconstructionRunner.txx"

#endif
