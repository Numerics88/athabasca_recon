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

#ifndef BONELAB_fftw_mt_hpp_INCLUDED
#define BONELAB_fftw_mt_hpp_INCLUDED

#include <fftw3.h>
#include <boost/thread.hpp>

namespace bonelab
  {

  template<typename T> class fftw_mt_types;

  template <> class fftw_mt_types<float>
    {
    public:
    typedef fftwf_plan plan;
    typedef fftwf_complex complex;
    };

  template <> class fftw_mt_types<double>
    {
    public:
    typedef fftw_plan plan;
    typedef fftw_complex complex;
    };

  /** A thread-safe and templated interface to the fftw library.
    *
    * FFTW functions are *not* thread-safe, except for execute.  This class
    * uses a static mutex member to serialize access to all fftw functions
    * (except for execute).
    *
    * The functions in this class are named identically to fftw, except 
    * without the fftw_ prefix (or the fftwf_ prefix for single-precision).
    *
    * Refer to the fftw documentation for documentation of particular functions.
    *
    * Obviously not complete.  Add functions as needed.
    */
  template<typename T>
  class fftw_mt
    {
    protected:

      static boost::mutex m_Mutex;

    public:

      typedef typename fftw_mt_types<T>::complex complex;
      typedef typename fftw_mt_types<T>::plan plan;

      static plan plan_dft_r2c_1d(
          int n,
          T *in,
          complex *out,
          unsigned flags);
    
      static plan plan_dft_c2r_1d(
          int n,
          complex *in,
          T *out,
          unsigned flags);
    
      static void execute(const plan p);

      static void execute_dft_r2c(
          const plan p,
          T *in,
          complex *out);
    
      static void destroy_plan(const plan p);

    };  // class fftw_mt<T>

  }  // namespace bonelab

#endif

