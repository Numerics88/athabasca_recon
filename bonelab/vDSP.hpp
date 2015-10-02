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

#ifndef BONELAB_vDSP_HPP_INCLUDED
#define BONELAB_vDSP_HPP_INCLUDED

#include <Accelerate/Accelerate.h>

namespace bonelab
  {

  template<typename T> class vDSPTypes;

  template <> class vDSPTypes<float>
    {
    public:
    typedef DSPComplex TComplex;
    typedef DSPSplitComplex TSplitComplex;
    typedef FFTSetup TFFTSetup;
    };

  template <> class vDSPTypes<double>
    {
    public:
    typedef DSPDoubleComplex TComplex;
    typedef DSPDoubleSplitComplex TSplitComplex;
    typedef FFTSetupD TFFTSetup;
    };

  /** A templated interface to the FFT functions in the vDSP library.
    *
    * Contains static member functions that map to functions in vDSP.
    * The only purpose to this class is to make it easier to write templated
    * code using vDSP, which uses different function names for float and
    * double versions of functions.
    *
    * The functions in this class are named identically to vDSP, except 
    * without the vDSP prefix and without the D suffix for double precision.
    *
    * Refer to the vDSP documentation for documentation of particular functions.
    *
    * Obviously not complete.  Add functions as needed.
    *
    * vDSP appears, by experiment, to be thread-safe, so long as each thread
    * creates its own setups.
    */
  template<typename T>
  class vDSP
    {
    public:

      typedef typename vDSPTypes<T>::TComplex TComplex;
      typedef typename vDSPTypes<T>::TSplitComplex TSplitComplex;
      typedef typename vDSPTypes<T>::TFFTSetup TFFTSetup;

      static inline TFFTSetup create_fftsetup(
          vDSP_Length __vDSP_log2n,
          FFTRadix __vDSP_radix);
      static inline void ctoz(
          const TComplex __vDSP_C[],
          vDSP_Stride __vDSP_strideC,
          TSplitComplex *__vDSP_Z,
          vDSP_Stride __vDSP_strideZ,
          vDSP_Length __vDSP_size);
      static inline void destroy_fftsetup(TFFTSetup __vDSP_setup);
      static inline void fft_zrip(
          TFFTSetup __vDSP_setup,
          TSplitComplex *__vDSP_ioData,
          vDSP_Stride __vDSP_stride,
          vDSP_Length __vDSP_log2n,
          FFTDirection __vDSP_direction);
      static inline void vmul(
          const T __vDSP_input1[],
          vDSP_Stride __vDSP_stride1,
          const T __vDSP_input2[],
          vDSP_Stride __vDSP_stride2,
          T __vDSP_result[],
          vDSP_Stride __vDSP_strideResult,
          vDSP_Length __vDSP_size);
      static inline void vsadd(
          const T *__vDSP_A,
          vDSP_Stride __vDSP_I,
          const T *__vDSP_B,
          T *__vDSP_C,
          vDSP_Stride __vDSP_K,
          vDSP_Length __vDSP_N);    
      static inline void vsmul(
          const T __vDSP_input1[],
          vDSP_Stride __vDSP_stride1,
          const T *__vDSP_input2,
          T __vDSP_result[],
          vDSP_Stride __vDSP_strideResult,
          vDSP_Length __vDSP_size);
      static inline void ztoc(
          const TSplitComplex *__vDSP_Z,
          vDSP_Stride __vDSP_strideZ,
          TComplex __vDSP_C[],
          vDSP_Stride __vDSP_strideC,
          vDSP_Length __vDSP_size);
    
    };  // class vDSP<T>

  template <>
  inline void vDSP<float>::ctoz
    (
    const TComplex __vDSP_C[],
    vDSP_Stride __vDSP_strideC,
    TSplitComplex *__vDSP_Z,
    vDSP_Stride __vDSP_strideZ,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_ctoz(__vDSP_C,
              __vDSP_strideC,
              __vDSP_Z,
              __vDSP_strideZ,
              __vDSP_size);
    }

  template <>
  inline void vDSP<double>::ctoz
    (
    const TComplex __vDSP_C[],
    vDSP_Stride __vDSP_strideC,
    TSplitComplex *__vDSP_Z,
    vDSP_Stride __vDSP_strideZ,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_ctozD(__vDSP_C,
               __vDSP_strideC,
               __vDSP_Z,
               __vDSP_strideZ,
               __vDSP_size);
    }

  template <>
  inline vDSP<float>::TFFTSetup vDSP<float>::create_fftsetup
    (
    vDSP_Length __vDSP_log2n,
    FFTRadix __vDSP_radix
    )
    { return vDSP_create_fftsetup(__vDSP_log2n, __vDSP_radix); }

  template <>
  inline vDSP<double>::TFFTSetup vDSP<double>::create_fftsetup
    (
    vDSP_Length __vDSP_log2n,
    FFTRadix __vDSP_radix
    )
    { return vDSP_create_fftsetupD(__vDSP_log2n, __vDSP_radix); }

  template <>
  inline void vDSP<float>::destroy_fftsetup
    (TFFTSetup __vDSP_setup)
    { vDSP_destroy_fftsetup(__vDSP_setup); }

  template <>
  inline void vDSP<double>::destroy_fftsetup
    (TFFTSetup __vDSP_setup)
    { vDSP_destroy_fftsetupD(__vDSP_setup); }

  template <>
  inline void vDSP<float>::fft_zrip
    (
    TFFTSetup __vDSP_setup,
    TSplitComplex *__vDSP_ioData,
    vDSP_Stride __vDSP_stride,
    vDSP_Length __vDSP_log2n,
    FFTDirection __vDSP_direction
    )
    {
    vDSP_fft_zrip(__vDSP_setup,
                  __vDSP_ioData,
                  __vDSP_stride,
                  __vDSP_log2n,
                  __vDSP_direction);
    }

  template <>
  inline void vDSP<double>::fft_zrip
    (
    TFFTSetup __vDSP_setup,
    TSplitComplex *__vDSP_ioData,
    vDSP_Stride __vDSP_stride,
    vDSP_Length __vDSP_log2n,
    FFTDirection __vDSP_direction
    )
    {
    vDSP_fft_zripD(__vDSP_setup,
                   __vDSP_ioData,
                   __vDSP_stride,
                   __vDSP_log2n,
                   __vDSP_direction);
    }

  template <>
  inline void vDSP<float>::vmul(
    const float __vDSP_input1[],
    vDSP_Stride __vDSP_stride1,
    const float __vDSP_input2[],
    vDSP_Stride __vDSP_stride2,
    float __vDSP_result[],
    vDSP_Stride __vDSP_strideResult,
    vDSP_Length __vDSP_size)
    {
    vDSP_vmul(
    __vDSP_input1,
    __vDSP_stride1,
    __vDSP_input2,
    __vDSP_stride2,
    __vDSP_result,
    __vDSP_strideResult,
    __vDSP_size);
    }

  template <>
  inline void vDSP<double>::vmul(
    const double __vDSP_input1[],
    vDSP_Stride __vDSP_stride1,
    const double __vDSP_input2[],
    vDSP_Stride __vDSP_stride2,
    double __vDSP_result[],
    vDSP_Stride __vDSP_strideResult,
    vDSP_Length __vDSP_size)
    {
    vDSP_vmulD(
    __vDSP_input1,
    __vDSP_stride1,
    __vDSP_input2,
    __vDSP_stride2,
    __vDSP_result,
    __vDSP_strideResult,
    __vDSP_size);
    }

  template<>
  inline void vDSP<float>::vsadd
    (
    const float *__vDSP_A,
    vDSP_Stride __vDSP_I,
    const float *__vDSP_B,
    float *__vDSP_C,
    vDSP_Stride __vDSP_K,
    vDSP_Length __vDSP_N
    )
    {
    // Why is vDSP_vsadd not const correct when vDSP_vsmul is????
    vDSP_vsadd(
        const_cast<float *>(__vDSP_A),
        __vDSP_I,
        const_cast<float *>(__vDSP_B),
        __vDSP_C,
        __vDSP_K,
        __vDSP_N);
    }

  template<>
  inline void vDSP<double>::vsadd
    (
    const double *__vDSP_A,
    vDSP_Stride __vDSP_I,
    const double *__vDSP_B,
    double *__vDSP_C,
    vDSP_Stride __vDSP_K,
    vDSP_Length __vDSP_N
    )
    {
    // Why is vDSP_vsadd not const correct when vDSP_vsmul is????
    vDSP_vsaddD(
        const_cast<double *>(__vDSP_A),
        __vDSP_I,
        const_cast<double *>(__vDSP_B),
        __vDSP_C,
        __vDSP_K,
        __vDSP_N);
    }

  template <>
  inline void vDSP<float>::vsmul
    (
    const float __vDSP_input1[],
    vDSP_Stride __vDSP_stride1,
    const float *__vDSP_input2,
    float __vDSP_result[],
    vDSP_Stride __vDSP_strideResult,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_vsmul(__vDSP_input1,
               __vDSP_stride1,
               __vDSP_input2,
               __vDSP_result,
               __vDSP_strideResult,
               __vDSP_size);
    }

  template <>
  inline void vDSP<double>::vsmul
    (
    const double __vDSP_input1[],
    vDSP_Stride __vDSP_stride1,
    const double *__vDSP_input2,
    double __vDSP_result[],
    vDSP_Stride __vDSP_strideResult,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_vsmulD(__vDSP_input1,
                __vDSP_stride1,
                __vDSP_input2,
                __vDSP_result,
                __vDSP_strideResult,
                __vDSP_size);
    }

  template <>
  inline void vDSP<float>::ztoc
    (
    const TSplitComplex *__vDSP_Z,
    vDSP_Stride __vDSP_strideZ,
    TComplex __vDSP_C[],
    vDSP_Stride __vDSP_strideC,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_ztoc(__vDSP_Z,
              __vDSP_strideZ,
              __vDSP_C,
              __vDSP_strideC,
              __vDSP_size);
    }

  template <>
  inline void vDSP<double>::ztoc
    (
    const TSplitComplex *__vDSP_Z,
    vDSP_Stride __vDSP_strideZ,
    TComplex __vDSP_C[],
    vDSP_Stride __vDSP_strideC,
    vDSP_Length __vDSP_size
    )
    {
    vDSP_ztocD(__vDSP_Z,
               __vDSP_strideZ,
               __vDSP_C,
               __vDSP_strideC,
               __vDSP_size);
    }

  }  // namespace bonelab

#endif
