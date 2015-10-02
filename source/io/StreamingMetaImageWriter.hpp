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

#ifndef BONELAB_StreamingMetaImageWriter_hpp_INCLUDED
#define BONELAB_StreamingMetaImageWriter_hpp_INCLUDED

#include "StreamingRawWriter.hpp"
#include <fstream>
#include <string>

namespace athabasca_recon
  {

  /** A streaming writer class for writing ITK MetaImage .mhd files.
    *
    * Refer to the manual for more information on the MetaImage file format.
    *
    * Note: Unified .mha files are not supported.
    */  
  template <typename TSlice>
  class StreamingMetaImageWriter : public StreamingRawWriter<TSlice>
    {
    public:

      typedef typename TSlice::value_type TValue;
      typedef typename TSlice::index_type TIndex;
      typedef typename TSlice::space_type TSpace;
      typedef typename bonelab::Image<3,TValue,TIndex,TSpace> TSliceStack;

      virtual ~StreamingMetaImageWriter();

      virtual void SetFileName(std::string fn);

      virtual void Close();

      /** Writes just the meta data to a .mhd file. */
      virtual void WriteMetaFile();

    protected:

      std::string m_MetaFileName;

    };  // class StreamingMetaImageWriter

  } // namespace athabasca_recon

// Include the template definitions so we don't require explicit instantiations
#include "StreamingMetaImageWriter.txx"

#endif
