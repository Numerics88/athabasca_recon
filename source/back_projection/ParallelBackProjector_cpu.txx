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

#include "AthabascaException.hpp"
#include <cmath>

template <typename T> inline T sqr(T x) { return x*x; }

namespace athabasca_recon
  {

  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  void ParallelBackProjector_cpu<TProjection,TVolume,TPixelInterpolator,TRayTracer>
  ::AllocateVolume
    (
    bonelab::Tuple<3,TIndex> volumeDims,
    bonelab::Tuple<3,TSpace> voxelSize,
    bonelab::Tuple<3,TSpace> volumeOrigin
    )
    {
    athabasca_assert(!this->m_Volume.is_constructed());
    athabasca_assert((volumeDims[1] > 0) &&
                     (volumeDims[2] > 0));
    athabasca_assert((voxelSize[0] > 0) &&
                     (voxelSize[1] > 0) &&
                     (voxelSize[2] > 0));
    // Zero slices is allowed.
    if (volumeDims[0] == 0)
      { return; }
    this->m_Volume.construct(volumeDims, voxelSize, volumeOrigin);
    }

  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  void ParallelBackProjector_cpu<TProjection,TVolume,TPixelInterpolator,TRayTracer>
  ::ApplyProjection
    (
    const TProjection& proj,
    TSpace angle
    )
    {
    // Can legitimately be not constructed if zero slices were requested.
    if (!this->m_Volume.is_constructed())
      { return; }

    TSpace circleRadius2 = sqr(std::min(-proj.interior_extents()[1][0], proj.interior_extents()[1][1]));
    bonelab::Tuple<2,TSpace> reverse_normal(-sin(angle), cos(angle));
    // Store values used in inner loop in temporary variables for speed
    TIndex Nx = this->m_Volume.dims()[2];
    TSpace origin_x = this->m_Volume.origin()[2];
    TSpace spacing_x = this->m_Volume.spacing()[2];
    bonelab::Tuple<3,TSpace> position;
    for (TIndex i=0; i<this->m_Volume.dims()[0]; i++)
      {
      position[0] = this->m_Volume.origin()[0] + i*m_Volume.spacing()[0];
      if (position[0] <= proj.exterior_extents()[0][0])
        { continue; }
      if (position[0] >= proj.exterior_extents()[0][1])
        { break; }
      for (TIndex j=0; j<this->m_Volume.dims()[1]; j++)
        {
        position[1] = this->m_Volume.origin()[1] + j*m_Volume.spacing()[1];
        TSpace circleRadiusX = circleRadius2 - sqr(position[1]);
        if (circleRadiusX > 0)
          {
          circleRadiusX = sqrt(circleRadiusX);
          TIndex k = std::max(0, TIndex(floor((-circleRadiusX-origin_x)/spacing_x)));
          TIndex k_stop = std::min(Nx, TIndex(ceil((circleRadiusX-origin_x)/spacing_x)));
          position[2] = origin_x + k*spacing_x;
          TVolumeValue* vol = &(this->m_Volume(i,j,k));
          TVolumeValue* vol_end = vol + (k_stop-k);
          bonelab::Tuple<2,TSpace> intersection(position[0],
                 reverse_normal[0]*position[2] + reverse_normal[1]*position[1]);
          TSpace intersection_du = reverse_normal[0]*spacing_x;
          while (vol < vol_end)
            {
            // Note: Some future version that stores a minimal local copy
            //       of the projection will probably apply ScalingFactor to that
            //       for efficiency.

            // Note: RayTracer static function replaced with code here, because
            // then we can preserve values for the next iteration.
            //
            // *vol += this->m_ScalingFactor * TPixelInterpolator::Interpolate(
            //             proj,
            //             TRayTracer::Intersect(position, reverse_normal));

            *vol += this->m_ScalingFactor * TPixelInterpolator::Interpolate(
                                                          proj,intersection);

            // ++k;    // Not required
            // position[2] += spacing_x;    // Not required
            intersection[1] += intersection_du;
            ++vol;
            }
          }
        }
      }
    }

  template <class TProjection, class TVolume, class TPixelInterpolator, class TRayTracer>
  void ParallelBackProjector_cpu<TProjection,TVolume,TPixelInterpolator,TRayTracer>
  ::WriteVolumeData(TVolumeWriter* writer)
    {
    // Can legitimately be not constructed if zero slices were requested.
    if (!this->m_Volume.is_constructed())
      { return; }
    writer->WriteStack(this->m_Volume);
    }

  }  // namespace athabasca_recon
