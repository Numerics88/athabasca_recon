/*
Copyright (C) 2011-2015 Eric Nodwell
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

#include "projection_correction/AttenuationCalculator.hpp"
#ifdef USE_VDSP
#include "filtering/ProjectionFilterer_vDSP.hpp"
#endif
#ifdef USE_FFTW
#include "filtering/ProjectionFilterer_fftw.hpp"
#endif
#include "filtering/ProjectionFilterer_Convolution.hpp"
#include "back_projection/ParallelBackProjector_cpu.hpp"
#include "back_projection/PixelInterpolator.hpp"
#include "back_projection/RayTracer.hpp"
#include "util/LinearFit.hpp"
#include "AthabascaException.hpp"
#include "bonelab/Image.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/math/constants/constants.hpp>

using boost::scoped_ptr;
using boost::format;
namespace constants = boost::math::constants;

namespace athabasca_recon
  {

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void SingleThreadedReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>
  ::Execute()
    {
    // Require exactly one type of projections reader.
    athabasca_assert(((this->m_RawProjectionsReader != NULL) + 
                      (this->m_AttenuationProjectionsReader != NULL) + 
                      (this->m_FilteredProjectionsReader != NULL)) == 1);

    // Create data objects.  Not all of these will necessarily be used.
    TProjectionIn raw_projection;
    TProjectionOut attenuation_projection;
    TProjectionOut filtered_projection;

    int numberOfProjections = 0;
    if (this->m_RawProjectionsReader)
      {
      raw_projection.construct(
          this->m_RawProjectionsReader->GetDimensions(),
          this->m_RawProjectionsReader->GetPixelSize(),
          this->m_RawProjectionsReader->GetOrigin());
      attenuation_projection.construct(
          this->m_RawProjectionsReader->GetDimensions(),
          this->m_RawProjectionsReader->GetPixelSize(),
          this->m_RawProjectionsReader->GetOrigin());
      if (this->m_FilteredProjectionsWriter || this->m_VolumeWriter)
        {
        filtered_projection.construct(
            this->m_RawProjectionsReader->GetDimensions(),
            this->m_RawProjectionsReader->GetPixelSize(),
            this->m_RawProjectionsReader->GetOrigin());
        }
      numberOfProjections = this->m_RawProjectionsReader->GetNumberOfProjections();
      }
    else if (this->m_AttenuationProjectionsReader)
      {
      attenuation_projection.construct(
          this->m_AttenuationProjectionsReader->GetDimensions(),
          this->m_AttenuationProjectionsReader->GetPixelSize(),
          this->m_AttenuationProjectionsReader->GetOrigin());
      filtered_projection.construct(
          this->m_AttenuationProjectionsReader->GetDimensions(),
          this->m_AttenuationProjectionsReader->GetPixelSize(),
          this->m_AttenuationProjectionsReader->GetOrigin());
      numberOfProjections = this->m_AttenuationProjectionsReader->GetNumberOfProjections();
      }
    else if (this->m_FilteredProjectionsReader)
      {
      filtered_projection.construct(
          this->m_FilteredProjectionsReader->GetDimensions(),
          this->m_FilteredProjectionsReader->GetPixelSize(),
          this->m_FilteredProjectionsReader->GetOrigin());
      numberOfProjections = this->m_FilteredProjectionsReader->GetNumberOfProjections();
      }

    // Determine angle increment
    TSpace deltaTheta;
    if (this->m_ProjectionsAtBothLimits)
      { deltaTheta = constants::pi<TSpace>()/(numberOfProjections-1); }
    else
      { deltaTheta = constants::pi<TSpace>()/numberOfProjections; }
    if (this->m_ReverseRotation)
      { deltaTheta = -deltaTheta; }
    
    // Create processing objects.  Not all of these will necessarily be used.
    AttenuationCalculator<TProjectionIn,TProjectionOut> attenuationCalculator;
    scoped_ptr<ProjectionFilterer<TProjectionOut> > filterer;
    scoped_ptr<ParallelBackProjector<TProjectionOut, TVolume> > backProjector;

    if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_NEAREST)
      {
      backProjector.reset(new ParallelBackProjector_cpu<TProjectionOut, TVolume,
          NearestNeighborPixelInterpolator<TProjectionOut>,
          ParallelRayTracer<TSpace> >);
      }
    else if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_BILINEAR)
      {
      backProjector.reset(new ParallelBackProjector_cpu<TProjectionOut, TVolume,
          BilinearPixelInterpolator<TProjectionOut>,
          ParallelRayTracer<TSpace> >);
      }
    else if (this->m_PixelInterpolation == PIXEL_INTERPOLATION_BILINEAR_WITH_FALLBACK)
      {
      backProjector.reset(new ParallelBackProjector_cpu<TProjectionOut, TVolume,
          BilinearPixelInterpolatorWithFallback<TProjectionOut>,
          ParallelRayTracer<TSpace> >);
      }
    else
      { throw_athabasca_exception("Internal Error."); }

    if (this->m_RawProjectionsReader)
      {
      attenuationCalculator.SetCalibration(this->m_Calibration);
      }

    if ((!this->m_FilteredProjectionsReader) &&
        (this->m_FilteredProjectionsWriter || this->m_VolumeWriter))
      {
      if (this->m_FilteringModule == FILTERING_MODULE_CONVOLUTION)
        { filterer.reset(new ProjectionFilterer_Convolution<TProjectionOut>); }
#ifdef USE_VDSP
      else if (this->m_FilteringModule == FILTERING_MODULE_VDSP)
        { filterer.reset(new ProjectionFilterer_vDSP<TProjectionOut>); }
#endif
#ifdef USE_FFTW
      else if (this->m_FilteringModule == FILTERING_MODULE_FFTW)
        { filterer.reset(new ProjectionFilterer_fftw<TProjectionOut>); }
#endif
      else
        { throw_athabasca_exception("Internal error\n"); }
      filterer->SetDimensions(filtered_projection.dims());
      filterer->SetPixelSpacing(filtered_projection.spacing()[1]);
      filterer->SetWeight(fabs(deltaTheta));
      filterer->SetSmoothingFilter(this->m_SmoothingFilter);
      filterer->Initialize();
      }

    if (this->m_VolumeWriter)
      {
      backProjector->SetScalingFactor(this->m_ScalingFactor);
      backProjector->AllocateVolume(this->m_VolumeDims, this->m_VoxelSize, this->m_VolumeOrigin);
      }

    // Allocate some space for saving values
    bonelab::Array<1,TProjectionOutValue,TIndex> corrections(numberOfProjections);

    this->Print("\n");
    for (int p=0; p < numberOfProjections; p++)
      {
      TSpace angle = p*deltaTheta;
      this->Print(format("Projection %d at angle %.2f : ")
                    % p % (angle*180/constants::pi<TSpace>()));

      if (this->m_RawProjectionsReader)
        {
        this->Print("R ");
        this->m_RawProjectionsReader->GetNextProjection(raw_projection);
        this->Print("P ");
        attenuationCalculator.ProcessProjection(raw_projection, attenuation_projection);
        if (this->m_BadPixelCorrector)
          {
          this->Print("C ");
          this->m_BadPixelCorrector->CorrectBadPixels(attenuation_projection);
          }
        if (this->m_BeamPowerCorrector)
          {
          this->Print("X ");
          if (p == 0)
            { this->m_BeamPowerCorrector->SetReferenceProjection(attenuation_projection); }
          corrections[p] = this->m_BeamPowerCorrector->ProcessProjection(
                                                    p, attenuation_projection);
          if (this->m_AttenuationCorrectionsFile)
            { *this->m_AttenuationCorrectionsFile << corrections[p] << '\n'; }
          }
        if (this->m_AttenuationProjectionsWriter)
          {
          this->Print("W ");
          this->m_AttenuationProjectionsWriter->WriteSlice(attenuation_projection);
          }
        }
      if (this->m_AttenuationProjectionsReader)
        {
        this->Print("R ");
        this->m_AttenuationProjectionsReader->GetNextProjection(attenuation_projection);
        }
      if (this->m_FilteredProjectionsReader)
        {
        this->Print("R ");
        this->m_FilteredProjectionsReader->GetNextProjection(filtered_projection);
        }
      else if (this->m_FilteredProjectionsWriter || this->m_VolumeWriter)
        {
        this->Print("F ");
        filterer->FilterProjection(attenuation_projection, filtered_projection);
        if (this->m_FilteredProjectionsWriter)
          {
          this->Print("W ");
          this->m_FilteredProjectionsWriter->WriteSlice(filtered_projection);
          }
        }
      if (this->m_VolumeWriter &&
         !(this->m_ProjectionsAtBothLimits && p == numberOfProjections-1))
        {
        this->Print("B ");
        backProjector->ApplyProjection(filtered_projection, angle);
        }
      this->Print("\n");
      }

    if (this->m_VolumeWriter)
      {
      std::cout << "Writing out volume data.\n";
      backProjector->WriteVolumeData(this->m_VolumeWriter);
      }

    if (this->m_AttenuationCorrectionsFile)
      {
      bonelab::Array<1,TProjectionOutValue,TIndex> x(numberOfProjections);
      for (TIndex i=0; i<numberOfProjections; ++i) {x[i] = i;}
      LinearFit<bonelab::Array<1,TProjectionOutValue,TIndex> >fit(x,corrections);
      std::cout << "Linear fit to beam power corrections gives "
                << fit.GetConstantTerm() << ", "
                << fit.GetLinearTerm() << " .\n";
      }

    }
  } // namespace athabasca_recon

