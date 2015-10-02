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

#ifndef BONELAB_TaperedCosineWindowFilter_HPP_INCLUDED
#define BONELAB_TaperedCosineWindowFilter_HPP_INCLUDED

#include "TransferFunction.hpp"
 
namespace athabasca_recon
  {

  /** A tapered-cosine smoothing window function.
    *
    * Note that a tapered-cosine smoothing window may introduce ripples near
    * sharp edges.  See the manual for a discussion.
    *
    * The tapered cosone window is given (in k-space) by:
    * @verbatim
    *  w(f) = 1                                    if |f| <= f1
    *         0.5 + 0.5*cos(pi*((|f|-f1)/(f2-f1))  if f1 < |f| < f2
    *         0                                    if |f| >= f2
    * @endverbatim
    * where f1 and f2 are the filter parameters.
    */
  template <typename TArray>
  class TaperedCosineWindowFilter : public TransferFunction<TArray>
    {
    public:

      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      TaperedCosineWindowFilter();

      /** Sets first filter frequency parameter (as a fraction of the Nyquist frequency). */
      void SetF1(TValue arg)  { this->m_F1 = arg; }
      TValue GetF1() const { return this->m_F1; }

      /** Sets second filter frequency parameter (as a fraction of the Nyquist frequency). */
      void SetF2(TValue arg)  { this->m_F2 = arg; }
      TValue GetF2() const { return this->m_F2; }

      virtual void ConstructTransferFunction(TArray& f);

    protected:
      
      TValue m_F1;
      TValue m_F2;
    
    };  // class TaperedCosineWindowFilter

  } // namespace athabasca_recon

#endif
