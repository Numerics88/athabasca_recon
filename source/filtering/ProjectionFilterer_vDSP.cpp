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

#include "ProjectionFilterer_vDSP.hpp"
#include "RampFilterRealSpace.hpp"
#include "AthabascaException.hpp"
#include "bonelab/FFT_util.hpp"
#include "bonelab/exception.hpp"
#include <boost/scoped_ptr.hpp>


namespace athabasca_recon
  {

  template <typename TProjection>
  ProjectionFilterer_vDSP<TProjection>::ProjectionFilterer_vDSP()
    :
    m_PowerOfTwo(0),
    m_FFTLength(1),
    m_FFTsetup(NULL),
    m_Scratch(NULL),
    m_RampFilter(NULL)
    {}

  template <typename TProjection>
  ProjectionFilterer_vDSP<TProjection>::~ProjectionFilterer_vDSP()
    {this->Destroy();}

  template <typename TProjection>
  void ProjectionFilterer_vDSP<TProjection>::Initialize()
    {
    athabasca_assert(!this->m_FFTsetup);
    athabasca_assert(!this->m_Scratch);
    athabasca_assert(!this->m_RampFilter);
    athabasca_assert(this->m_Weight > 0);

    // Add one to next power of two in order to pad.
    this->m_PowerOfTwo = bonelab::FFT_util::NextPowerOfTwo(this->m_Dimensions[1]) + 1;

// #define EXTRA_FFT_PADDING
#ifdef EXTRA_FFT_PADDING
    std::cout << "**** WARNING: Adding extra FFT padding!\n";
    this->m_PowerOfTwo++;
#endif

    this->m_FFTLength = 1 << this->m_PowerOfTwo;

    this->m_FFTsetup = bonelab::vDSP<TValue>::create_fftsetup(this->m_PowerOfTwo, 0);
    this->m_Scratch = new TSplitComplex;
    this->m_Scratch->realp = (TValue*)malloc(this->m_FFTLength * sizeof(TValue));
    this->m_Scratch->imagp = this->m_Scratch->realp + this->m_FFTLength/2;
    this->m_RampFilter = new TSplitComplex;
    this->m_RampFilter->realp = (TValue*)malloc(this->m_FFTLength * sizeof(TValue));
    this->m_RampFilter->imagp = this->m_RampFilter->realp + this->m_FFTLength/2;

    // Create Ramp Filter by transforming real space version
    TFunction1D rampFilterRealSpace(this->m_FFTLength);
    RampFilterRealSpace<TFunction1D> rampFilterGenerator;
    rampFilterGenerator.SetLength(this->m_FFTLength);
    rampFilterGenerator.SetSpacing(this->m_PixelSpacing);
    // Calculate the factor.  This consists of:
    // 1.  In the vDSP library, the inverse FFT should be
    //     multiplied by the 1/(2*FFTLength).  Actually, experimentation
    //     seems to indicate that 1/(4*FFTLength) is required.  For the
    //     moment I am baffled by this, although I guess has something do
    //     to with multiplying in transformed space.
    double factor = this->m_Weight/(4*this->m_FFTLength);
    rampFilterGenerator.SetWeight(factor);
    rampFilterGenerator.ConstructRealSpaceFilter(rampFilterRealSpace);

    // performance check
    athabasca_assert(size_t(rampFilterRealSpace.ptr()) % 16 == 0);
    athabasca_assert(size_t(m_RampFilter->realp) % 16 == 0);
    bonelab::vDSP<TValue>::ctoz(
        reinterpret_cast<const TComplex*>(rampFilterRealSpace.ptr()),
        2,
        this->m_RampFilter,
        1,
        this->m_FFTLength/2);    
    /* to the frequency domain */
    bonelab::vDSP<TValue>::fft_zrip(
        this->m_FFTsetup,
        this->m_RampFilter,
        1,
        this->m_PowerOfTwo,
        FFT_FORWARD);

    if (this->m_SmoothingFilter)
      {
      // Note: We will construct transferFunction out to this->m_FFTLength,
      //       which is in fact more than we really need (for the storage
      //       scheme used for symmetric real data).  However, as the
      //       length also indicates the Nyquist frequency, this is necessary
      //       for correctness.
      TFunction1D transferFunction(this->m_FFTLength);
      this->m_SmoothingFilter->SetLength(this->m_FFTLength);
      this->m_SmoothingFilter->ConstructTransferFunction(transferFunction);
      // Multiply RampFilter by transferFunction, taking special care to deal with the
      // Nyquist component stored in imagp[0] in this storage scheme.
      TValue Nyquist_component = this->m_RampFilter->imagp[0] * transferFunction[this->m_FFTLength/2];
      bonelab::vDSP<TValue>::vmul(
          this->m_RampFilter->realp,
          1,
          transferFunction.ptr(),
          1,
          this->m_RampFilter->realp,
          1,
          this->m_FFTLength/2);
      this->m_RampFilter->imagp[0] = Nyquist_component;
      // Just assume that m_RampFilter has no imaginary part - actually even
      // if it does, it is ignored below.
      }

    }

  template <typename TProjection>
  void ProjectionFilterer_vDSP<TProjection>::Destroy()
    {
    if (m_FFTsetup)
      {
      bonelab::vDSP<TValue>::destroy_fftsetup(this->m_FFTsetup);
      m_FFTsetup = NULL;
      }
    if (m_Scratch)
      {
      free(m_Scratch->realp);
      delete m_Scratch;
      m_Scratch = NULL;
      }
    if (m_RampFilter)
      {
      free(m_RampFilter->realp);
      delete m_RampFilter;
      m_RampFilter = NULL;
      }
    }

  template <typename TProjection>
  void ProjectionFilterer_vDSP<TProjection>::FilterProjection
    (
    const TProjection& in,
    TProjection& out
    )
    {
    athabasca_assert(this->m_FFTsetup);
    athabasca_assert(this->m_Scratch);
    athabasca_assert(this->m_RampFilter);
    athabasca_assert(in.dims()[0] == out.dims()[0]);
    athabasca_assert(in.dims()[1] >= out.dims()[1]);  // Current design actually uses the same sizes.
    athabasca_assert(in.dims()[1] <= this->m_FFTLength);
    athabasca_assert(this->m_FFTLength == 1 << this->m_PowerOfTwo);
    athabasca_assert(in.spacing()[1] == this->m_PixelSpacing);

    // loop over rows
    for (TIndex i=0; i<in.dims()[0]; i++)
      {
      // in array needs to be copied to split complex format.
      // At this point we are also padding out to m_FFTLength to avoid
      // wrap around.
      bonelab::vDSP<TValue>::ctoz(
          reinterpret_cast<const TComplex*>(&(in(i,0))),
          2,
          this->m_Scratch,
          1,
          in.dims()[1]/2);
      // Fill the rest of the split complex array with zeros
      size_t remainingByteCount = sizeof(TValue)*(this->m_FFTLength - in.dims()[1])/2;
      memset(this->m_Scratch->realp + in.dims()[1]/2, 0, remainingByteCount);
      memset(this->m_Scratch->imagp + in.dims()[1]/2, 0, remainingByteCount);

      /* to the frequency domain Batman */
      bonelab::vDSP<TValue>::fft_zrip(
          this->m_FFTsetup,
          this->m_Scratch,
          1,
          this->m_PowerOfTwo,
          FFT_FORWARD);

      // Have to deal specially with the Nyquist component, which after
      // real-valued FFT is stored in the imaginary part of the 0th component,
      // but doesn't really belong there.
      TValue Nyquist_component = this->m_Scratch->imagp[0] * this->m_RampFilter->imagp[0];

      // Convolve <-> multiplication in k-space
      // Note that the ramp filter is real-valued; we make use of that.
      bonelab::vDSP<TValue>::vmul(
          this->m_Scratch->realp,
          1,
          this->m_RampFilter->realp,
          1,
          this->m_Scratch->realp,
          1,
          this->m_FFTLength/2);
      bonelab::vDSP<TValue>::vmul(
          this->m_Scratch->imagp,
          1,
          this->m_RampFilter->realp,
          1,
          this->m_Scratch->imagp,
          1,
          this->m_FFTLength/2);

      // Restore the Nyquist component to the imaginary part of the 0th component
      // in preparation for inverse FFT.
      this->m_Scratch->imagp[0] = Nyquist_component;

      /* and back to the real world again */
      bonelab::vDSP<TValue>::fft_zrip(
          this->m_FFTsetup,
          this->m_Scratch,
          1,
          this->m_PowerOfTwo,
          FFT_INVERSE);

      // From the split complex form, restore normal ordering.
      // Also, only copy what is necessary for out (typically half or less,
      // since not padded).
      bonelab::vDSP<TValue>::ztoc(
          this->m_Scratch,
          1,
          reinterpret_cast<TComplex*>(&(out(i,0))),
          2,
          out.dims()[1]/2);

      }

    }

  // explicit instantiations
  template class ProjectionFilterer_vDSP<bonelab::Image<2,float,int,float> >;
  template class ProjectionFilterer_vDSP<bonelab::Image<2,double,int,double> >;

  }  // namespace athabasca_recon
