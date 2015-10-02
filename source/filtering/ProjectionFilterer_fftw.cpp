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

#include "ProjectionFilterer_fftw.hpp"
#include "RampFilterRealSpace.hpp"
#include "AthabascaException.hpp"
#include "bonelab/FFT_util.hpp"
#include "bonelab/exception.hpp"
#include <boost/scoped_ptr.hpp>

using bonelab::fftw_mt;

namespace athabasca_recon
  {

  template <typename TProjection>
  ProjectionFilterer_fftw<TProjection>::ProjectionFilterer_fftw()
    :
    m_ForwardPlan(NULL),
    m_BackwardPlan(NULL),
    m_FFTLength(0)
    {}

  template <typename TProjection>
  ProjectionFilterer_fftw<TProjection>::~ProjectionFilterer_fftw()
    {this->Destroy();}

  template <typename TProjection>
  void ProjectionFilterer_fftw<TProjection>::Initialize()
    {
    athabasca_assert(!this->m_ForwardPlan);
    athabasca_assert(!this->m_BackwardPlan);

    // Add one to next power of two in order to pad.
    this->m_FFTLength = bonelab::FFT_util::NextNumberWithFactors2And3(2*this->m_Dimensions[1]);
    // Buffer length is slightly longer due to FFTW storage scheme.
    unsigned int bufferLength = 2*(this->m_FFTLength/2 + 1);
    this->m_Buffer.construct(bufferLength);

    m_ForwardPlan = fftw_mt<TValue>::plan_dft_r2c_1d(
                 this->m_FFTLength,
                 this->m_Buffer.ptr(),
                 reinterpret_cast<typename fftw_mt<TValue>::complex*>(this->m_Buffer.ptr()),
                 FFTW_MEASURE);
    m_BackwardPlan = fftw_mt<TValue>::plan_dft_c2r_1d(
                 this->m_FFTLength,
                 reinterpret_cast<typename fftw_mt<TValue>::complex*>(this->m_Buffer.ptr()),
                 this->m_Buffer.ptr(),
                 FFTW_MEASURE);

    this->m_RampFilter.construct(bufferLength);
    // Create another array that is just a reference to the same data,
    // but with length just FFTLength, instead of length Buffer.
    TFunction1D rampFilterRef;
    rampFilterRef.construct_reference(this->m_RampFilter.ptr(), this->m_FFTLength);

    // Create Ramp Filter by transforming real space version
    RampFilterRealSpace<TFunction1D> rampFilterGenerator;
    rampFilterGenerator.SetLength(this->m_FFTLength);
    rampFilterGenerator.SetSpacing(this->m_PixelSpacing);
    // Calculate the factor.  This consists of:
    // 1.  In the FFTW library, the inverse FFT should be
    //     multiplied by the 1/FFTLength.
    double factor = this->m_Weight/(this->m_FFTLength);
    rampFilterGenerator.SetWeight(factor);
    rampFilterGenerator.ConstructRealSpaceFilter(rampFilterRef);

    // performance check
    athabasca_assert(size_t(m_RampFilter.ptr()) % 16 == 0);

    /* to the frequency domain */
    typename fftw_mt<TValue>::plan rampForwardPlan = fftw_mt<TValue>::plan_dft_r2c_1d(
                   this->m_FFTLength,
                   this->m_RampFilter.ptr(),
                   reinterpret_cast<typename fftw_mt<TValue>::complex*>(this->m_RampFilter.ptr()),
                   FFTW_ESTIMATE);
    fftw_mt<TValue>::execute(rampForwardPlan);
    fftw_mt<TValue>::destroy_plan(rampForwardPlan);

    if (this->m_SmoothingFilter)
      {
      // Note: We will construct transferFunction out to this->m_FFTLength,
      //       which is in fact more than we really need.  However, as the
      //       length also indicates the Nyquist frequency, this is necessary
      //       for correctness.
      TFunction1D transferFunction(this->m_FFTLength);
      this->m_SmoothingFilter->SetLength(this->m_FFTLength);
      this->m_SmoothingFilter->ConstructTransferFunction(transferFunction);
      // Multiply RampFilter by transferFunction 
      for (unsigned int i=0, j=0; j<bufferLength; ++i, j+=2)
        {
        // Note that these are only the real components.
        // Just assume that m_RampFilter has no imaginary part - actually even
        // if it does, it is ignored below.
        this->m_RampFilter[j] *= transferFunction[i];
        }
      }

    }

  template <typename TProjection>
  void ProjectionFilterer_fftw<TProjection>::Destroy()
    {
    if (this->m_ForwardPlan)
      {
      fftw_mt<TValue>::destroy_plan(this->m_ForwardPlan);
      this->m_ForwardPlan = NULL;
      }
    if (this->m_BackwardPlan)
      {
      fftw_mt<TValue>::destroy_plan(this->m_BackwardPlan);
      this->m_BackwardPlan = NULL;
      }
    this->m_Buffer.destruct();
    this->m_RampFilter.destruct();
    }

  template <typename TProjection>
  void ProjectionFilterer_fftw<TProjection>::FilterProjection
    (
    const TProjection& in,
    TProjection& out
    )
    {
    athabasca_assert(this->m_ForwardPlan);
    athabasca_assert(this->m_BackwardPlan);
    athabasca_assert(this->m_Buffer.is_constructed());
    athabasca_assert(this->m_RampFilter.is_constructed());
    athabasca_assert(this->m_Buffer.size() == this->m_RampFilter.size());
    athabasca_assert(in.dims()[0] == out.dims()[0]);
    athabasca_assert(in.dims()[1] >= out.dims()[1]);  // Current design actually uses the same sizes.
    athabasca_assert(in.dims()[1] <= this->m_FFTLength);
    athabasca_assert(in.spacing()[1] == this->m_PixelSpacing);

    // loop over rows
    for (TIndex i=0; i<in.dims()[0]; i++)
      {
      // Copy row data to buffer
      memcpy(this->m_Buffer.ptr(), &(in(i,0)), sizeof(TValue)*in.dims()[1]);
      // Fill the rest of the buffer with zeros
      memset(&(this->m_Buffer[in.dims()[1]]), 0, sizeof(TValue)*(this->m_Buffer.size()-in.dims()[1]));

      /* to the frequency domain Batman */
      fftw_mt<TValue>::execute(this->m_ForwardPlan);

      // Convolve <-> multiplication in k-space
      // Note that the ramp filter is real-valued; we make use of that.
        {  // scope
        TValue* b = this->m_Buffer.ptr();
        const TValue* b_end = this->m_Buffer.end();
        TValue* t = this->m_RampFilter.ptr();
        while (b != b_end)
          {
          *b *= *t;   // Real part
          ++b;
          *b *= *t;   // Imaginary part
          ++b;
          t += 2;  // next real part of ramp filter
          }
        }

      /* and back to the real world again */
      fftw_mt<TValue>::execute(this->m_BackwardPlan);

      // Copy what is necessary for out (typically half or less,
      // since not padded).
      memcpy(&(out(i,0)), this->m_Buffer.ptr(), sizeof(TValue)*out.dims()[1]);
      }

    }

  // explicit instantiations
  template class ProjectionFilterer_fftw<bonelab::Image<2,float,int,float> >;
  template class ProjectionFilterer_fftw<bonelab::Image<2,double,int,double> >;

  }  // namespace athabasca_recon

