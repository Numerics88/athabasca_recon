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

#ifndef BONELAB_ParallelBackProjector_cpu_hpp_INCLUDED
#define BONELAB_ParallelBackProjector_cpu_hpp_INCLUDED

#include "ParallelBackProjector.hpp"

namespace athabasca_recon
  {

  /** A class for performing parallel backprojection on the CPU.
    *
    * The class is templated on both the PixelInterpolator and the RayTracer,
    * allowing various implementations of these with no runtime
    * evaluation of conditionals.
    */
  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  class ParallelBackProjector_cpu : public ParallelBackProjector<TProjection,TVolume>
    {
    public:

      typedef typename TVolume::value_type TVolumeValue;
      typedef typename TVolume::index_type TIndex;
      typedef typename TVolume::space_type TSpace;
      typedef typename bonelab::Image<2,TVolumeValue,TIndex,TSpace> TVolumeSlice;
      typedef StreamingWriter<TVolumeSlice> TVolumeWriter;

      /** Specifies the volume parameters and allocates the volume.
        *
        * @param volumeDims  the volume dimensions
        * @param voxelSize  the volume voxel spacing
        * @param volumeOrigin  the volume origin
        */
      void AllocateVolume(bonelab::Tuple<3,TIndex> volumeDims,
                          bonelab::Tuple<3,TSpace> voxelSize,
                          bonelab::Tuple<3,TSpace> volumeOrigin);

      /** Back-projects one projection through the volume.
        *
        * The projection is positioned as follows: in the y-z plane as
        * specified by the origin and spacing of the projection, then
        * rotated about the z axis by the specified angle.
        *
        * @param proj  the projection to back-project.
        * @param angle  the angle of the projection.
        */
      void ApplyProjection(const TProjection& proj, TSpace angle);

      /** Returns a reference to the volume.
        * Only used for debugging at present.
        */
      TVolume& GetVolume() {return this->m_Volume;}
      
      /** Writes out the volume data using the specified VolumeWriter. */
      void WriteVolumeData(TVolumeWriter* writer);
      
   protected:
     
      TVolume m_Volume;

    };  // class ParallelBackProjector_cpu

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ParallelBackProjector_cpu.txx"

#endif
