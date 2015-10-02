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

#ifndef BONELAB_StreamingRawProjectionsReader_HPP_INCLUDED
#define BONELAB_StreamingRawProjectionsReader_HPP_INCLUDED

#include "StreamingProjectionsReader.hpp"
#include <fstream>
#include <string>

namespace athabasca_recon
  {

  /** A streaming reader for raw binary data files. */
  template <typename TProjection>
  class StreamingRawProjectionsReader : public StreamingProjectionsReader<TProjection>
    {
    public:

      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;

      /** Set/get the name of the raw file. */
      virtual void SetFileName(std::string fn);

      virtual void Initialize();
      
      virtual void GetNextProjection(TProjection& image);
      
      virtual void Rewind();
      
      virtual void CloseReadingProjections();

    protected:

      // this is in addition to File Name so subclasses can read data from a
      // different file as necessary. e.g. MetaImage reader.
      std::string m_ProjectionsFileName;
      std::ifstream m_ProjectionsFileStream;

    };  // class StreamingRawProjectionsReader

  } // namespace athabasca_recon

// Include the template definitions so we don't require explicit instantiations
#include "StreamingRawProjectionsReader.txx"

#endif
