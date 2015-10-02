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

#include "TaperedCosineWindowFilter.hpp"
#include "AthabascaException.hpp"
#include "bonelab/Image.hpp"
#include <boost/math/constants/constants.hpp>
#include <cmath>

using boost::math::constants::pi;

template <typename T> inline T sqr(T x) {return x*x;}

namespace athabasca_recon
  {

  template <typename TArray>
  TaperedCosineWindowFilter<TArray>::TaperedCosineWindowFilter()
    :
    m_F1(0.25),
    m_F2(0.5)
    {}

  template <typename TArray>
  void TaperedCosineWindowFilter<TArray>::ConstructTransferFunction(TArray& f)
    {
    athabasca_assert(this->m_Length > 0);
    if (f.is_constructed())
      { athabasca_assert(f.size() == this->m_Length); }
    else
      { f.construct(this->m_Length); }

    f[0] = 1;
    TIndex i = 1;
    TIndex halfLength = this->m_Length/2;
    TValue freq = (TValue)i/(TValue)halfLength;
    TValue a = pi<TValue>()/(this->m_F2 - this->m_F1);
    while ((freq <= this->m_F1) && (i < halfLength))
      {
      f[this->m_Length-i] = f[i] = 1;
      ++i;
      freq = (TValue)i/(TValue)halfLength;
      }
    while ((freq < this->m_F2) && (i < halfLength))
      {
      f[this->m_Length-i] = f[i] = (TValue)0.5 + (TValue)0.5*cos(a*(freq - this->m_F1));
      ++i;
      freq = (TValue)i/(TValue)halfLength;
      }
    while (i <= halfLength)
      {
      f[this->m_Length-i] = f[i] = 0;
      ++i;
      freq = (TValue)i/(TValue)halfLength;
      }

    }

  // explicit instantiations
  template class TaperedCosineWindowFilter<bonelab::Array<1,float,int> >;
  template class TaperedCosineWindowFilter<bonelab::Array<1,double,int> >;

  } // namespace athabasca_recon
