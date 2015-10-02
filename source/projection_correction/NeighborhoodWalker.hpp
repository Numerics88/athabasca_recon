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

#ifndef BONELAB_NeighborhoodWalker_HPP_INCLUDED
#define BONELAB_NeighborhoodWalker_HPP_INCLUDED

#include "bonelab/Image.hpp"
#include <boost/noncopyable.hpp>
#include <cstddef>  // for size_t

namespace athabasca_recon
  {

  /** Returns a sequence of neighboring pixels.
    *
    * This class is intended to aid in finding good pixels near bad pixels.
    * Given a starting point, each time GetNextLocation is called it returns
    * another location, never visiting the same one twice, and getting further
    * away from the starting point as required.
    *
    * It skips over points that are outside the specified image boundaries.
    *
    * As implemented, it walks a square of increasing size around the starting
    * location.
    */
  template <class TImage>
  class NeighborhoodWalker : private boost::noncopyable
    {
    public:

      typedef typename TImage::value_type TValue;
      typedef typename TImage::index_type TIndex;
      typedef typename TImage::space_type TSpace;

      /** Constructor.
        *
        * @param image_dims  The dimensions of the image.
        * @param starting_point  The coordinates of the starting point.
        */
      NeighborhoodWalker(const bonelab::Tuple<2,TIndex> image_dims,
                         const bonelab::Tuple<2,TIndex> starting_point);

      /** Return the next location on the walk around the starting point. */
      bonelab::Tuple<2,TIndex> GetNextLocation();

    protected:
      
      void TakeStep();
      bool LocationIsValid();

      bonelab::Tuple<2,TIndex> m_StartingPoint;
      bonelab::Tuple<2,TIndex> m_ImageDims;
      size_t m_Count;
      TIndex m_Radius;
      bonelab::Tuple<2,int> m_CurrentRelativeLocation;  // Need signed value
      bonelab::Tuple<2,int> m_CurrentAbsoluteLocation;  // Need signed value

    };  // class NeighborhoodWalker

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "NeighborhoodWalker.txx"

#endif
