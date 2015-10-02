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

#ifndef BONELAB_ProjectionFilterer_vDSP_HPP_INCLUDED
#define BONELAB_ProjectionFilterer_vDSP_HPP_INCLUDED

#include "ProjectionFilterer.hpp"
#include "bonelab/vDSP.hpp"

namespace athabasca_recon
  {

  /** Apply the ramp function to projections using Apple's vDSP library.
    * ( http://developer.apple.com/library/ios/documentation/Accelerate/Reference/vDSPRef/Reference/reference.html#//apple_ref/doc/uid/TP40009464 )
    *
    * This object is thread-safe only if separate instances are created in
    * each thread.
    */
  template <typename TProjection>
  class ProjectionFilterer_vDSP : public ProjectionFilterer<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef bonelab::Array<1,TValue,TIndex> TFunction1D;

      ProjectionFilterer_vDSP();
      ~ProjectionFilterer_vDSP();

      virtual void Initialize();

      // Automatically called in destructor.
      virtual void Destroy();

      virtual void FilterProjection(const TProjection& in, TProjection& out);

    protected:

      unsigned int m_PowerOfTwo;
      unsigned int m_FFTLength;
      typedef typename bonelab::vDSP<TValue>::TComplex TComplex;
      typedef typename bonelab::vDSP<TValue>::TSplitComplex TSplitComplex;
      typedef typename bonelab::vDSP<TValue>::TFFTSetup TFFTSetup;
      TFFTSetup m_FFTsetup;
      TSplitComplex* m_Scratch;
      TSplitComplex* m_RampFilter;

    };  // class ProjectionFilterer_vDSP

  } // namespace athabasca_recon

#endif
