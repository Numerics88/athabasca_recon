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

#include "back_projection/ParallelBackProjector_cpu.hpp"
#include "ReconstructionThreadWorker.hpp"
#ifdef TRACE_THREADING
#include "bonelab/print_multithread.hpp"
#include <boost/format.hpp>
using bonelab::print_mt;
using boost::format;
#endif

namespace athabasca_recon
  {

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume>
  void CreateThreadWorkerMessage<TProjection,TVolume>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
                  new ReconstructionThreadWorker<TProjection,TVolume>();
    object = worker;
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  CreateBackProjectorMessage<TProjection,TVolume,TPixelInterpolator,TRayTracer>
  ::CreateBackProjectorMessage(typename TVolume::value_type scalingFactor)
    :
    m_ScalingFactor(scalingFactor)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating CreateBackProjectorMessage %d\n") % this);
#endif    
    }

  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  void CreateBackProjectorMessage<TProjection,TVolume,TPixelInterpolator,TRayTracer>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    worker->BackProjector = new ParallelBackProjector_cpu<TProjection,TVolume,TPixelInterpolator,TRayTracer>();
    worker->BackProjector->SetScalingFactor(this->m_ScalingFactor);
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume, class TProjectionFilterer>
  CreateProjectionFiltererMessage<TProjection,TVolume,TProjectionFilterer>
  ::CreateProjectionFiltererMessage
    (
    bonelab::Tuple<2,TIndex> dimensions,
    TSpace pixelSpacing,
    TValue weight,
    TransferFunction<TFunction1D>* smoothingFilter
    )
    :
    m_Dimensions(dimensions),
    m_PixelSpacing(pixelSpacing),
    m_Weight(weight),
    m_SmoothingFilter(smoothingFilter)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating CreateProjectionFiltererMessage %d\n") % this);
#endif
    }

  template <class TProjection, class TVolume, class TProjectionFilterer>
  void CreateProjectionFiltererMessage<TProjection,TVolume,TProjectionFilterer>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    TProjectionFilterer* filterer = new TProjectionFilterer();
    filterer->SetDimensions(this->m_Dimensions);
    filterer->SetPixelSpacing(this->m_PixelSpacing);
    filterer->SetWeight(this->m_Weight);
    filterer->SetSmoothingFilter(this->m_SmoothingFilter);
    filterer->Initialize();
    worker->ProjectionFilterer = filterer;
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume>
  AllocateVolumeMessage<TProjection,TVolume>
  ::AllocateVolumeMessage
    (
    bonelab::Tuple<3,TIndex> volumeDims,
    bonelab::Tuple<3,TSpace> voxelSize,
    bonelab::Tuple<3,TSpace> volumeOrigin
    )
    :
    m_VolumeDims(volumeDims),
    m_VoxelSize(voxelSize),
    m_VolumeOrigin(volumeOrigin)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating AllocateVolumeMessage %d\n") % this);
#endif
    }

  template <class TProjection, class TVolume>
  void AllocateVolumeMessage<TProjection,TVolume>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    athabasca_assert(worker->BackProjector);
    worker->BackProjector->AllocateVolume(this->m_VolumeDims, this->m_VoxelSize, this->m_VolumeOrigin);
    }

  //------------------------------------------------------------------------

  template <class TRawProjection, class TProcessedProjection>
  WaitForBufferReleaseMessage<TRawProjection,TProcessedProjection>
  ::WaitForBufferReleaseMessage
    (
    AsynchronousIOProcessor<TRawProjection,TProcessedProjection>& ioSerializer,
    void* data
    )
    :
    m_AsynchronousIOProcessor(ioSerializer),
    m_Data(data)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating WaitForBufferReleaseMessage %d\n") % this);
#endif
    }

  template <class TRawProjection, class TProcessedProjection>
  void WaitForBufferReleaseMessage<TRawProjection,TProcessedProjection>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    this->m_AsynchronousIOProcessor.WaitForBufferRelease(this->m_Data);
    }

  //------------------------------------------------------------------------

  template <class TProjectionIn, class TProjectionOut>
  ConvertToAttenuationMessage<TProjectionIn,TProjectionOut>
  ::ConvertToAttenuationMessage
    (
    const AttenuationCalculator<TProjectionIn,TProjectionOut>& attenuationCalculator,
    const TProjectionIn& rawProjection,
    TProjectionOut& attenuationProjection
    )
    :
    m_AttenuationCalculator(attenuationCalculator),
    m_RawProjection(rawProjection),
    m_AttenuationProjection(attenuationProjection)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating ConvertToAttenuationMessage %d\n") % this);
#endif      
    }

  template <class TProjectionIn, class TProjectionOut>
  void ConvertToAttenuationMessage<TProjectionIn,TProjectionOut>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    this->m_AttenuationCalculator.ProcessProjection(this->m_RawProjection, this->m_AttenuationProjection);
    }

  //------------------------------------------------------------------------

  template <class TProjection>
  CorrectBadPixelsMessage<TProjection>
  ::CorrectBadPixelsMessage(const BadPixelCorrector<TProjection>& corrector,
                            TProjection& projection)
    :
    m_BadPixelCorrector(corrector),
    m_Projection(projection)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating CorrectBadPixelsMessage %d\n") % this);
#endif    
    }

  template <class TProjection>
  void CorrectBadPixelsMessage<TProjection>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    this->m_BadPixelCorrector.CorrectBadPixels(this->m_Projection);
    }

  //------------------------------------------------------------------------

  template <class TProjection>
  CorrectBeamPowerMessage<TProjection>
  ::CorrectBeamPowerMessage(const BeamPowerCorrector<TProjection>& beamPowerCorrector,
                            int index,
                            TProjection& projection,
                            double* correction)
    :
    m_BeamPowerCorrector(beamPowerCorrector),
    m_Index(index),
    m_Projection(projection),
    m_Correction(correction)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating CorrectBeamPowerMessage %d\n") % this);
#endif
    }

  template <class TProjection>
  void CorrectBeamPowerMessage<TProjection>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    *this->m_Correction = this->m_BeamPowerCorrector.ProcessProjection(
                                         this->m_Index, this->m_Projection);
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume>
  FilterProjectionMessage<TProjection,TVolume>
  ::FilterProjectionMessage(const TProjection& attenuationProjection,
                            TProjection& filteredProjection)
    :
    m_AttenuationProjection(attenuationProjection),
    m_FilteredProjection(filteredProjection)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating FilterProjectionMessage %d\n") % this);
#endif
    }

  template <class TProjection, class TVolume>
  void FilterProjectionMessage<TProjection,TVolume>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    if (worker->ProjectionFilterer == NULL)
      { throw_athabasca_exception("ProjectionFilterer object not created."); }
    worker->ProjectionFilterer->FilterProjection(this->m_AttenuationProjection, this->m_FilteredProjection);
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume>
  ApplyProjectionMessage<TProjection,TVolume>
  ::ApplyProjectionMessage(const TProjection& proj, TSpace angle)
    :
    m_Projection(proj),
    m_Angle(angle)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating ApplyProjectionMessage %d\n") % this);
#endif
    }

  template <class TProjection, class TVolume>
  void ApplyProjectionMessage<TProjection,TVolume>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    athabasca_assert(worker->BackProjector);
    worker->BackProjector->ApplyProjection(this->m_Projection, this->m_Angle);
    }

  //------------------------------------------------------------------------

  template <class TProjection, class TVolume>
  WriteVolumeDataMessage<TProjection,TVolume>
  ::WriteVolumeDataMessage(TVolumeWriter* writer)
    :
    m_Writer(writer)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating WriteVolumeDataMessage %d\n") % this);
#endif    
    }

  template <class TProjection, class TVolume>
  void WriteVolumeDataMessage<TProjection,TVolume>
  ::Execute(bonelab::ThreadWorker*& object)
    {
    ReconstructionThreadWorker<TProjection,TVolume>* worker =
        dynamic_cast<ReconstructionThreadWorker<TProjection,TVolume>*>(object);
    if (worker == NULL)
      { throw_athabasca_exception("Invalid object for message."); }
    athabasca_assert(worker->BackProjector);
    worker->BackProjector->WriteVolumeData(this->m_Writer);
    }

  }  // namespace athabasca_recon
