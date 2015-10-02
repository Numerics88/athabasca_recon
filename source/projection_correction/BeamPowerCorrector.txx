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
#include <cmath>

namespace athabasca_recon
  {

  template <typename TProjection>
  void BeamPowerCorrector<TProjection>::ShiftProjection
    (
    TProjection& proj,
    typename TProjection::value_type shift)
    const
    {
    for (size_t i=0; i<proj.size(); ++i)
      { proj[i] += shift; }
    }

  } // namespace athabasca_recon