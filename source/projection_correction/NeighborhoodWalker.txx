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

namespace athabasca_recon
  {

  template <class TImage>
  NeighborhoodWalker<TImage>::NeighborhoodWalker
    (
    const bonelab::Tuple<2,TIndex> image_dims,
    const bonelab::Tuple<2,TIndex> starting_point
    )
    :
    m_ImageDims(image_dims),
    m_StartingPoint(starting_point),
    m_Count(0),
    m_Radius(1)
    {}

  template <class TImage>
  bonelab::Tuple<2,typename NeighborhoodWalker<TImage>::TIndex> NeighborhoodWalker<TImage>
  ::GetNextLocation()
    {
    this->TakeStep();
    while (!this->LocationIsValid())
      {
      athabasca_assert(this->m_Count < long_product(this->m_ImageDims));
      this->TakeStep();
      }
    return bonelab::Tuple<2,TIndex>(
               static_cast<TIndex>(this->m_CurrentAbsoluteLocation[0]),
               static_cast<TIndex>(this->m_CurrentAbsoluteLocation[1]));
    }

  template <class TImage>
  void NeighborhoodWalker<TImage>::TakeStep()
    {
    // The first 8 points have pre-assigned positions; after that, we use
    // a spiral walk pattern.
    if (this->m_Count < 8)
      {
      switch (this->m_Count)
        {
        case 0:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(1,0);
          break;
        case 1:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(0,1);
          break;
        case 2:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(-1,0);
          break;
        case 3:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(0,-1);
          break;
        case 4:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(1,1);
          break;
        case 5:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(-1,1);
          break;
        case 6:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(-1,-1);
          break;
        case 7:
          this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(1,-1);
          break;
        // case 8:
        //   // This is the starting point for the spiral walk
        //   this->m_CurrentRelativeLocation = bonelab::Tuple<2,int>(2,-1);
        //   this->m_Radius = 2;
        //   break;
        }
      }

    else  // m_Count >= 8
      {
      if (this->m_CurrentRelativeLocation[0] == this->m_Radius)
        {
        if (this->m_CurrentRelativeLocation[1] == this->m_Radius)
          { --(this->m_CurrentRelativeLocation[0]); }
        else if (this->m_CurrentRelativeLocation[1] == -this->m_Radius)
          {
          ++(this->m_CurrentRelativeLocation[0]);
          ++(this->m_Radius);
          }
        else
          { ++(this->m_CurrentRelativeLocation[1]); }
        }
      else if (this->m_CurrentRelativeLocation[0] == -this->m_Radius)
        {
        if (this->m_CurrentRelativeLocation[1] == -this->m_Radius)
          { ++(this->m_CurrentRelativeLocation[0]); }
        else
          { --(this->m_CurrentRelativeLocation[1]); }
        }
      else if (this->m_CurrentRelativeLocation[1] == this->m_Radius)
        { --(this->m_CurrentRelativeLocation[0]); }
      else if (this->m_CurrentRelativeLocation[1] == -this->m_Radius)
        { ++(this->m_CurrentRelativeLocation[0]); }
      else
        { throw_athabasca_exception("Internal error."); }
      }

    this->m_CurrentAbsoluteLocation = this->m_CurrentRelativeLocation +
                                      this->m_StartingPoint;
    ++(this->m_Count);
    }

  template <class TImage>
  bool NeighborhoodWalker<TImage>::LocationIsValid()
    {
    if ((this->m_CurrentAbsoluteLocation[0] < 0) ||
        (this->m_CurrentAbsoluteLocation[1] < 0) ||
        (this->m_CurrentAbsoluteLocation[0] >= this->m_ImageDims[0]) ||
        (this->m_CurrentAbsoluteLocation[1] >= this->m_ImageDims[1]))
      {
      return false;
      }
    return true;
    }

  } // namespace athabasca_recon


