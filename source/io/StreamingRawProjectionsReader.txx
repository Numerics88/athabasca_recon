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
#include <boost/iostreams/seek.hpp>

namespace athabasca_recon
  {

  template <typename TProjection>
  void StreamingRawProjectionsReader<TProjection>::SetFileName(std::string fn)
    {
    this->m_FileName = fn;
    this->m_ProjectionsFileName = fn;
    }

  template <typename TProjection>
  void StreamingRawProjectionsReader<TProjection>::Initialize()
    {
    athabasca_assert(!this->m_ProjectionsFileName.empty());
    this->m_ProjectionsFileStream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
      {
      this->m_ProjectionsFileStream.open(this->m_ProjectionsFileName.c_str(), std::ios::binary);
      }
    catch (...)
      { throw_athabasca_exception((format("Error opening file %s") % this->m_ProjectionsFileName).str()); }
    this->m_ReadCount = 0;
    }

  template <typename TProjection>
  void StreamingRawProjectionsReader<TProjection>::GetNextProjection(TProjection& image)
    {
    athabasca_assert(image.is_constructed());
    athabasca_assert(image.size() == long_product(this->m_Dimensions));
    athabasca_assert(this->m_ProjectionsFileStream.is_open());
    size_t byte_count = long_product(this->m_Dimensions)*sizeof(TValue);
    try
      {
      if (this->m_ProjectionStride > 1  && this->m_ReadCount > 0)
        {
        // seekg uses a 32 bit position in all versions of Visual Studio
        // prior to 11 (i.e. 2012).  The boost seek function supposedly is a
        // work-around (not tested, since I don't usually use Windows).
        // this->m_ProjectionsFileStream.seekg((this->m_ProjectionStride - 1)*byte_count, std::ios::cur);
        boost::iostreams::seek(this->m_ProjectionsFileStream,
                               (this->m_ProjectionStride - 1)*byte_count,
                               BOOST_IOS::cur);
        }
      this->m_ProjectionsFileStream.read((char*)image.ptr(), byte_count);
      }
    catch (...)
      { throw_athabasca_exception((format("Error reading file %s") % this->m_ProjectionsFileName).str()); }
    if (this->m_BigEndian)
      {
      bonelab::EndianConversion<bonelab::BIG_ENDIAN_ORDER, bonelab::HOST_ENDIAN_ORDER>
          (image.ptr(), image.size());
      }
    else
      {
      bonelab::EndianConversion<bonelab::LITTLE_ENDIAN_ORDER, bonelab::HOST_ENDIAN_ORDER>
          (image.ptr(), image.size());
      }
    this->m_ReadCount++;
    }

  template <typename TProjection>
  void StreamingRawProjectionsReader<TProjection>::Rewind()
    {
    athabasca_assert(this->m_ProjectionsFileStream.is_open());
    try
      {
      // seekg uses a 32 bit position in all versions of Visual Studio
      // prior to 11 (i.e. 2012).  The boost seek function supposedly is a
      // work-around (not tested, since I don't usually use Windows).
      // this->m_ProjectionsFileStream.seekg(0, std::ios::beg);
      boost::iostreams::seek(this->m_ProjectionsFileStream, 0,  BOOST_IOS::beg);
      }
    catch (...)
      { throw_athabasca_exception((format("Error reading file %s") % this->m_ProjectionsFileName).str()); }
    this->m_ReadCount = 0;
    }

  template <typename TProjection>
  void StreamingRawProjectionsReader<TProjection>::CloseReadingProjections()
    {
    if (this->m_ProjectionsFileStream.is_open())
      { this->m_ProjectionsFileStream.close(); }
    }

  }  // namespace athabasca_recon
