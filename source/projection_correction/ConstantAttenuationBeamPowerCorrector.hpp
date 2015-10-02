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

#ifndef BONELAB_ConstantAttenuationBeamPowerCorrector_HPP_INCLUDED
#define BONELAB_ConstantAttenuationBeamPowerCorrector_HPP_INCLUDED

#include "BeamPowerCorrector.hpp"

namespace athabasca_recon
  {

  /** A class for performing beam power correction based on constant
    * total integrated attenuation.
    *
    * This method requires that a reference projection first be set;
    * usually the first one.  All other attenuation projections are then
    * shifted to have the same total integrated attenuation.
    *
    * The method can't be used for the bright field; therefore, for correct
    * background level, BeamDecayConstantTerm must be set.
    *
    * The method assumes the the total integrated attenuation is independent
    * of the angle of the projection.  This is true only in the absence of
    * nonlinearities (beam hardening, extinction, etc...)  See the manual for
    * further discussion.
    */
  template <typename TProjection>
  class ConstantAttenuationBeamPowerCorrector : public BeamPowerCorrector<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;

      /** Default constructor. */
      ConstantAttenuationBeamPowerCorrector();

      /** Sets the beam decay constant term.
        * This sets the offset, or background level, which cannot be automatically
        * determined.  In general if the bright field was done at beam current
        * P0, and the first (reference) projection is done at P1, this should
        * be set to log(P0/P1) (a positive value for decaying beam power).
        *
        * Default is zero.
        */
      void SetBeamDecayConstantTerm(double arg) {this->m_BeamDecayConstantTerm = arg;}
      virtual double GetBeamDecayConstantTerm() const {return this->m_BeamDecayConstantTerm;}

      /** Sets the reference projection (typically the first one).
        * For multithreading, take care that this is in fact set before any
        * worker thread can call ProcessProjection.
        */
      virtual void SetReferenceProjection(const TProjection& proj);

      virtual double ProcessProjection(int index, TProjection& proj) const;

    protected:
      
      virtual double GetIntegratedAttenuation(const TProjection& proj) const;
      TValue CalculateRequiredShift(const TProjection& proj) const;
      TValue GetReferenceAttenuation() const { return m_ReferenceAttenuation; }

      bool m_ReferenceSet;
      TValue m_ReferenceAttenuation;
      double m_BeamDecayConstantTerm;

    };  // class ConstantAttenuationBeamPowerCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ConstantAttenuationBeamPowerCorrector.txx"

#endif
