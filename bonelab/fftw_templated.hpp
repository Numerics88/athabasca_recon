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

#ifndef BONELAB_fftw_templated_hpp_INCLUDED
#define BONELAB_fftw_templated_hpp_INCLUDED

#include <fftw3.h>

namespace bonelab
  {

  template<typename T> class fftw_types;

  template <> class fftw_types<float>
    {
    public:
    typedef fftwf_plan plan;
    typedef fftwf_complex complex;
    };

  template <> class fftw_types<double>
    {
    public:
    typedef fftw_plan plan;
    typedef fftw_complex complex;
    };

  /** A templated interface to the fftw library.
    *
    * Contains static member functions that map to functions in fftw.
    * The only purpose to this class is to make it easier to write templated
    * code using fftw, which uses different function names for float and
    * double versions of functions.
    *
    * The functions in this class are named identically to fftw, except 
    * without the fftw_ prefix (or the fftwf_ prefix for single-precision).
    *
    * Refer to the fftw documentation for documentation of particular functions.
    *
    * Obviously not complete.  Add functions as needed.
    *
    * WARNING: This version is not thread-safe.  Use fftw_mt if you require
    * thread safety.
    */
  template<typename T>
  class fftw
    {
    public:
      typedef typename fftw_types<T>::complex complex;
      typedef typename fftw_types<T>::plan plan;

      static inline plan plan_dft_r2c_1d(
          int n,
          T *in,
          complex *out,
          unsigned flags);
    
      static inline plan plan_dft_c2r_1d(
          int n,
          complex *in,
          T *out,
          unsigned flags);
    
      static inline void execute(const plan p);

      static inline void execute_dft_r2c(
          const plan p,
          T *in,
          complex *out);
    
      static inline void destroy_plan(const plan p);
    };  // class fftw<T>

    template <>
    inline fftw<float>::plan fftw<float>::plan_dft_r2c_1d
      (int n, float *in, complex *out, unsigned flags)
      {
      return fftwf_plan_dft_r2c_1d(n, in, out, flags);
      }

    template <>
    inline fftw<double>::plan fftw<double>::plan_dft_r2c_1d
      (int n, double *in, complex *out, unsigned flags)
      {
      return fftw_plan_dft_r2c_1d(n, in, out, flags);
      }

    template <>
    inline fftw<float>::plan fftw<float>::plan_dft_c2r_1d
      (int n, complex *in, float *out, unsigned flags)
      {
      return fftwf_plan_dft_c2r_1d(n, in, out, flags);
      }

    template <>
    inline fftw<double>::plan fftw<double>::plan_dft_c2r_1d
      (int n, complex *in, double *out, unsigned flags)
      {
      return fftw_plan_dft_c2r_1d(n, in, out, flags);
      }

    template <>
    inline void fftw<float>::execute(const plan p)
      { fftwf_execute(p); }

    template <>
    inline void fftw<double>::execute(const plan p)
      { fftw_execute(p); }

    template <>
    inline void fftw<float>::execute_dft_r2c(
        const plan p,
        float *in,
        complex *out)
      {
      fftwf_execute_dft_r2c(p, in, out);
      }

    template <>
    inline void fftw<double>::execute_dft_r2c(
        const plan p,
        double *in,
        complex *out)
      {
      fftw_execute_dft_r2c(p, in, out);
      }

    template <>
    inline void fftw<float>::destroy_plan(const plan p)
      { fftwf_destroy_plan(p); }

    template <>
    inline void fftw<double>::destroy_plan(const plan p)
      { fftw_destroy_plan(p); }

  }  // namespace bonelab

#endif

