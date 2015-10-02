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

#include "MetaImageReader.hpp"
#include "AthabascaException.hpp"
#include "bonelab/text.hpp"
#include "bonelab/endian.hpp"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using boost::format;

namespace athabasca_recon
  {

  // Implementations
  template<typename TIndex, typename TSpace>
  template<int N, typename TValue>
  void MetaImageReader<TIndex,TSpace>::Read
    (bonelab::Image<N,TValue,TIndex,TSpace>& image)
    {
    this->GetInformation();
    this->GetData<N,TValue>(image);
    }

  template<typename TIndex, typename TSpace>
  template<int N, typename TValue>
  void MetaImageReader<TIndex,TSpace>::GetData
    (bonelab::Image<N,TValue,TIndex,TSpace>& image)
    {
    athabasca_verbose_assert(N == this->m_NDims,
      "Number of dimensions in MetaData file doesn't match number of requested dimensions.");
    athabasca_verbose_assert(this->m_DataType == bonelab::TemplatedDataType<TValue>(),
      "Data types in MetaData file doesn't match requested data type.");
    bonelab::Tuple<N,TIndex> dims(this->m_DimSize);
    bonelab::Tuple<N,TSpace> pixelsize;
    if (this->m_ElementSpacing)
      { pixelsize = *this->m_ElementSpacing; }
    else
      { pixelsize = bonelab::Tuple<N,TSpace>::ones(); }
    bonelab::Tuple<N,TSpace> origin;
    if (this->m_Offset)
      { origin = *this->m_Offset; }
    if (image.is_constructed())
      {
      athabasca_assert(image.dims() == dims);
      image.spacing() = pixelsize;
      image.origin() = origin;
      }
    else
      {
      image.construct(dims, pixelsize, origin);
      }
    athabasca_assert(!this->m_RawFileName.empty());
    std::ifstream rawFileStream;
    rawFileStream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
      {
      rawFileStream.open(this->m_RawFileName.c_str(), std::ios::binary);
      }
    catch (...)
      { throw_athabasca_exception((format("Error opening file %s") % this->m_RawFileName).str()); }
    try
      {
      rawFileStream.read(reinterpret_cast<char*>(image.ptr()),
                         sizeof(TValue)*image.size());
      rawFileStream.close();
      }
    catch (...)
      { throw_athabasca_exception((format("Error reading file %s") % this->m_RawFileName).str()); }
    if (this->m_BigEndian)  // whether this flag exists
      {
      if (*this->m_BigEndian)  // what the value of this flag is
        {
        bonelab::EndianConversion<bonelab::BIG_ENDIAN_ORDER, bonelab::HOST_ENDIAN_ORDER>
            (image.ptr(), image.size());
        }
      else
        {
        bonelab::EndianConversion<bonelab::LITTLE_ENDIAN_ORDER, bonelab::HOST_ENDIAN_ORDER>
            (image.ptr(), image.size());
        }
      }
    }

  template <typename TIndex, typename TSpace>
  void MetaImageReader<TIndex,TSpace>::GetInformation()
    {
    athabasca_assert(!this->m_FileName.empty());
    std::ifstream mhd_file;
    try
      {
      mhd_file.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
      mhd_file.open(this->m_FileName.c_str());
      }
    catch (...)
      { throw_athabasca_exception((format("Error opening file %s") % this->m_FileName).str()); }
      
    try
      {
      std::map<std::string,std::string> meta;
      this->ParseMeta(mhd_file, meta);
    
      if (!meta.count("ObjectType"))
        {
        // throw_athabasca_exception("Missing meta field: ObjectType.");
        }
      else if (meta["ObjectType"] != "Image")
        { throw_athabasca_exception("ObjectType is not Image."); }

      if (!meta.count("NDims"))
        { throw_athabasca_exception("Missing meta field: NDims."); }
      try
        {
        this->m_NDims = boost::lexical_cast<TIndex>(meta["NDims"]);
        }
      catch(boost::bad_lexical_cast &)
        { throw_athabasca_exception("Error: Unable to parse NDims field.\n"); }
      
      if (!meta.count("BinaryData"))
        {
        // throw_athabasca_exception("Missing meta field: BinaryData.");
        }
      else if (meta["BinaryData"] != "True")
        { throw_athabasca_exception("No support yet for BinaryData = False."); }
    
      // Not sure what the difference between BinaryDataByteOrderMSB and
      // ElementByteOrderMSB is, but they both seem to occur.
      if (meta.count("BinaryDataByteOrderMSB"))
        {
        if (meta["BinaryDataByteOrderMSB"] == "True")
          { this->m_BigEndian = true; }
        else if (meta["BinaryDataByteOrderMSB"] == "False")
          { this->m_BigEndian = false; }
        else
          { throw_athabasca_exception("Unrecognized value for BinaryDataByteOrderMSB."); }
        }
      if (meta.count("ElementByteOrderMSB"))
        {
        if (meta["ElementByteOrderMSB"] == "True")
          { this->m_BigEndian = true; }
        else if (meta["ElementByteOrderMSB"] == "False")
          { this->m_BigEndian = false; }
        else
          { throw_athabasca_exception("Unrecognized value for ElementByteOrderMSB."); }
        }
    
      if (meta.count("CompressedData") &&
          meta["CompressedData"] != "False")
        { throw_athabasca_exception("No support yet for CompressedData = True"); }
    
      std::vector<std::string> tokens;
      if (meta.count("Offset"))
        {
        this->m_Offset.reset(std::vector<TSpace>());
        this->m_Offset->resize(this->m_NDims);
        bonelab::split_arguments(meta["Offset"], tokens);
        if (tokens.size() != this->m_NDims)
          { throw_athabasca_exception("Error parsing Offset field."); }
        try
          {
          for (int i=0; i<this->m_NDims; i++)
            { (*this->m_Offset)[i] = boost::lexical_cast<TSpace>(tokens[this->m_NDims-1-i]); }
          }
        catch(boost::bad_lexical_cast &)
          { throw_athabasca_exception("Error: Unable to parse Offset field.\n"); }
        }

      if (meta.count("ElementSpacing"))
        {
        this->m_ElementSpacing.reset(std::vector<TSpace>());
        this->m_ElementSpacing->resize(this->m_NDims);
        bonelab::split_arguments(meta["ElementSpacing"], tokens);
        if (tokens.size() != this->m_NDims)
          { throw_athabasca_exception("Error parsing ElementSpacing field."); }
        try
          {
          for (int i=0; i<this->m_NDims; i++)
            { (*this->m_ElementSpacing)[i] = boost::lexical_cast<TSpace>(tokens[this->m_NDims-1-i]); }
          }
        catch(boost::bad_lexical_cast &)
          { throw_athabasca_exception("Error: Unable to parse ElementSpacing field.\n"); }
        }

      if (!meta.count("DimSize"))
        { throw_athabasca_exception("Missing meta field: DimSize."); }
      bonelab::split_arguments(meta["DimSize"], tokens);
      if (tokens.size() != this->m_NDims)
        { throw_athabasca_exception("Error parsing DimSize field."); }
      m_DimSize.resize(this->m_NDims);
      try
        {
        for (int i=0; i<this->m_NDims; i++)
          { this->m_DimSize[i] = boost::lexical_cast<TIndex>(tokens[this->m_NDims-1-i]); }
        }
      catch(boost::bad_lexical_cast &)
        { throw_athabasca_exception("Error: Unable to parse DimSize field.\n"); }

      std::map<std::string, bonelab::DataType_t> elementTypeMap;
        elementTypeMap["MET_CHAR"  ] = bonelab::BONELAB_INT8;
        elementTypeMap["MET_UCHAR" ] = bonelab::BONELAB_UINT8;
        elementTypeMap["MET_SHORT" ] = bonelab::BONELAB_INT16;
        elementTypeMap["MET_USHORT"] = bonelab::BONELAB_UINT16;
        elementTypeMap["MET_INT"   ] = bonelab::BONELAB_INT32;
        elementTypeMap["MET_UINT"  ] = bonelab::BONELAB_UINT32;
        elementTypeMap["MET_LONG"  ] = bonelab::BONELAB_INT64;
        elementTypeMap["MET_ULONG" ] = bonelab::BONELAB_UINT64;
        elementTypeMap["MET_FLOAT" ] = bonelab::BONELAB_FLOAT32;
        elementTypeMap["MET_DOUBLE"] = bonelab::BONELAB_FLOAT64;
      if (!meta.count("ElementType"))
        { throw_athabasca_exception("Missing meta field: ElementType."); }
      if (!elementTypeMap.count(meta["ElementType"]))
        { throw_athabasca_exception("Unrecognized value for ElementType."); }
      this->m_DataType = elementTypeMap[meta["ElementType"]];

      if (!meta.count("ElementDataFile"))
        { throw_athabasca_exception("Missing meta field: ElementDataFile."); }
      this->m_RawFileName = meta["ElementDataFile"];
      
      } // try
    catch (AthabascaException& e)
      {
      throw AthabascaException((format("Error processing file %s: %s") % this->m_FileName % e.what()).str(),
                e.file(),
                e.line());
      }
    catch (std::exception& e)
      {
      throw AthabascaException((format("Error processing file %s: %s") % this->m_FileName % e.what()).str());
      }
    catch (...)
      { throw_athabasca_exception((format("Error processing file %s") % this->m_FileName).str()); }
    }

  template <typename TIndex, typename TSpace>
  void MetaImageReader<TIndex,TSpace>::ParseMeta
    (
    std::istream& s,
    std::map<std::string,std::string>& meta
    )
    {
    std::string line;
    s.exceptions(std::ios::goodbit);
    getline(s, line);
    while (!s.eof())
      {
      if (line.empty())
        { continue; }
      std::vector<std::string> tokens;
      bonelab::split_arguments(line, tokens, "=");
      if (tokens.size() != 2)
        { throw_athabasca_exception("Error parsing meta data."); }
      boost::trim(tokens[0]);
      boost::trim(tokens[1]);
      meta[tokens[0]] = tokens[1];
      getline(s, line);
      }
    }

  }  // namespace athabasca_recon
