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

#ifndef BONELAB_ProjectionFilterer_fftw_hpp_INCLUDED
#define BONELAB_ProjectionFilterer_fftw_hpp_INCLUDED

#include "ProjectionFilterer.hpp"
#include "bonelab/fftw_mt.hpp"

namespace athabasca_recon
  {

  /** Apply the ramp function to projections using the FFTW library.
    * ( http://fftw.org/ )
    *
    * The FFTW functions for creating plans are not thread-safe.  This
    * object uses the bonelab/fftw_mt interface which serializes access to
    * the FFTW functions as required.  (Note that actually performing the FFTs
    * is thread-safe, and will not be serialized.)
    */
  template <typename TProjection>
  class ProjectionFilterer_fftw : public ProjectionFilterer<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef bonelab::Array<1,TValue,TIndex> TFunction1D;

      ProjectionFilterer_fftw();
      ~ProjectionFilterer_fftw();

      virtual void Initialize();
      
      // Automatically called in destructor.
      virtual void Destroy();

      virtual void FilterProjection(const TProjection& in, TProjection& out);

    protected:

      unsigned int m_FFTLength;
      typename bonelab::fftw_mt<TValue>::plan m_ForwardPlan;
      typename bonelab::fftw_mt<TValue>::plan m_BackwardPlan;
      TFunction1D m_Buffer;
      TFunction1D m_RampFilter;

    };  // class ProjectionFilterer_fftw

  } // namespace athabasca_recon

#endif

