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

#ifndef BONELAB_Subvolumer_HPP_INCLUDED
#define BONELAB_Subvolumer_HPP_INCLUDED

#include "bonelab/Image.hpp"

namespace athabasca_recon
  {

  /** A class to produce a division a volume into a number of subvolumes,
    * sliced along the z axis.
    *
    * Note that this class only calculates an optimal division; no operation on
    * data is performed.
    *
    * The subvolumes will be of equal size if the z dimension of the
    * volume divides evenly by the number of subvolumes.  In all cases, the
    * subvolumes will be within 1 pixel in size (z direction) of each other.
    *
    * Note that there is no method to return the voxel spacing of the subvolumes,
    * because this is the same as the input volume's voxel spacing.
    */
  template <class TVolume>
  class Subvolumer
    {
    public:

      typedef typename TVolume::value_type TValue;
      typedef typename TVolume::index_type TIndex;
      typedef typename TVolume::space_type TSpace;

      /** Constructor.
        *
        * @param dims  The dimensions of the input volume (z,y,x).
        * @param spacing  The voxel spacing of the input volume.
        * @param origin  The origin of the input volume.
        * @param subvolumes  The number of subvolumes in which to divide the volume.
        */
      Subvolumer(bonelab::Tuple<3,TIndex> dims,
                 bonelab::Tuple<3,TSpace> spacing,
                 bonelab::Tuple<3,TSpace> origin,
                 int subvolumes);

      /** Get the dimensions of subvolume i. */
      bonelab::Tuple<3,TIndex> GetSubvolumeDims(int index) {return this->m_SubvolumeDims[index];}

      /** Get the origin of subvolume i. */
      bonelab::Tuple<3,TSpace> GetSubvolumeOrigin(int index) {return this->m_SubvolumeOrigins[index];}

    protected:
      
      std::vector<TIndex> m_SubvolumeStartZIndex;
      std::vector<bonelab::Tuple<3,TIndex> > m_SubvolumeDims;
      std::vector<bonelab::Tuple<3,TSpace> > m_SubvolumeOrigins;

    };  // class Subvolumer

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "Subvolumer.txx"

#endif
