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

#ifndef BONELAB_ManualBeamPowerCorrector_HPP_INCLUDED
#define BONELAB_ManualBeamPowerCorrector_HPP_INCLUDED

#include "BeamPowerCorrector.hpp"

namespace athabasca_recon
  {

  /** A class for performing beam power correction based on known coefficients.
    *
    * This method assumes exponential decay of the beam power, which is
    * equivalent to a constant shift of the attenuations with time.
    *
    * The class is intended to be able to use either projection number or
    * actual time as the independent variable.  At present, only
    * support for projection number is implemented.
    *
    * For further discussion of this method please refer to the manual.
    */
  template <typename TProjection>
  class ManualBeamPowerCorrector : public BeamPowerCorrector<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;

      ManualBeamPowerCorrector();

      /** Sets the beam decay constant term.
        * This sets the offset, or background level, of the attenuation projections.
        * In general if the bright field was done at beam current
        * P0, and the first projection is done at P1, this should
        * be set to log(P0/P1) (a positive value for decaying beam power).
        *
        * Default is zero.
        */
      void SetBeamDecayConstantTerm(double arg) {this->m_BeamDecayConstantTerm = arg;}
      virtual double GetBeamDecayConstantTerm() const {return this->m_BeamDecayConstantTerm;}

      /** Sets the beam decay constant term.
        * This sets linear term (as a function of projection number or time)
        * adjustment of the attenuation projections.
        * In general if during the measurement of N projections the beam
        * current changes from P0 to P1, this value should be
        * log(P0/P1)/N (a positive value for decaying beam power).
        *
        * Default is zero.
        */
      void SetBeamDecayLinearTerm(double arg) {this->m_BeamDecayLinearTerm = arg;}
      virtual double GetBeamDecayLinearTerm() const {return this->m_BeamDecayLinearTerm;}

      virtual double ProcessProjection(int index, TProjection& proj) const;

    protected:
      
      double m_BeamDecayConstantTerm;
      double m_BeamDecayLinearTerm;

    };  // class ManualBeamPowerCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ManualBeamPowerCorrector.txx"

#endif
