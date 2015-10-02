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

#ifndef BONELAB_StreamingRawWriter_HPP_INCLUDED
#define BONELAB_StreamingRawWriter_HPP_INCLUDED

#include "StreamingWriter.hpp"
#include <fstream>
#include <string>

namespace athabasca_recon
  {

  /** A streaming writer class for writing raw format data files. */
  template <typename TSlice>
  class StreamingRawWriter : public StreamingWriter<TSlice>
    {
    public:

      typedef typename TSlice::value_type TValue;
      typedef typename TSlice::index_type TIndex;
      typedef typename TSlice::space_type TSpace;
      typedef bonelab::Image<3,TValue,TIndex,TSpace> TStack;

      virtual ~StreamingRawWriter() {}

      /** Set/get filename. */
      virtual void SetFileName(std::string fn) {this->m_DataFileName = fn;}
  
      virtual void Initialize();
      virtual void WriteSlice(TSlice& image);
      virtual void WriteStack(TStack& stack);
      virtual void Close();

    protected:

      std::string m_DataFileName;
      std::ofstream m_DataFileStream;

    };  // class StreamingRawWriter

  } // namespace athabasca_recon

// Include the template definitions so we don't require explicit instantiations
#include "StreamingRawWriter.txx"

#endif
