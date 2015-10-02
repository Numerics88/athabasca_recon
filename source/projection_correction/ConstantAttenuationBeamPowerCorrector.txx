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

#include "AthabascaException.hpp"
#include "bonelab/Image.hpp"
#include <iostream>

namespace athabasca_recon
  {

  template <typename TProjection>
  ConstantAttenuationBeamPowerCorrector<TProjection>::ConstantAttenuationBeamPowerCorrector()
    :
    m_ReferenceSet(false),
    m_ReferenceAttenuation(0),
    m_BeamDecayConstantTerm(0)
    {}

  template <typename TProjection>
  void ConstantAttenuationBeamPowerCorrector<TProjection>::SetReferenceProjection(const TProjection& proj)
    {
    this->m_ReferenceAttenuation = this->GetIntegratedAttenuation(proj);
    this->m_ReferenceSet = true;
    }

  template <typename TProjection>
  double ConstantAttenuationBeamPowerCorrector<TProjection>::ProcessProjection(int index, TProjection& proj) const
    {
    TValue correction = -CalculateRequiredShift(proj) + this->m_BeamDecayConstantTerm;
    this->ShiftProjection(proj, -correction);
    return correction;
    }

  template <typename TProjection>
  double ConstantAttenuationBeamPowerCorrector<TProjection>::GetIntegratedAttenuation
    (const TProjection& proj) const
    {
    double sum = 0;
    for (size_t i=0; i<proj.size(); i++)
      { sum += proj[i]; }
    return sum*product(proj.spacing());
    }

  template <typename TProjection>
  typename ConstantAttenuationBeamPowerCorrector<TProjection>::TValue ConstantAttenuationBeamPowerCorrector<TProjection>::CalculateRequiredShift
    (const TProjection& proj) const
    {
    athabasca_assert(this->m_ReferenceSet);
    athabasca_assert(this->m_ReferenceAttenuation > 0);
    return (this->m_ReferenceAttenuation - this->GetIntegratedAttenuation(proj))
            / (proj.size()*product(proj.spacing()));
    }

  } // namespace athabasca_recon

