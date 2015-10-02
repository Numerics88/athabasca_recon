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
#include "LinearFit.hpp"

template <typename T>
inline T sqr(T x) {return x*x;}

template <class TArray>
inline typename TArray::value_type sum(const TArray& x)
  {
  typename TArray::value_type t = 0;
  for (typename TArray::index_type i=0; i<x.size(); ++i)
    { t += x[i]; }
  return t;
  }

template <class TArray>
inline typename TArray::value_type sum_sqr(const TArray& x)
  {
  typename TArray::value_type t = 0;
  for (typename TArray::index_type i=0; i<x.size(); ++i)
    { t += sqr(x[i]); }
  return t;
  }

template <class TArray>
inline typename TArray::value_type sum_prod(const TArray& x, const TArray& y)
  {
  typename TArray::value_type t = 0;
  for (typename TArray::index_type i=0; i<x.size(); ++i)
    { t += x[i]*y[i]; }
  return t;
  }

namespace athabasca_recon
  {

  template <typename TArray>
  LinearFit<TArray>::LinearFit(const TArray& x, const TArray& y)
    {
    athabasca_assert(x.is_constructed());
    athabasca_assert(y.is_constructed());
    size_t N = x.size();
    athabasca_assert(N == y.size());
    athabasca_assert(N > 1);
    
    // Assemble matrix X
    bonelab::Array<2,TValue,TIndex> X(2,2);
    X(0,0) = (TValue)N;
    X(1,0) = X(0,1) = sum(x);
    X(1,1) = sum_sqr(x);
    
    // Assemble vector Y
    bonelab::Array<1,TValue,TIndex> Y(2);
    Y[0] = sum(y);
    Y[1] = sum_prod(x,y);
    
    // Solution of X A = Y :
    typename TArray::value_type det = X(0,0)*X(1,1) - X(0,1)*X(1,0);
    this->m_a = (X(1,1)*Y[0] - X(0,1)*Y[1])/det;
    this->m_b = (X(0,0)*Y[1] - X(1,0)*Y[0])/det;
    }

  // explicit instantiations
  template class LinearFit<bonelab::Array<1,float,int> >;
  template class LinearFit<bonelab::Array<1,double,int> >;

  } // namespace athabasca_recon
