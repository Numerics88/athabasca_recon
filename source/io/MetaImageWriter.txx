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
#include "AthabascaException.hpp"
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/detail/endian.hpp>
#include <boost/type_traits.hpp>
#include <boost/cstdint.hpp>

using boost::int8_t;
using boost::uint8_t;
using boost::int16_t;
using boost::uint16_t;
using boost::int32_t;
using boost::uint32_t;
using boost::int64_t;
using boost::uint64_t;

using boost::algorithm::to_lower_copy;

namespace athabasca_recon
  {

  template <class TArray, typename TSpace>
  void MetaImageWriterBase<TArray,TSpace>::SetFileName
    (std::string fn)
    {
    std::string extension = to_lower_copy(boost::filesystem::extension(fn));
    assert(extension != ".raw");
    this->m_MetaFileName = fn;
    this->m_RawFileName = boost::filesystem::change_extension(
                          this->m_MetaFileName, ".raw").string();
    }

  template <class TArray, typename TSpace>
  void MetaImageWriterBase<TArray,TSpace>::WriteMetaFile()
    {
    assert(!this->m_MetaFileName.empty());
    std::ofstream outfile;
    outfile.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
      { outfile.open(this->m_MetaFileName.c_str()); }
    catch (...)
      { throw_athabasca_exception((format("Error opening file %s") % this->m_MetaFileName).str()); }
    try
      { this->FormatMetaInfo(outfile); }
    catch (AthabascaException& e)
      {
      throw AthabascaException((format("Error occurred writing file %s: %s") % this->m_MetaFileName % e.what()).str(),
                           e.file(), e.line());
      }
     catch (std::exception& e)
      {
      throw_athabasca_exception((format("Error occurred writing file %s: %s") % this->m_MetaFileName % e.what()).str());
      }
    catch (...)
      {
      throw_athabasca_exception((format("Error occurred writing file %s") % this->m_MetaFileName).str());
      }
    }

  template <class TArray, typename TSpace>
  void MetaImageWriterBase<TArray,TSpace>::WriteRawFile()
    {
    assert(!this->m_RawFileName.empty());
    std::ofstream outfile;
    outfile.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
      {
      outfile.open(this->m_RawFileName.c_str(), std::ios::binary);
      safe_stream_write(outfile, m_Data, sizeof(TValue)*long_product(this->m_DimSize));
      }
    catch (...)
      { throw_athabasca_exception((format("Error writing to file %s") % this->m_RawFileName).str()); }
    }

  template <class TArray, typename TSpace>
  void MetaImageWriterBase<TArray,TSpace>::FormatMetaInfo(std::ostream& s)
    {
    s << "ObjectType = Image\n"
      << "NDims = " << N << '\n'
      << "BinaryData = True\n";
#ifdef BOOST_LITTLE_ENDIAN
      s << "BinaryDataByteOrderMSB = False\n";
#elif defined BOOST_BIG_ENDIAN
      s << "BinaryDataByteOrderMSB = True\n";
#else
#error "Can't determine system endianness."
#endif
    s << "CompressedData = False\n";
    s << "Offset =";
    for (int i=N-1; i>=0; i--)
      { s << " " << this->m_Offset[i]; }
    s << "\nElementSpacing =";
    for (int i=N-1; i>=0; i--)
      { s << " " << this->m_ElementSpacing[i]; }
    s << "\nDimSize =";
    for (int i=N-1; i>=0; i--)
      { s << " " << this->m_DimSize[i]; }
    s << "\nElementType = " << MetaImageDataTypeLabel<TValue>() << '\n';
    s << "ElementDataFile = " << this->m_RawFileName << '\n';
    }

  template <int N, typename TValue, typename TIndex>
  MetaImageWriter<bonelab::Array<N,TValue,TIndex> >::MetaImageWriter
    (
    const bonelab::Array<N,TValue,TIndex>& array,
    std::string fileName
    )
    {
    this->m_DimSize = array.dims();
    this->m_Data = array.ptr();
    this->SetFileName(fileName);
    }

  template <int N, typename TValue, typename TIndex, typename TSpace>
  MetaImageWriter<bonelab::Image<N,TValue,TIndex,TSpace> >::MetaImageWriter
    (
    const bonelab::Image<N,TValue,TIndex,TSpace>& image,
    std::string fileName
    )
    {
    this->m_DimSize = image.dims();
    this->m_ElementSpacing = image.spacing();
    this->m_Offset = image.origin();
    this->m_Data = image.ptr();
    this->SetFileName(fileName);
    }

  // Specialized instantiations

  template<> inline const char* MetaImageDataTypeLabel<int8_t>() {return "MET_CHAR";}
  template<> inline const char* MetaImageDataTypeLabel<uint8_t>() {return "MET_UCHAR";}
  template<> inline const char* MetaImageDataTypeLabel<int16_t>() {return "MET_SHORT";}
  template<> inline const char* MetaImageDataTypeLabel<uint16_t>() {return "MET_USHORT";}
  template<> inline const char* MetaImageDataTypeLabel<int32_t>() {return "MET_INT";}
  template<> inline const char* MetaImageDataTypeLabel<uint32_t>() {return "MET_UINT";}
  template<> inline const char* MetaImageDataTypeLabel<int64_t>() {return "MET_LONG";}
  template<> inline const char* MetaImageDataTypeLabel<uint64_t>() {return "MET_ULONG";}
  template<> inline const char* MetaImageDataTypeLabel<float>() {return "MET_FLOAT";}
  template<> inline const char* MetaImageDataTypeLabel<double>() {return "MET_DOUBLE";}

  }  // namespace athabasca_recon
