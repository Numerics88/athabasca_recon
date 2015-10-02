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

#ifndef BONELAB_MetaImageWriterBase_HPP_INCLUDED
#define BONELAB_MetaImageWriterBase_HPP_INCLUDED

#define BOOST_FILESYSTEM_VERSION 3
#include "bonelab/Image.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/noncopyable.hpp>

using boost::algorithm::to_lower_copy;

namespace athabasca_recon
  {

  template <typename TValue> inline const char* MetaImageDataTypeLabel();

  /** A base class for MetaImageWriter.
    * This class provides methods that are used in both Array and Image
    * specializations of MetaImageWriter.
    */
  template <class TArray, class TSpace=double>
  class MetaImageWriterBase : private boost::noncopyable
    {
    public:
      
      enum {N = TArray::dimension};
      typedef typename TArray::value_type TValue;
      typedef typename TArray::index_type TIndex;

      /** Default constructor. */
      MetaImageWriterBase()
        :
        m_Offset(bonelab::Tuple<N,TSpace>::zeros()),
        m_ElementSpacing(bonelab::Tuple<N,TSpace>::zeros()),
        m_DimSize(bonelab::Tuple<N,TIndex>::zeros()),
        m_Data(NULL)
        {}
      
      /** Writes the meta data file. */
      virtual void WriteMetaFile();
      
      /** Writes the raw data file. */
      virtual void WriteRawFile();
      
      /** Writes both the meta data and raw data files. */
      virtual void Write()
        {
        this->WriteMetaFile();
        this->WriteRawFile();
        }

      /** Sets the offset (origin). */
      virtual void SetOffset(bonelab::Tuple<N,TSpace> arg) {m_Offset = arg;}
      bonelab::Tuple<N,TSpace> GetOffset() {return m_Offset;}

      /** Sets the element spacing. */
      virtual void SetElementSpacing(bonelab::Tuple<N,TSpace> arg) {m_ElementSpacing = arg;}
      bonelab::Tuple<N,TSpace> GetElementSpacing() {return m_ElementSpacing;}

      /** Sets the dimensions. */
      virtual void SetDimSize(bonelab::Tuple<N,TIndex> arg) {m_DimSize = arg;}
      bonelab::Tuple<N,TIndex> GetDimSize() {return this->m_DimSize;}

      /** Sets the dimensions. */
      virtual void SetFileName(std::string fn);
      std::string GetFileName() {return this->m_MetaFileName;}

      /** Sets the RawDataFileName.
        * The default value is MetaFileName with extension replaced with .raw .
        */
      virtual void SetRawFileName(std::string fn) {m_RawFileName = fn;}
      std::string GetRawFileName() {return m_RawFileName;}
        
    protected:
      
      bonelab::Tuple<N,TSpace> m_Offset;
      bonelab::Tuple<N,TSpace> m_ElementSpacing;
      bonelab::Tuple<N,TIndex> m_DimSize;
      const void* m_Data;
      std::string m_MetaFileName;
      std::string m_RawFileName;
      
      void FormatMetaInfo(std::ostream& s);

    };  // class MetaImageWriterBase

  /** A file writer that writes ITK MetaImage .mhd files.
    *
    * Refer to the manual for more information on the MetaImage file format.
    *
    * It is possible to use this writer to generate only the meta data .mhd file,
    * allowing the flexibility of generating the corresponding raw data file in some other
    * way.
    *
    * Note: Unified .mha files are not supported.
    */
  template <class TArray> class MetaImageWriter {};
  
  /** Specialization of MetaImageWriter to Array. */
  template <int N, typename TValue, typename TIndex>
  class MetaImageWriter<bonelab::Array<N,TValue,TIndex> >
    : public MetaImageWriterBase<bonelab::Array<N,TValue,TIndex>,double>
    {
    public:
      
      /** Default constructor with delayed reading. */
      MetaImageWriter() {};
      
      /** Constructor providing the file name and a destination Array.
        * File is read immediately.
        */
      MetaImageWriter(const bonelab::Array<N,TValue,TIndex>& array,
                      std::string fileName);

    };  // class MetaImageWriter<Array>

  /** Specialization of MetaImageWriter to Image. */
  template <int N, typename TValue, typename TIndex, typename TSpace>
  class MetaImageWriter<bonelab::Image<N,TValue,TIndex,TSpace> >
    : public MetaImageWriterBase<bonelab::Image<N,TValue,TIndex,TSpace>,TSpace>
    {
    public:

      /** Default constructor with delayed reading. */
      MetaImageWriter() {};
      
      /** Constructor providing the file name and a destination Image.
        * File is read immediately.
        */
      MetaImageWriter (const bonelab::Image<N,TValue,TIndex,TSpace>& image,
                       std::string fileName);
    };  // class MetaImageArrayWriter<Image>

  }  // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "MetaImageWriter.txx"

#endif
