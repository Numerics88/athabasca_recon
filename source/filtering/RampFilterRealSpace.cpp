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

#include "RampFilterRealSpace.hpp"
#include "AthabascaException.hpp"
#include "bonelab/Image.hpp"
#include <boost/math/constants/constants.hpp>

using boost::math::constants::pi;

template <typename T> inline T sqr(T x) {return x*x;}

namespace athabasca_recon
  {

  template <typename TArray>
  void RampFilterRealSpace<TArray>::ConstructRealSpaceFilter(TArray& f)
    {
    athabasca_assert(this->m_Length > 0);
    if (f.is_constructed())
      { athabasca_assert(f.size() == this->m_Length); }
    else
      { f.construct(this->m_Length); }
    f.zero();

// #define REPLACE_RAMP_FILTER_WITH_DELTA_FUNCTION
#ifdef REPLACE_RAMP_FILTER_WITH_DELTA_FUNCTION
    std::cout << "**** WARNING: Setting rampFilterRealSpace to delta function!\n";
    f[0] = this->m_Weight;
#else
    f[0] = this->m_Weight/(TValue(4.0) * this->m_Spacing);
    TValue C = -this->m_Weight/(sqr(pi<TValue>())*this->m_Spacing);
    // Only odd values are non-zero (other than k=0).
    // Width of only unpadded length in order to avoid wrap-around.
    TIndex halfLength = this->m_Length/2;
    for (TIndex i=1; i<=halfLength; i+=2)
      { f[this->m_Length-i] = f[i] = C/sqr(i); }
#endif
    }

  // explicit instantiations
  template class RampFilterRealSpace<bonelab::Array<1,float,int> >;
  template class RampFilterRealSpace<bonelab::Array<1,double,int> >;

  } // namespace athabasca_recon
