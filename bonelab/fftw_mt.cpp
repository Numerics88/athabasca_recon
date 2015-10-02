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

#include "fftw_mt.hpp"
#include "fftw_templated.hpp"
#ifdef TRACE_THREADING
#include <iostream>
#endif

namespace bonelab
  {

  // static member - create the global instance. (Actually one for each version of template).
  template <typename T> boost::mutex fftw_mt<T>::m_Mutex;

  template<typename T>
  typename fftw_mt<T>::plan fftw_mt<T>::plan_dft_r2c_1d
    (
    int n,
    T *in,
    typename fftw_mt<T>::complex *out,
    unsigned flags
    )
    {
    boost::lock_guard<boost::mutex> lock(m_Mutex);
#ifdef TRACE_THREADING
    std::cout << "Entering fftw_mt::plan_dft_r2c_1d\n";
#endif
    plan p = fftw<T>::plan_dft_r2c_1d(n, in, out, flags);
#ifdef TRACE_THREADING
    std::cout << "Exiting fftw_mt::plan_dft_r2c_1d\n";
#endif
    return p;
    }

  template<typename T>
  typename fftw_mt<T>::plan fftw_mt<T>::plan_dft_c2r_1d
    (
    int n,
    typename fftw_mt<T>::complex *in,
    T *out,
    unsigned flags
    )
    {
    boost::lock_guard<boost::mutex> lock(m_Mutex);
#ifdef TRACE_THREADING
    std::cout << "Entering fftw_mt::plan_dft_c2r_1d\n";
#endif
    plan p = fftw<T>::plan_dft_c2r_1d(n, in, out, flags);
#ifdef TRACE_THREADING
    std::cout << "Exiting fftw_mt::plan_dft_c2r_1d\n";
#endif
    return p;
    }

  template<typename T>
  void fftw_mt<T>::execute(const typename fftw_mt<T>::plan p)
    {
    // No lock needed for execute.
#ifdef TRACE_THREADING
    std::cout << "Entering fftw_mt::execute\n";
#endif
    fftw<T>::execute(p);
#ifdef TRACE_THREADING
    std::cout << "Exiting fftw_mt::execute\n";
#endif
    }

  template<typename T>
  void fftw_mt<T>::execute_dft_r2c
    (
    const plan p,
    T *in,
    complex *out
    )
    {
    // No lock needed for execute.
#ifdef TRACE_THREADING
    std::cout << "Entering fftw_mt::execute_dft_r2c\n";
#endif
    fftw<T>::execute_dft_r2c(p, in, out);
#ifdef TRACE_THREADING
    std::cout << "Exiting fftw_mt::execute_dft_r2c\n";
#endif
    }

  template<typename T>
  void fftw_mt<T>::destroy_plan(const plan p)
    {
    boost::lock_guard<boost::mutex> lock(m_Mutex);
#ifdef TRACE_THREADING
    std::cout << "Entering fftw_mt::destroy_plan\n";
#endif
    fftw<T>::destroy_plan(p);
#ifdef TRACE_THREADING
    std::cout << "Exiting fftw_mt::destroy_plan\n";
#endif
    }

  // explicit instantiations
  template class fftw_mt<float>;
  template class fftw_mt<double>;

  }  // namespace bonelab
