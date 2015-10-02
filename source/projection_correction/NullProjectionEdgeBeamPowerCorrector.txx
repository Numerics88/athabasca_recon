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
  NullProjectionEdgeBeamPowerCorrector<TProjection>::NullProjectionEdgeBeamPowerCorrector()
    :
    m_EdgeWidth(10)
    {}

  template <typename TProjection>
  double NullProjectionEdgeBeamPowerCorrector<TProjection>::ProcessProjection(int index, TProjection& proj) const
    {
    TValue correction = IntegrateEdgeStrips(proj);
    this->ShiftProjection(proj, -correction);
    return correction;
    }

  template <typename TProjection>
  double NullProjectionEdgeBeamPowerCorrector<TProjection>::IntegrateEdgeStrips
    (const TProjection& proj) const
    {
    double sum = 0;
    TIndex M = proj.dims()[0];
    TIndex N = proj.dims()[1];
    for (TIndex i=0; i<M; ++i)
      {
      for (TIndex j=0; j<this->m_EdgeWidth; ++j)
        { sum += proj(i,j); }
      for (TIndex j=N-this->m_EdgeWidth; j<N; ++j)
        { sum += proj(i,j); }
      }
    sum /= 2 * this->m_EdgeWidth * M;
    return sum;
    }

  } // namespace athabasca_recon
