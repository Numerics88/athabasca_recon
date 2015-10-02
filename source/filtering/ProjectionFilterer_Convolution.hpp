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

#ifndef BONELAB_ProjectionFilterer_Convolution_HPP_INCLUDED
#define BONELAB_ProjectionFilterer_Convolution_HPP_INCLUDED

#include "ProjectionFilterer.hpp"

namespace athabasca_recon
  {

  /** Apply the ramp function to projections using real-space convolution.
    *
    * This class directly convolves the input projection
    * rows with the ramp function kernel.  This is a whole order slower than
    * performing this operation in k-space with FFTs, so this implementation
    * will typically only be used by developers using small data sets for
    * experimentation and verification.
    *
    * Additionally, no smoothing filter can be specified with this implementation.
    * Attempting to doing so will cause an exception to be thrown.
    */
  template <typename TProjection>
  class ProjectionFilterer_Convolution : public ProjectionFilterer<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef bonelab::Array<1,TValue,TIndex> TFunction1D;

      /** Initializes.
        *
        * This function must be called before the FilterProjection method
        * can be used.
        */
      virtual void Initialize();

      /** Apply the ramp filter to the input projection.
        */
      virtual void FilterProjection(const TProjection& in, TProjection& out);

    protected:

      TFunction1D m_RampFilterRealSpace;

    };  // class ProjectionFilterer_Convolution

  } // namespace athabasca_recon

#endif
