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

#define BOOST_FILESYSTEM_VERSION 3
#include "MetaImageWriter.hpp"
#include <boost/filesystem.hpp>

namespace athabasca_recon
  {

  template <typename TSlice>
  StreamingMetaImageWriter<TSlice>::~StreamingMetaImageWriter()
    {
    if (this->m_DataFileStream.is_open())
      { this->Close(); }
    }

  template <typename TSlice>
  void StreamingMetaImageWriter<TSlice>::SetFileName
    (std::string fn)
    {
    this->m_MetaFileName = fn;
    this->m_DataFileName = boost::filesystem::change_extension(
                                  this->m_MetaFileName, ".raw").string();
    }

  template <typename TSlice>
  void StreamingMetaImageWriter<TSlice>::Close()
    {
    this->m_DataFileStream.close();
    this->WriteMetaFile();
    }

  template <typename TSlice>
  void StreamingMetaImageWriter<TSlice>::WriteMetaFile()
    {      
    MetaImageWriter<TSliceStack> meta_writer;
    meta_writer.SetFileName(this->m_MetaFileName);
    meta_writer.SetRawFileName(this->m_DataFileName);
    bonelab::Tuple<3,TIndex> dims(this->m_NumberOfSlices,
                                  this->m_SliceDimensions[0],
                                  this->m_SliceDimensions[1]);
    meta_writer.SetDimSize(dims);
    meta_writer.SetOffset(this->m_Origin);
    meta_writer.SetElementSpacing(this->m_ElementSize);
    meta_writer.WriteMetaFile();
    }

  }  // namespace athabasca_recon
