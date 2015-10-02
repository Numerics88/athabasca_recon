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
#include "bonelab/Image.hpp"

namespace athabasca_recon
  {

  template <typename TProjectionIn, typename TProjectionOut>
  AttenuationCalculator<TProjectionIn,TProjectionOut>::AttenuationCalculator()
    :
    m_Calibration(NULL)
    {}

  template <typename TProjectionIn, typename TProjectionOut>
  void AttenuationCalculator<TProjectionIn,TProjectionOut>::ProcessProjection
    (
    const TProjectionIn& in,
    TProjectionOut& out
    ) const
    {
    athabasca_assert(this->m_Calibration);
    athabasca_assert(out.dims()[0] == in.dims()[0]);
    athabasca_assert(out.dims()[1] >= in.dims()[1]);
    // The following assignments might avoid unnecessary
    // dereferencing inside the loop.  (With some compilers it is
    // unnecessary, but does no harm.)
    const TProjectionOut& dark = this->m_Calibration->GetDarkField();
    const TProjectionOut& flat = this->m_Calibration->GetFlatField();
    athabasca_assert(in.dims() == dark.dims());
    athabasca_assert(in.dims() == flat.dims());
    TProjectionOutValue sensitivity_limit = this->m_Calibration->GetSensitivityLimit();
    TIndex N = in.dims()[0];
    TIndex M_in = in.dims()[1];
    TIndex M_out = out.dims()[1];
    // TO DO:
    // The following loop can probably be broken into a number of
    // loops each performing one simple operation, for a total gain in speed.
    // (May permit SSE).
    for (TIndex i=0; i<N; i++)
      {
      TIndex j=0;
      for (; j<M_in; j++)
        {
        // Notes:
        //  1. It is important to convert to a float format before doing the
        //     division; TOut is always a floating point format.
        //  2. It is also important to convert to a signed quantity before
        //     doing the subtraction; TOut is fine, but TIn may not be.
        //  3. It is important to convert to TOut before taking the
        //     log, as we want to avoid calculating the log to double
        //     precision if we only need a single-precision answer.
        //  4. The end result could still be (slightly) negative, even for
        //     good pixels, due to fluctuations above the bright field, but
        //     this is OK.  It is statistically undesirable to clamp these values.
        //  5. Bad pixels may end up as NaNs.  Subsequent processing with
        //     a bad pixel map is required to remove these.
        TProjectionOutValue x = static_cast<TProjectionOutValue>(in(i,j)) - dark(i,j);
        x = std::max(x, sensitivity_limit);
        out(i,j) = log(flat(i,j)/x);
        }
      // The following will ensure that if out is larger than in, that the
      // extra padding is set to zero.
      // Actually, the current design does not pad out at this point.
      // (Now happens at the FFT stage.)
      for (; j<M_out; j++)
        {
        out(i,j) = 0;
        }
      }
    }

  }  // namespace athabasca_recon
