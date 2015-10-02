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

#ifndef BONELAB_RampFilterRealSpace_HPP_INCLUDED
#define BONELAB_RampFilterRealSpace_HPP_INCLUDED

#include "bonelab/Array.hpp"
#include <boost/noncopyable.hpp>
 
namespace athabasca_recon
  {

  /** Generates the real-space ramp kernel.
    *
    * The correct ramp function must be constructed in real-space, and not in
    * k-space.  Refer to the Athabasca manual for discussion.
    *
    * The ramp function kernel is:
    * @verbatim
    *   g[k] = 1/(4 delta t)           if k = 0
    *          0                       if k is even
    *          -1/(k^2 pi^2 delta t)   if k is odd
    * @endverbatim
    *
    * Note that
    * @verbatim
    *   1 + 1/3^2 + 1/5^2 + 1/7^2 + 1/9^2 + ..
    * @endverbatim
    * converges to pi^2/8 (slowly).
    *
    * The function is generated as a real-valued function of length Length,
    * centered at 0, with wrap-around.  Hence indices greater than Length/2
    * correspond to negative arguments.
    *
    * The ramp function should be generated to the length of the padded
    * FFT length.  It should not itself be zero-padded.
    */
  template <typename TArray>
  class RampFilterRealSpace : private boost::noncopyable
    {
    public:

      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      RampFilterRealSpace()
        :
        m_Length(0),
        m_Spacing(1),
        m_Weight(1)
        {}

      void SetLength(TIndex l)  { this->m_Length = l; }
      TIndex GetLength() const { return this->m_Length; }
      void SetSpacing(TValue spacing) { this->m_Spacing = spacing; }
      TValue GetSpacing() { return this->m_Spacing; }      
      void SetWeight(TValue weight) { this->m_Weight = weight; }
      TValue GetWeight() { return this->m_Weight; }

      void ConstructRealSpaceFilter(TArray& f);
    
    protected:
      
      TIndex m_Length;
      TValue m_Spacing;
      TValue m_Weight;

    };  // class RampFilterRealSpace

  } // namespace athabasca_recon

#endif
