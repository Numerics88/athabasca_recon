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

namespace athabasca_recon
  {

  template <class TVolume>
  Subvolumer<TVolume>::Subvolumer
    (
    bonelab::Tuple<3,TIndex> dims,
    bonelab::Tuple<3,TSpace> spacing,
    bonelab::Tuple<3,TSpace> origin,
    int subvolumes
    )
    {
    this->m_SubvolumeStartZIndex.resize(subvolumes);
    this->m_SubvolumeOrigins.resize(subvolumes);
    this->m_SubvolumeDims.resize(subvolumes);
    float fractional_count = static_cast<float>(dims[0])/static_cast<float>(subvolumes);
    int i;
    for (i=0; i<subvolumes; ++i)
      { this->m_SubvolumeStartZIndex[i] = i*fractional_count; }
    for (i=0; i<subvolumes-1; ++i)
      {
      this->m_SubvolumeDims[i][0] = this->m_SubvolumeStartZIndex[i+1] - this->m_SubvolumeStartZIndex[i];
      this->m_SubvolumeDims[i][1] = dims[1];
      this->m_SubvolumeDims[i][2] = dims[2];
      this->m_SubvolumeOrigins[i][0] = origin[0] + spacing[0]*this->m_SubvolumeStartZIndex[i];
      this->m_SubvolumeOrigins[i][1] = origin[1];
      this->m_SubvolumeOrigins[i][2] = origin[2];
      }
    // At this point, i = subvolumes-1
    this->m_SubvolumeDims[i][0] = dims[0] - this->m_SubvolumeStartZIndex[i];
    this->m_SubvolumeDims[i][1] = dims[1];
    this->m_SubvolumeDims[i][2] = dims[2];
    this->m_SubvolumeOrigins[i][0] = origin[0] + spacing[0]*this->m_SubvolumeStartZIndex[i];
    this->m_SubvolumeOrigins[i][1] = origin[1];
    this->m_SubvolumeOrigins[i][2] = origin[2];
    // Check
    TIndex dims_z_sum = 0;
    for (i=0; i<subvolumes; ++i)
      { dims_z_sum += this->m_SubvolumeDims[i][0]; }
    athabasca_assert(dims_z_sum == dims[0]);
    }

  } // namespace athabasca_recon

