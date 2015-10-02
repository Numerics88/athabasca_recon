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

#ifndef BONELAB_FFT_util_HPP_INCLUDED
#define BONELAB_FFT_util_HPP_INCLUDED

#include <algorithm>

namespace bonelab
  {

  /** Miscellaneous utility functions that are handy when working with FFTs. */
  class FFT_util
    {
    public:
    
      /** Returns the next power of 2 that is greater than or equal to n. */
      template <typename T>
      static inline unsigned int NextPowerOfTwo(T n)
        {
        unsigned int p = 0;
        while ((T(1) << p) < n) {p++;}
        return p;
        }
      
      /** Returns the smallest number that is greater than or equal to n, and
        * that has only 2 and 3 as prime factors. */
      template <typename T>
      static inline T NextNumberWithFactors2And3(T n)
        {
        unsigned int factorsOf2 = NextPowerOfTwo(n);
        T bestResult = T(1) << factorsOf2;
        T testResult = bestResult;
        while (factorsOf2 > 0)
          {
          factorsOf2--;
          testResult /= 2; 
          if (testResult < n)
            { testResult *= 3; }
          bestResult = std::min(bestResult, testResult);
          }
        return bestResult;
        }

    };  // class FFT_util

  } // namespace bonelab_recon

#endif
