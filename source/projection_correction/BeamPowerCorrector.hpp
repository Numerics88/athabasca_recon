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

#ifndef BONELAB_BeamPowerCorrector_HPP_INCLUDED
#define BONELAB_BeamPowerCorrector_HPP_INCLUDED

#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** A base class for adjusting attenuation projections to compensate for
    * decaying beam power.
    */
  template <typename TProjection>
  class BeamPowerCorrector : private boost::noncopyable
    {
    public:
      
      virtual ~BeamPowerCorrector() {}

      /** Sets the reference projection.
        * Some methods of beam power correction require that a reference
        * projection be set.
        */
      virtual void SetReferenceProjection(const TProjection& proj) {}

      /** Adjusts an attenuation projection.
        * The attenuation projection will be uniformly shifted by an amount
        * dependent on the specific method used.
        *
        * @param index  The index of the projection (0 being the first).  Some
        *               methods require this value to calculate the shift.
        * @param proj  The input projection.
        * @return  The applied shift.
        */
      virtual double ProcessProjection(int index, TProjection& proj) const = 0;

    protected:
      
      virtual void ShiftProjection(TProjection& proj, typename TProjection::value_type shift) const;

    };  // class BeamPowerCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "BeamPowerCorrector.txx"

#endif
