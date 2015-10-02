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

#ifndef BONELAB_LinearFit_hpp_INCLUDED
#define BONELAB_LinearFit_hpp_INCLUDED

#include "bonelab/Array.hpp"
#include <boost/static_assert.hpp>

namespace athabasca_recon
  {

  /** A class to perform a linear least-squares fit to 1D data. */
  template <typename TArray>
  class LinearFit
    {
    public:
      
      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      BOOST_STATIC_ASSERT(1 == TArray::dimension);

      /** Constructor.
        *
        * @param x  The independent variable.
        * @param y  The dependent variable.
        */
      LinearFit(const TArray& x, const TArray& y);

      /** Returns the constant term of the fit. */
      TValue GetConstantTerm() {return this->m_a;}

      /** Returns the linear term of the fit. */
      TValue GetLinearTerm() {return this->m_b;}

    protected:
      
      TValue m_a;
      TValue m_b;

    };

  } // namespace athabasca_recon

#endif
