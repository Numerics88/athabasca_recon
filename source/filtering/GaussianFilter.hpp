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

#ifndef BONELAB_GaussianFilter_HPP_INCLUDED
#define BONELAB_GaussianFilter_HPP_INCLUDED

#include "TransferFunction.hpp"
 
namespace athabasca_recon
  {

  /** A gaussian smoothing filter.
    *
    * Gaussian filters minimize the product delta x delta f, and are in that
    * sense ideal.  See the manual for discussion of this.
    *
    * In frequency space the Gaussian is
    * @verbatim
    *  1/(sigma sqrt(2 pi)) e^(-f^2/(2 sigma^2))
    * @endverbatim
    * sigma is the filter width.
    *
    * It has the same form in real-space, except with sigma being the filter
    * radius, R.  The relationship between sigma and R in units of pixels is:
    * @verbatim
    *    (sigma/f_Nyquist) = 1/(pi*R)
    * @endverbatim
    */
  template <typename TArray>
  class GaussianFilter : public TransferFunction<TArray>
    {
    public:

      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      GaussianFilter();

      /** Sets the filter radius in units of pixels.
        * This modifies the value of Sigma.
        */
      void SetRadius(TValue arg);
      
      /** Set/get the Gaussian width (as a fraction of the Nyquist frequency). */
      void SetSigma(TValue arg)  { this->m_Sigma = arg; }
      TValue GetSigma() const { return this->m_Sigma; }

      virtual void ConstructTransferFunction(TArray& f);

    protected:
      
      TValue m_Sigma;

    };  // class GaussianFilter

  } // namespace athabasca_recon

#endif
