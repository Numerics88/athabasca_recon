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

#ifndef BONELAB_MetaImageReader_HPP_INCLUDED
#define BONELAB_MetaImageReader_HPP_INCLUDED

#include "bonelab/Image.hpp"
#include "bonelab/types.hpp"
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <map>
#include <vector>
#include <string>
#include <fstream>

namespace athabasca_recon
  {

  /** A file reader that reads ITK MetaImage .mhd files.
    *
    * This reader parses the meta data and reads the data.
    *
    * Refer to the manual for more information on the MetaImage file format.
    *
    * Note: Unified .mha files are not supported.
    */
  template <typename TIndex, typename TSpace>
  class MetaImageReader : private boost::noncopyable
    {
    public:

      /** Default constructor.
        * You must subsequently call SetFileName.
        */
      MetaImageReader()
        : m_NDims(0)
        {}

      /** Constructor with file name. */
      MetaImageReader(const char* fn)
        :
        m_FileName(fn),
        m_NDims(0)
        {}

      /** Sets the file name. */
      virtual void SetFileName(std::string fn) {this->m_FileName = fn;}

      /** Process the meta data information.
        * This must be called before trying to retrieve any paramters.
        */ 
      virtual void GetInformation();
      
      /** Returns the number of dimensions of the data. */
      int GetNDims() {return this->m_NDims;}
      
      /** Returns the dimensions of the data. */
      std::vector<TIndex> GetDimSize() {return m_DimSize;}

      /** Returns the data type. */
      bonelab::DataType_t GetDataType() {return this->m_DataType;}
      
      /** Returns the endianness of the data. */
      bool IsBigEndian() {return *this->m_BigEndian;}

      /** Returns true if the endianness of the data is specified. */
      bool HasEndian() {return bool(this->m_BigEndian);}
      
      /** Returns the offset, or origin, of the data. */
      std::vector<TSpace> GetOffset() {return *this->m_Offset;}
      
      /** Returns true if the offest of the data is specified. */
      bool HasOffset() {return bool(this->m_Offset);}
      
      /** Returns the element spacing. */
      std::vector<TSpace> GetElementSpacing() {return *this->m_ElementSpacing;}
      
      /** Returns true is the element spacing is specified. */
      bool HasElementSpacing() {return bool(this->m_ElementSpacing);}
      
      /** Get the raw data file name. */      
      std::string GetRawFileName() {return m_RawFileName;}

      /** Reads the complete raw data into image.
        * Note that if you require part of the data, or want to stream the
        * data, this method is not appropriate.
        * This can only be called after a call to GetInformation.
        * See StreamingMetaImageProjectionsReader.
        */
      template<int N, typename TValue>
      void GetData(bonelab::Image<N,TValue,TIndex,TSpace>& image);

      /** Calls both GetInformation and GetData. */
      template<int N, typename TValue>
      void Read(bonelab::Image<N,TValue,TIndex,TSpace>& image);

    protected:

      std::string m_FileName;

      int m_NDims;
      bonelab::DataType_t m_DataType;
      std::vector<TIndex> m_DimSize;
      boost::optional<bool> m_BigEndian;
      boost::optional<std::vector<TSpace> > m_Offset;
      boost::optional<std::vector<TSpace> > m_ElementSpacing;
      std::string m_RawFileName;

      virtual void ParseMeta(
          std::istream& s,
          std::map<std::string,std::string>& meta);

    };  // class MetaImageReader

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "MetaImageReader.txx"

#endif
