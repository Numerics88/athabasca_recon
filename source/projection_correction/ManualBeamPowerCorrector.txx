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
  ManualBeamPowerCorrector<TProjection>::ManualBeamPowerCorrector()
    :
    m_BeamDecayConstantTerm(0),
    m_BeamDecayLinearTerm(0)
    {}

  template <typename TProjection>
  double ManualBeamPowerCorrector<TProjection>::ProcessProjection(int index, TProjection& proj) const
    {
    TValue correction = this->m_BeamDecayConstantTerm + index*(this->m_BeamDecayLinearTerm);
    this->ShiftProjection(proj, -correction);
    return correction;
    }

  } // namespace athabasca_recon

