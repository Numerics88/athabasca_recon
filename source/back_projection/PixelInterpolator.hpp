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

#ifndef BONELAB_PixelInterpolator_hpp_INCLUDED
#define BONELAB_PixelInterpolator_hpp_INCLUDED

#include "bonelab/Image.hpp"
#include <boost/static_assert.hpp>
#include <cmath>

namespace athabasca_recon
  {

  /** A class containing a static method to perform nearest neighour pixel
    * interpolation.
    *
    * This class can be used as a template argument to ParallelBackProjector_cpu
    * to specify the type of pixel interpolation.
    */
  template <class TImage>
  class NearestNeighborPixelInterpolator
    {
    public:
      
      typedef typename TImage::value_type TValue;
      typedef typename TImage::index_type TIndex;
      typedef typename TImage::space_type TSpace;
      enum {N = TImage::dimension};

      // Only implemented for N=2 at the moment.
      BOOST_STATIC_ASSERT(N == 2);

      /** Interpolates an Image to the location u using nearest neighbor
        * interpolation.
        */
      inline static TValue Interpolate
        (
        const TImage& image,
        bonelab::Tuple<2,TSpace> u
        )
        {
        if ((u[0] <= image.exterior_extents()[0][0]) ||
            (u[0] >= image.exterior_extents()[0][1]) ||
            (u[1] <= image.exterior_extents()[1][0]) ||
            (u[1] >= image.exterior_extents()[1][1]))
          { return 0; }          
        TIndex index0 = (TIndex)((u[0] - static_cast<TSpace>(image.origin()[0]))/
                                  static_cast<TSpace>(image.spacing()[0]) + 0.5);
        TIndex index1 = (TIndex)((u[1] - static_cast<TSpace>(image.origin()[1]))/
                                  static_cast<TSpace>(image.spacing()[1]) + 0.5);
        return image(index0, index1);
        }

    };  // class NearestNeighborPixelInterpolator


  /** A class containing a static method to perform bilinear pixel
    * interpolation.
    *
    * This class can be used as a template argument to ParallelBackProjector_cpu
    * to specify the type of pixel interpolation.
    */
  template <class TImage>
  class BilinearPixelInterpolator
    {
    public:
      
      typedef typename TImage::value_type TValue;
      typedef typename TImage::index_type TIndex;
      typedef typename TImage::space_type TSpace;

      // Only implemented for N=2 at the moment.
      BOOST_STATIC_ASSERT(2 == TImage::dimension);

      /** Interpolates an Image to the location u using bilinear interpolation. */
      inline static TValue Interpolate
        (
        const TImage& image,
        bonelab::Tuple<2,TSpace> u
        )
        {
        if ((u[0] <= image.interior_extents()[0][0]) ||
            (u[0] >= image.interior_extents()[0][1]) ||
            (u[1] <= image.interior_extents()[1][0]) ||
            (u[1] >= image.interior_extents()[1][1]))
          { return 0; }          
        TSpace index_fp0 = (u[0] - static_cast<TSpace>(image.origin()[0]))/
                            static_cast<TSpace>(image.spacing()[0]);
        TSpace index_fp1 = (u[1] - static_cast<TSpace>(image.origin()[1]))/
                            static_cast<TSpace>(image.spacing()[1]);
        TIndex index0 = (TIndex)floor(index_fp0);
        TIndex index1 = (TIndex)floor(index_fp1);
        TSpace s0 = index_fp0 - floor(index_fp0);
        TSpace s1 = index_fp1 - floor(index_fp1);
        bonelab::Tuple<2,TSpace> t;
        TSpace t0 = (1-s1)*image(index0,index1) + s1*image(index0,index1+1);
        TSpace t1 = (1-s1)*image(index0+1,index1) + s1*image(index0+1,index1+1);
        return (1-s0)*t0 + s0*t1;
        }

    };  // class BilinearPixelInterpolator

  /** A class containing a static method to perform bilinear pixel
    * interpolation.
    *
    * This class can be used as a template argument to ParallelBackProjector_cpu
    * to specify the type of pixel interpolation.
    */
  template <class TImage>
  class BilinearPixelInterpolatorWithFallback
    {
    public:
      
      typedef typename TImage::value_type TValue;
      typedef typename TImage::index_type TIndex;
      typedef typename TImage::space_type TSpace;

      // Only implemented for N=2 at the moment.
      BOOST_STATIC_ASSERT(2 == TImage::dimension);

      /** Interpolates an Image to the location u using bilinear interpolation. */
      inline static TValue Interpolate
        (
        const TImage& image,
        bonelab::Tuple<2,TSpace> u
        )
        {
        if ((u[0] <= image.interior_extents()[0][0]) ||
            (u[0] >= image.interior_extents()[0][1]) ||
            (u[1] <= image.interior_extents()[1][0]) ||
            (u[1] >= image.interior_extents()[1][1]))
          {
          // If we are outside the interior extents, revert to Nearest-Neighbor;
          // this will give us an extra 1/2 pixel width on all sides.
          return NearestNeighborPixelInterpolator<TImage>::Interpolate(image, u);
          }          
        TSpace index_fp0 = (u[0] - static_cast<TSpace>(image.origin()[0]))/
                            static_cast<TSpace>(image.spacing()[0]);
        TSpace index_fp1 = (u[1] - static_cast<TSpace>(image.origin()[1]))/
                            static_cast<TSpace>(image.spacing()[1]);
        TIndex index0 = (TIndex)floor(index_fp0);
        TIndex index1 = (TIndex)floor(index_fp1);
        TSpace s0 = index_fp0 - floor(index_fp0);
        TSpace s1 = index_fp1 - floor(index_fp1);
        bonelab::Tuple<2,TSpace> t;
        TSpace t0 = (1-s1)*image(index0,index1) + s1*image(index0,index1+1);
        TSpace t1 = (1-s1)*image(index0+1,index1) + s1*image(index0+1,index1+1);
        return (1-s0)*t0 + s0*t1;
        }

    };  // class BilinearPixelInterpolatorWithFallback

  }  // namespace athabasca_recon
  
#endif
