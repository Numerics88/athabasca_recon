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

#ifndef BONELAB_StreamingProjectionsReader_HPP_INCLUDED
#define BONELAB_StreamingProjectionsReader_HPP_INCLUDED

#include "io/StreamingIoBase.hpp"
#include "bonelab/Image.hpp"

namespace athabasca_recon
  {

  /** Untemplated base class for StreamingProjectionsReader.
    * Useful for for storing a pointer to any type of StreamingProjectionsReader;
    * the pointer can be dynamically cast to the correct templated type at run time.
    */
  class StreamingProjectionsReaderBase : public StreamingIOBase {};

  /** An abstract base class for a file reader for reading projections.
    * This class streams projections, in that they are read only as requested.
    * They must be requested in order however.  Therefore, it is generally
    * not appropriate to use this class in worker threads.
    *
    * Note that ALL parameters must be set (dimensions, origin, etc...), even
    * for file formats that store these.  The correct procedure is to read
    * them when processing the configuration file if necessary (see
    * ReconConfiguration), then set the values in this reader.  That allows
    * the user to override the stored parameters when desired.
    */
  template <typename TProjection>
  class StreamingProjectionsReader : public StreamingProjectionsReaderBase
    {
    public:

      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;

      /** Default constructor. */
      StreamingProjectionsReader()
        :
        m_Dimensions(bonelab::Tuple<2,TIndex>::zeros()),
        m_Origin(bonelab::Tuple<2,TSpace>::zeros()),
        m_PixelSize(bonelab::Tuple<2,TSpace>::zeros()),
        m_NumberOfProjections(0),
        m_ProjectionStride(1),
        m_BigEndian(false),
        m_ReadCount(0)
        {}

      virtual ~StreamingProjectionsReader() {}

      /** Set/get the projections file name. */
      virtual void SetFileName(std::string fn) {this->m_FileName = fn;}
      std::string GetFileName() {return this->m_FileName;}
    
      /** Set/get the dimensions of the projections. */
      virtual void SetDimensions(bonelab::Tuple<2,TIndex> arg) {m_Dimensions = arg;}
      bonelab::Tuple<2,TIndex> GetDimensions() {return m_Dimensions;}

      /** Set/get the origin of the projections.
        * Note that the origin positions the projection relative to the central ray.
        */
      virtual void SetOrigin(bonelab::Tuple<2,TSpace> arg) {m_Origin = arg;}
      bonelab::Tuple<2,TSpace> GetOrigin() {return m_Origin;}

      /** Set/get the pixel size of the projections.
        * No particular units are assumed; consistent units must be used throughout.
        */
      virtual void SetPixelSize(bonelab::Tuple<2,TSpace> arg) {m_PixelSize = arg;}
      bonelab::Tuple<2,TSpace> GetPixelSize() {return m_PixelSize;}

      /** Set/get the number of projections.
        * For parallel projections, this is the number of projections in
        * 180 degrees, and may be inclusive or exclusive of the projection
        * at 180 degrees, depending on the setting of the parameter
        * Projections.ProjectionAt180 .
        */
      virtual void SetNumberOfProjections(int arg) {m_NumberOfProjections = arg;}
      int GetNumberOfProjections() {return m_NumberOfProjections;}

      /** Set/get a projection stride.
        * If set to N, every Nth projection will be read.
        * Note: Do not adjust NumberOfProjections when setting ProjectionStride;
        *       it should remain as the total number of projections in the file.
        * Default is 1.
        */
      virtual void SetProjectionStride(int arg) {m_ProjectionStride = arg;}
      int GetProjectionStride() {return m_ProjectionStride;}

      /** Sets the endianness of the data.
        * The default is the machine endianness.  But note that many image
        * formats use big endian, so leaving this as the default with Intel
        * CPUs is generally catastrophic.
        */
      virtual void SetBigEndian(bool bigEndian) {this->m_BigEndian = bigEndian;}

      /** Initialize.
        * Must be called before GetNextProjection can be used.
        */
      virtual void Initialize() = 0;

      /** Read the next projection into image. */
      virtual void GetNextProjection(TProjection& image) = 0;
      
      /** Rewind the file to the first projection. */
      virtual void Rewind() = 0;
      
      /** Clean up.  Called automatically in the destructor. */
      virtual void CloseReadingProjections() = 0;

    protected:

      std::string m_FileName;
      bonelab::Tuple<2,TIndex> m_Dimensions;
      bonelab::Tuple<2,TSpace> m_Origin;
      bonelab::Tuple<2,TSpace> m_PixelSize;
      int m_NumberOfProjections;
      int m_ProjectionStride;
      bool m_BigEndian;
      int m_ReadCount;

    };  // class StreamingProjectionsReader

  } // namespace bonelab

#endif
