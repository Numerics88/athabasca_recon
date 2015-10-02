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

#ifndef BONELAB_ParallelBackProjector_HPP_INCLUDED
#define BONELAB_ParallelBackProjector_HPP_INCLUDED

#include "bonelab/Image.hpp"
#include "io/StreamingWriter.hpp"
#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** An abstract base class for performing parallel backprojection.
    *
    * This class allocates and manages the volume. (Note that multiple instances
    * of ParallelBackProjector may each be managing a different subvolume.)
    */
  template <class TProjection, class TVolume>
  class ParallelBackProjector : private boost::noncopyable
    {
    public:

      typedef typename TVolume::value_type TVolumeValue;
      typedef typename TVolume::index_type TIndex;
      typedef typename TVolume::space_type TSpace;
      typedef typename bonelab::Image<2,TVolumeValue,TIndex,TSpace> TVolumeSlice;
      typedef StreamingWriter<TVolumeSlice> TVolumeWriter;

      ParallelBackProjector()
        : m_ScalingFactor(1.0) {}

      virtual ~ParallelBackProjector() {}

      /** Set the ScalingFactor.
        * The default value is 1.
        */
      virtual void SetScalingFactor(TVolumeValue arg) {this->m_ScalingFactor = arg;}

      /** Specifies the volume parameters and allocates the volume.
        *
        * @param volumeDims  the volume dimensions
        * @param voxelSize  the volume voxel spacing
        * @param volumeOrigin  the volume origin
        */
      virtual void AllocateVolume(bonelab::Tuple<3,TIndex> volumeDims,
                          bonelab::Tuple<3,TSpace> voxelSize,
                          bonelab::Tuple<3,TSpace> volumeOrigin) = 0;

      /** Back-projects one projection through the volume.
        *
        * The projection is positioned as follows: in the y-z plane as
        * specified by the origin and spacing of the projection, then
        * rotated about the z axis by the specified angle.
        *
        * @param proj  the projection to back-project.
        * @param angle  the angle of the projection.
        */
      virtual void ApplyProjection(const TProjection& proj, TSpace angle) = 0;

      /** Writes out the volume data using the specified VolumeWriter. */
      virtual void WriteVolumeData(TVolumeWriter* writer) = 0;
    
    protected:
    
      TVolumeValue m_ScalingFactor;

    };  // class ParallelBackProjector

  } // namespace athabasca_recon

#endif
