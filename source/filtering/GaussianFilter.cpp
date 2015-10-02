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

#include "GaussianFilter.hpp"
#include "AthabascaException.hpp"
#include "bonelab/Image.hpp"
#include <boost/math/constants/constants.hpp>
#include <cmath>

using boost::math::constants::pi;

template <typename T> inline T sqr(T x) {return x*x;}

namespace athabasca_recon
  {

  template <typename TArray>
  GaussianFilter<TArray>::GaussianFilter()
    :
    m_Sigma(0.5)
    {}

  template <typename TArray>
  void GaussianFilter<TArray>::SetRadius(typename GaussianFilter<TArray>::TValue arg)
    {
    this->m_Sigma = TValue(1)/(arg*pi<TValue>());
    }

  template <typename TArray>
  void GaussianFilter<TArray>::ConstructTransferFunction(TArray& f)
    {
    athabasca_assert(this->m_Length > 0);
    if (f.is_constructed())
      { athabasca_assert(f.size() == this->m_Length); }
    else
      { f.construct(this->m_Length); }

    f[0] = 1;
    TValue a = -2.0/sqr(this->m_Length*this->m_Sigma);
    TIndex halfLength = this->m_Length/2;
    for (TIndex i=1; i<=halfLength; ++i)
      { f[this->m_Length-i] = f[i] = exp(a*sqr(static_cast<TValue>(i))); }
    }

  // explicit instantiations
  template class GaussianFilter<bonelab::Array<1,float,int> >;
  template class GaussianFilter<bonelab::Array<1,double,int> >;

  } // namespace athabasca_recon
