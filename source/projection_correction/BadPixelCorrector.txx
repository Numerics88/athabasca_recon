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

#include "NeighborhoodWalker.hpp"
#include "AthabascaException.hpp"
#include <limits>

namespace athabasca_recon
  {

  template <class TProjection>
  BadPixelCorrector<TProjection>::BadPixelCorrector(bonelab::Tuple<2,TIndex> dims)
    :
    m_ImageDims(dims),
    m_DarkFieldBadThreshold(std::numeric_limits<TValue>::max()),
    m_FlatFieldBadThreshold(10)
    {}

  template <class TProjection>
  void BadPixelCorrector<TProjection>::IdentifyBadPixels
    (
    const TProjection& darkField,
    const TProjection& flatField
    )
    {
    athabasca_assert(darkField.is_constructed());
    athabasca_assert(flatField.is_constructed());
    athabasca_assert(darkField.dims() == flatField.dims());
    
    for (TIndex i=0; i<darkField.dims()[0]; ++i)
      {
      for (TIndex j=0; j<darkField.dims()[1]; ++j)
        {
        if ((darkField(i,j) > this->m_DarkFieldBadThreshold) ||
            (flatField(i,j) < this->m_FlatFieldBadThreshold))
          {
          this->m_BadPixelList.push_back(bonelab::Tuple<2,TIndex>(i,j));
          }
        }
      }
    }

  template <class TProjection>
  bool BadPixelCorrector<TProjection>::IsBadPixel(bonelab::Tuple<2,TIndex> location) const
    {
    for (typename TBadPixelList::const_iterator iter = this->m_BadPixelList.begin();
         iter != this->m_BadPixelList.end(); ++iter)
      { if (*iter == location) {return true;} }
    return false;
    }

  template <class TProjection>
  void BadPixelCorrector<TProjection>::ConstructCorrectionTable()
    {
    this->m_CorrectionTable.clear();
    for (typename TBadPixelList::const_iterator iter = this->m_BadPixelList.begin();
         iter != this->m_BadPixelList.end(); ++iter)
      {
      NeighborhoodWalker<TProjection> walker(this->m_ImageDims, *iter);
      std::vector<bonelab::Tuple<2,TIndex> > averaging_pixels;
      for (int j=0; j<4; ++j)
        {
        bonelab::Tuple<2,TIndex> location = walker.GetNextLocation();
        while (this->IsBadPixel(location))
          { location = walker.GetNextLocation(); }
        averaging_pixels.push_back(location);
        }
      this->m_CorrectionTable.push_back(averaging_pixels);
      }
    }
  
  template <class TProjection>
  void BadPixelCorrector<TProjection>::CorrectBadPixels
    (TProjection& projection) const
    {
    athabasca_assert(this->m_BadPixelList.size() == this->m_CorrectionTable.size());
    for (int i=0; i<this->m_BadPixelList.size(); ++i)
      {
      TValue x = 0;
      for (int j=0; j<this->m_CorrectionTable[i].size(); ++j)
        { x += projection(this->m_CorrectionTable[i][j]); }
      x /= this->m_CorrectionTable[i].size();
      projection(this->m_BadPixelList[i]) = x;
      }
    }

  template <class TProjection>
  void BadPixelCorrector<TProjection>::ZeroBadPixels
    (TProjection& projection) const
    {
    for (int i=0; i<this->m_BadPixelList.size(); ++i)
      {
      projection(this->m_BadPixelList[i]) = 0;
      }
    }

  } // namespace athabasca_recon
