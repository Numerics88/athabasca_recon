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

#ifndef BONELAB_NullProjectionEdgeBeamPowerCorrector_hpp_INCLUDED
#define BONELAB_NullProjectionEdgeBeamPowerCorrector_hpp_INCLUDED

#include "BeamPowerCorrector.hpp"

namespace athabasca_recon
  {

  template <typename TProjection>
  /** A class for performing beam power correction based nulling the
    * attenuation in some strip along both edges of the projections.
    *
    * This method requires that the input projections are in fact
    * illuminated right to the edge, and the there is some strip at the edges
    * that is never occluded by any part of the object.
    *
    * See the manual for further discussion.
    */
  class NullProjectionEdgeBeamPowerCorrector : public BeamPowerCorrector<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;

      NullProjectionEdgeBeamPowerCorrector();

      /** Sets the width in pixels of the strips to null at the edges of the
        * projection.
        *
        * Default is 10.
        */
      void SetEdgeWidth(TIndex arg) {this->m_EdgeWidth = arg;}

      virtual double ProcessProjection(int index, TProjection& proj) const;

    protected:

      virtual double IntegrateEdgeStrips(const TProjection& proj) const;

      TIndex m_EdgeWidth;
      
    };  // class NullProjectionEdgeBeamPowerCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "NullProjectionEdgeBeamPowerCorrector.txx"

#endif
