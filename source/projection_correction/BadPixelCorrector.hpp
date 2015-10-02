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

#ifndef BONELAB_BadPixelCorrector_HPP_INCLUDED
#define BONELAB_BadPixelCorrector_HPP_INCLUDED

#include "bonelab/Image.hpp"
#include "bonelab/Tuple.hpp"
#include <boost/noncopyable.hpp>
#include <vector>

namespace athabasca_recon
  {

  /** Identifies and corrects bad pixels.
    *
    * Bad pixels are identified in three ways:
    *   1. The dark field is too bright.
    *   2. The flat field is too dark.
    *   3. The dark field is brighter than the dark field
    *     (i.e. the flat field is negative).
    * A threshold can be set for the first two methods.
    *
    * Bad pixels may either be zeroed, or replaced with a weighted average
    * of some close good pixels.
    */
  template <class TProjection>
  class BadPixelCorrector : private boost::noncopyable
    {
    public:

      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef std::vector<bonelab::Tuple<2,TIndex> > TBadPixelList;
      typedef std::vector<std::vector<bonelab::Tuple<2,TIndex> > > TCorrectionTable;

      /** Constructor.
        *
        * @param dims  The projection dimensions.
        */
      BadPixelCorrector(bonelab::Tuple<2,TIndex> dims);

      /** Sets the threshold above which pixels in the dark field will be
        * identified as bad.
        *
        * The default is the maximum value of TValue, which disables identification
        * of bad pixels from the dark field.
        */
      void SetDarkFieldBadThreshold(TValue arg) {this->m_DarkFieldBadThreshold = arg;}
      
      /** Sets the threshold below which pixels in the flat field are identified
        * as bad.
        *
        * The default is 10, which is reasonable for discrete data (i.e. if your
        * flat field is less than 10 counts, there is no dynamic range in
        * your signal at that pixel).  However, if your raw data is floating-point,
        * the value of 10 is likely to be inappropriate.
        */
      void SetFlatFieldBadThreshold(TValue arg) {this->m_FlatFieldBadThreshold = arg;}

      /** Identifies bad pixels from the given dark and flat fields.
        * This generates BadPixelList.  You still subsequently need to call
        * ConstructCorrectionTable before using CorrectBadPixels.
        */
      void IdentifyBadPixels(const TProjection& darkField, const TProjection& flatField);
      
      /** Returns the number of identified bad pixels.
        * You must previously have called IdentifyBadPixels.
        */
      int GetNumberOfBadPixels() {return int(m_BadPixelList.size());}

      /** Given a pixel index, returns true if it is a bad pixel.
        * You must previously have called IdentifyBadPixels.
        *
        * Note that this method is thread-safe, and may be called concurrently
        * from multiple threads.
        */
      bool IsBadPixel(bonelab::Tuple<2,TIndex> location) const;

      /** Constructs CorrectionTable.
        * The correction table is a weighted list for each bad pixel of
        * close good pixels that can be used as replacement values.
        * You must previously have called IdentifyBadPixels.
        * This method is inefficiently implemented, which is unimportant for
        * small numbers of bad pixels.  If the number of bad pixels is very
        * large, this call can take a considerable time to complete.
        */
      void ConstructCorrectionTable();

      /** Replaces all bad pixels from BadPixelList in the input projection
        * with the weighted replacement values from CorrectionTable.
        * you must previously have called ConstructCorrectionTable.
        *
        * Note that this method is thread-safe, and may be called concurrently
        * from multiple threads.
        */
      void CorrectBadPixels(TProjection& projection) const;

      /** Zeros all bad pixels from BadPixelList in the input projection.
        * You must previously have called IdentifyBadPixels.
        *
        * Note that this method is thread-safe, and may be called concurrently
        * from multiple threads.
        */
      void ZeroBadPixels(TProjection& projection) const;

    protected:
      
      bonelab::Tuple<2,TIndex> m_ImageDims;
      TValue m_DarkFieldBadThreshold;
      TValue m_FlatFieldBadThreshold;
      TBadPixelList m_BadPixelList;
      TCorrectionTable m_CorrectionTable;

    };  // class BadPixelCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "BadPixelCorrector.txx"

#endif
