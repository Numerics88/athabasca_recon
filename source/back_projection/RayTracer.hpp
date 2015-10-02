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

#ifndef BONELAB_RayTracer_hpp_INCLUDED
#define BONELAB_RayTracer_hpp_INCLUDED

#include "bonelab/Tuple.hpp"

namespace athabasca_recon
  {

  /** A class containing a static method to perform ray tracing from a point
    * to a projection.
    *
    * This class can be used as a template argument to ParallelBackProjector_cpu
    * to specify the type of ray tracing.
    */
  template <class TSpace>
  class ParallelRayTracer
    {
    public:
         
      /** Returns the intersection point (u,v) of a ray with an abstract projection.
        *
        * The abstract projection is positioned as follows: in the y-z plane
        * centered at (0,0) with spacing (1,1) then
        * rotated about the z axis by the specified angle.
        *
        * @param position  a point on the ray.
        * @param reverse_normal  the normal vector to the ray, reflected about
        *                the x axis (so with negative angle).  Also gives the
        *                angle of the ray, since the ray is assumed to be
        *                perpendicular to the projections.
        */
      inline static bonelab::Tuple<2,TSpace> Intersect
        (
        const bonelab::Tuple<3,TSpace>& position,
        const bonelab::Tuple<2,TSpace>& reverse_normal
        )
        {
        return bonelab::Tuple<2,TSpace>(
                 position[0],
                 reverse_normal[0]*position[2] + reverse_normal[1]*position[1]);
        }

    };  // class ParallelRayTracer

  // template <class TSpace>
  // class SkewedProjectionParallelRayTracer
  //   {
  //   public:
  //     inline static bonelab::Tuple<2,TSpace> Intersect
  //       (
  //       const bonelab::Tuple<3,TSpace>& position,
  //       const bonelab::Tuple<2,TSpace>& normal,
  //       const bonelab::Tuple<2,TSpace>& inplane,
  //       TSpace sin_skew_angle
  //       )
  //       {
  //       bonelab::Tuple<2,TSpace> intersection = ParallelRayTracer<TSpace>::Intersect(
  //           position,normal,inplane,sin_skew_angle);
  //       intersection[1] -= intersection[0]*sin_skew_angle;
  //       return intersection;
  //       }
  //   };  // class SkewedProjectionParallelRayTracer
  // 
  
  // template <class TSpace>
  // class RotatedProjectionParallelRayTracer
  //   {
  //   public:
  //     inline static bonelab::Tuple<2,TSpace> Intersect
  //       (
  //       const bonelab::Tuple<3,TSpace>& position,
  //       const bonelab::Tuple<2,TSpace>& normal,
  //       const bonelab::Tuple<2,TSpace>& inplane,
  //       const TSpace rotationCos,
  //       const TSpace rotationSin
  //       )
  //       {
  //       bonelab::Tuple<2,TSpace> intersection = ParallelRayTracer<TSpace>::Intersect(
  //           position,normal,inplane,rotationCos,rotationSin);
  //       return bonelab::Tuple<2,TSpace>(
  //           intersection[0]*rotationCos + intersection[1]*rotationSin,
  //          -intersection[0]*rotationSin + intersection[1]*rotationCos);
  //       }
  //   };  // class RotatedProjectionParallelRayTracer

  }  // namespace athabasca_recon
  
#endif
