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

/** Some functions for dealing with Endian issues. */

#include <boost/detail/endian.hpp>
#include <boost/type_traits.hpp>

namespace bonelab
  {
  
  /** Values specifying endianness. */  
  enum EEndian
    {
    LITTLE_ENDIAN_ORDER,
    BIG_ENDIAN_ORDER,
#ifdef BOOST_LITTLE_ENDIAN
    HOST_ENDIAN_ORDER = LITTLE_ENDIAN_ORDER
#elif defined BOOST_BIG_ENDIAN
    HOST_ENDIAN_ORDER = BIG_ENDIAN_ORDER
#else
#error "Can't determine system endianness."
#endif
    };

  /** Swaps the bytes of value. */
  template <class T, unsigned int size>
  inline T SwapBytes(T value)
    {
    union
      {
      T value;
      char bytes[size];
      } in, out;

    in.value = value;

    for (unsigned int i = 0; i < size/2; ++i)
      {
      out.bytes[i] = in.bytes[size - 1 - i];
      out.bytes[size - 1 - i] = in.bytes[i];
      }

    return out.value;
    }

  /** Templated function to swap the bytes of value.
    *
    * Note that it is determined at compile time whether a swap is required;
    * there is zero runtime overhead if not.
    *
    * This function should to be called like this:
    * @code
    *   int x = someValue;
    *   int i = EndianConversion<HOST_ENDIAN_ORDER, BIG_ENDIAN_ORDER>(x);
    * @endcode
    */
  template<EEndian from, EEndian to, class T>
  inline T EndianConversion(T value)
    {
    // BOOST_STATIC_ASSERT((sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8));
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value);

    // No swap if it is the same types.
    if (from == to)
       return value;

    return SwapBytes<T, sizeof(T)>(value);
    }

  /** Templated function to swap the bytes of values in a memory region.
    * Note that it is determined at compile time whether a swap is required;
    * there is zero runtime overhead if not.
    */
  template<EEndian from, EEndian to, class T>
  inline T* EndianConversion(T* ptr, size_t count)
    {
    // BOOST_STATIC_ASSERT((sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8));
    BOOST_STATIC_ASSERT(boost::is_arithmetic<T>::value);

    // No swap if it is the same types.
    if (from == to)
      { return ptr; }

    T* p = ptr;
    const T* end = ptr + count;
    while (p != end)
      {
      *p = SwapBytes<T, sizeof(T)>(*p);
      ++p;
      }
    return ptr;
    }

  }  // namespace bonelab
