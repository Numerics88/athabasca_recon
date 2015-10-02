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

#ifndef BONELAB_AnyFormatStreamingProjectionsReader_HPP_INCLUDED
#define BONELAB_AnyFormatStreamingProjectionsReader_HPP_INCLUDED

#include "StreamingProjectionsReader.hpp"
#include <string>

namespace athabasca_recon
  {

  /** A streaming reader that will select the appropriate reader implementation
    * based on the file extension of the specified file.
    */
  template <typename TProjection>
  class AnyFormatStreamingProjectionsReader
    : public StreamingProjectionsReader<TProjection>
    {
    public:

      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;

      AnyFormatStreamingProjectionsReader();
      ~AnyFormatStreamingProjectionsReader();

      virtual void Initialize();
      virtual void GetNextProjection(TProjection& image);
      virtual void Rewind();
      virtual void CloseReadingProjections();

  protected:

      StreamingProjectionsReader<TProjection>* m_Worker;

    };  // class AnyFormatStreamingProjectionsReader

  } // namespace bonelab

// Template definitions so we don't require explicit instantiations
#include "AnyFormatStreamingProjectionsReader.txx"

#endif
