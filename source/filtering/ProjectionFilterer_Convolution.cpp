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

#include "ProjectionFilterer_Convolution.hpp"
#include "RampFilterRealSpace.hpp"
#include "AthabascaException.hpp"

// #define TEST

namespace athabasca_recon
  {

  template <typename TProjection>
  void ProjectionFilterer_Convolution<TProjection>::Initialize()
    {
    athabasca_assert(!this->m_RampFilterRealSpace.is_constructed());
    // This implementation can't handle a SmoothingFilter.
    if (this->m_SmoothingFilter)
      { throw_athabasca_exception("Convolution Filterer can't accept a Smoothing Filter"); }
    RampFilterRealSpace<TFunction1D> rampFilterGenerator;
    // Need to have Ramp Filter at least twice the length, as is designed
    // as wrap-around function.
    rampFilterGenerator.SetLength(2*this->m_Dimensions[1]);
    rampFilterGenerator.SetSpacing(this->m_PixelSpacing);
    rampFilterGenerator.SetWeight(this->m_Weight);
    rampFilterGenerator.ConstructRealSpaceFilter(this->m_RampFilterRealSpace);
    }

  template <typename TProjection>
  void ProjectionFilterer_Convolution<TProjection>::FilterProjection
    (
    const TProjection& in,
    TProjection& out
    )
    {
    // Currently only written to support length in = length out.
    // Should generalize.
    athabasca_assert(this->m_RampFilterRealSpace.is_constructed());
    athabasca_assert(in.dims()[0] == out.dims()[0]);
    int length = this->m_Dimensions[1];
    athabasca_assert(in.dims()[1] == length);
    athabasca_assert(out.dims()[1] == length);
    athabasca_assert(in.spacing()[1] == this->m_PixelSpacing);

    out.zero();

    // loop over rows
    for (TIndex i=0; i<in.dims()[0]; ++i)
      {
      for (TIndex j=0; j<length; ++j)
        {
        for (TIndex k=0; k<length; ++k)
          {
          // take advantage of the fact that RampFilterRealSpace is symmetric.
          out(i,j) += in(i,k)*this->m_RampFilterRealSpace(abs(k-j));
          }
        }
      }

    }

  // explicit instantiations
  template class ProjectionFilterer_Convolution<bonelab::Image<2,float,int,float> >;
  template class ProjectionFilterer_Convolution<bonelab::Image<2,double,int,double> >;

  }  // namespace athabasca_recon
