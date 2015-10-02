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

#ifndef BONELAB_StreamingMetaImageProjectionsReader_HPP_INCLUDED
#define BONELAB_StreamingMetaImageProjectionsReader_HPP_INCLUDED

#include "StreamingRawProjectionsReader.hpp"
#include <string>

namespace athabasca_recon
  {

  /** A streaming reader for ITK MetaImage .mhd files. */
  template <typename TProjection>
  class StreamingMetaImageProjectionsReader : public StreamingRawProjectionsReader<TProjection>
    {
    public:

      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;

      virtual void Initialize();

    };  // class StreamingMetaImageProjectionsReader

  } // namespace athabasca_recon

// Include the template definitions so we don't require explicit instantiations
#include "StreamingMetaImageProjectionsReader.txx"

#endif
