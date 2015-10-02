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

#ifndef BONELAB_TransferFunction_HPP_INCLUDED
#define BONELAB_TransferFunction_HPP_INCLUDED

#include "bonelab/Array.hpp"
#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** An abstract class for generators of real-valued transfer functions.
    *
    * The method ConstructTransferFunction will generate an array of sample
    * points for the real-valued transfer function (in k-space).
    *
    * Setting the Length sets not only the number of values to be generated,
    * but also specifies the Nyquist frequency as Length/2.  This value
    * is used typically in the construction of the filter.  Note then that
    * indices greater than Length/2 actually correspond to negative k-values;
    * for symmetric filters this should mirror the values below Length/2.
    */
  template <typename TArray>
  class TransferFunction : private boost::noncopyable
    {
    public:

      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      /** Default constructor. */
      TransferFunction()
        :
        m_Length(0)
        {}

      virtual ~TransferFunction() {}

      /** Set/get the filter length.
        * Note that the Nyquist frequency is Length/2 .
        * The default is 0, which is invalid, so this must be set before
        * calling ConstructTransferFunction.
        */
      void SetLength(TIndex arg)  { this->m_Length = arg; }
      TIndex GetLength() const { return this->m_Length; }

      /** Generates an array of Length sample points of the function.
        * f must either be pre-constructed with size Length, or will be
        * be constructed with this size. 
        */
      virtual void ConstructTransferFunction(TArray& f) = 0;
    
    protected:
      
      TIndex m_Length;

    };  // class TransferFunction

  } // namespace athabasca_recon

#endif
