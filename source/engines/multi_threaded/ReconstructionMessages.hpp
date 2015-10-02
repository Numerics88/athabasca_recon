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

#ifndef ATHABASCA_RECON_ReconstructionMessages_hpp_INCLUDED
#define ATHABASCA_RECON_ReconstructionMessages_hpp_INCLUDED

#include "io/AsynchronousIOProcessor.hpp"
#include "bonelab/Message.hpp"
#include "bonelab/Tuple.hpp"

// forward declarations
namespace bonelab {class ThreadWorker;}

namespace athabasca_recon
  {

  // forward declarations
  template <class TProjection> class ProjectionFilterer;
  template <class TProjectionIn, class TProjectionOut> class AttenuationCalculator;
  
  /** Message to create a thread worker in the thread.
    * This should be issed before any other message.
    * The ThreadRunner will de-allocate the ThreadWorker automatically.
    */
  template <class TProjection, class TVolume>
  class CreateThreadWorkerMessage : public bonelab::Message
    {
    public:
      virtual void Execute(bonelab::ThreadWorker*& object);
    };

  /** Message to create a back projector object in the thread.
    * Obviously you need to issue this message before attempting any
    * back-projection in the thread.
    */
  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  class CreateBackProjectorMessage : public bonelab::Message
    {
    public:
      CreateBackProjectorMessage(typename TVolume::value_type scalingFactor);
      virtual void Execute(bonelab::ThreadWorker*& object);
      
    protected:
      typename TVolume::value_type m_ScalingFactor;
    };

  /** Message to create a filtering object in the thread.
    * Obviously you need to issue this message before attempting any
    * filtering in the thread.
    */
  template <class TProjection, class TVolume, class TProjectionFilterer>
  class CreateProjectionFiltererMessage : public bonelab::Message
    {
    public:
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef bonelab::Array<1,TValue,TIndex> TFunction1D;

      CreateProjectionFiltererMessage(bonelab::Tuple<2,TIndex> dimensions,
                                      TSpace pixelSpacing,
                                      TValue weight,
                                      TransferFunction<TFunction1D>* smoothingFilter);
      virtual void Execute(bonelab::ThreadWorker*& object);
      
    protected:
      bonelab::Tuple<2,TIndex> m_Dimensions;
      TSpace m_PixelSpacing;
      TValue m_Weight;
      TransferFunction<TFunction1D>* m_SmoothingFilter;
    };

  /** Message to allocate a volume in the thread.
    * This message must be issued after CreateBackProjectorMessage and before
    * attempting any back-projection.
    */
  template <class TProjection, class TVolume>
  class AllocateVolumeMessage : public bonelab::Message
    {
    public:

      typedef typename TVolume::value_type TValue;
      typedef typename TVolume::index_type TIndex;
      typedef typename TVolume::space_type TSpace;

      AllocateVolumeMessage(bonelab::Tuple<3,TIndex> volumeDims,
                            bonelab::Tuple<3,TSpace> voxelSize,
                            bonelab::Tuple<3,TSpace> volumeOrigin);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      bonelab::Tuple<3,TIndex> m_VolumeDims;
      bonelab::Tuple<3,TSpace> m_VoxelSize;
      bonelab::Tuple<3,TSpace> m_VolumeOrigin;
    };

  /** 
    */
  template <class TRawProjection, class TProcessedProjection>
  class WaitForBufferReleaseMessage : public bonelab::Message
    {
    public:

      WaitForBufferReleaseMessage(AsynchronousIOProcessor<TRawProjection,TProcessedProjection>& ioSerializer,
                         void* data);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      AsynchronousIOProcessor<TRawProjection,TProcessedProjection>& m_AsynchronousIOProcessor;
      void* m_Data;
    };

  /** Message to convert a raw projection to an attenuation projection.
    * As currently implemented, the AttenuationCalculator is not a thread-
    * local resource; you must provide one.
    */
  template <class TProjectionIn, class TProjectionOut>
  class ConvertToAttenuationMessage : public bonelab::Message
    {
    public:

      ConvertToAttenuationMessage(const AttenuationCalculator<TProjectionIn,TProjectionOut>& attenuationCalculator,
                                  const TProjectionIn& rawProjection,
                                  TProjectionOut& attenuationProjection);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      const AttenuationCalculator<TProjectionIn,TProjectionOut>& m_AttenuationCalculator;
      const TProjectionIn& m_RawProjection;
      TProjectionOut& m_AttenuationProjection;
    };

  /** Message to correct bad pixels in a raw projection.
    * As currently implemented, the BadPixelCorrector is not a thread-
    * local resource; you must provide one.
    */
  template <class TProjection>
  class CorrectBadPixelsMessage : public bonelab::Message
    {
    public:

      CorrectBadPixelsMessage(const BadPixelCorrector<TProjection>& corrector,
                              TProjection& projection);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      const BadPixelCorrector<TProjection>& m_BadPixelCorrector;
      TProjection& m_Projection;
    };

  /** Message to apply beam power correction to an attenuation projection.
    * As currently implemented, the BeamPowerCorrector is not a thread-
    * local resource; you must provide one.
    */
  template <class TProjection>
  class CorrectBeamPowerMessage : public bonelab::Message
    {
    public:

      CorrectBeamPowerMessage(const BeamPowerCorrector<TProjection>& beamPowerCorrector,
                              int index,
                              TProjection& projection,
                              double* correction);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      const BeamPowerCorrector<TProjection>& m_BeamPowerCorrector;
      int m_Index;
      TProjection& m_Projection;
      double* m_Correction;
    };

  /** Message to filter an attenuation projection.
    * These messages should only be issued after CreateProjectionFiltererMessage.
    */
  template <class TProjection, class TVolume>
  class FilterProjectionMessage : public bonelab::Message
    {
    public:

      FilterProjectionMessage(const TProjection& attenuationProjection,
                              TProjection& filteredProjection);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      const TProjection& m_AttenuationProjection;
      TProjection& m_FilteredProjection;
    };

  /** Message to back-project a projection.
    * You must previously have issued AllocateVolumeMessage.
    */
  template <class TProjection, class TVolume>
  class ApplyProjectionMessage : public bonelab::Message
    {
    public:

      typedef typename TVolume::space_type TSpace;

      ApplyProjectionMessage(const TProjection& proj, TSpace angle);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      const TProjection& m_Projection;
      TSpace m_Angle;
    };

  /** Message to write out the volume using the specified file writer.
    * You must previously have issued AllocateVolumeMessage.
    */
  template <class TProjection, class TVolume>
  class WriteVolumeDataMessage : public bonelab::Message
    {
    public:

      typedef typename TVolume::value_type TValue;
      typedef typename TVolume::index_type TIndex;
      typedef typename TVolume::space_type TSpace;
      typedef typename bonelab::Image<2,TValue,TIndex,TSpace> TVolumeSlice;
      typedef StreamingWriter<TVolumeSlice> TVolumeWriter;

      WriteVolumeDataMessage(TVolumeWriter* writer);
      virtual void Execute(bonelab::ThreadWorker*& object);

    protected:

      TVolumeWriter* m_Writer;
    };

  }  // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ReconstructionMessages.txx"

#endif
