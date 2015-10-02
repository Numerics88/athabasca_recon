/*
Copyright (C) 2011-2015 Eric Nodwell
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

#ifndef BONELAB_StreamingWriter_hpp_INCLUDED
#define BONELAB_StreamingWriter_hpp_INCLUDED

#include "io/StreamingIOBase.hpp"
#include "bonelab/Image.hpp"

namespace athabasca_recon
  {

  /** Untemplated base class for StreamingWriter.
    * Useful for for storing a pointer to any type of StreamingWriter;
    * the pointer can be dynamically cast to the correct templated type at run time.
    */
  class StreamingWriterBase : public StreamingIOBase {};

  /** A base class for a file writer that allows Array data to be written
    * in slices.
    */
  template <typename TSlice>
  class StreamingWriter : public StreamingWriterBase
    {
    public:

      typedef typename TSlice::value_type TValue;
      typedef typename TSlice::index_type TIndex;
      typedef typename TSlice::space_type TSpace;
      typedef typename bonelab::Image<3,TValue,TIndex,TSpace> TStack;

      /** Constructor. */
      StreamingWriter()
        :
        m_SliceDimensions(bonelab::Tuple<2,TIndex>::zeros()),
        m_Origin(bonelab::Tuple<3,TSpace>::zeros()),
        m_ElementSize(bonelab::Tuple<3,TSpace>::zeros()),
        m_NumberOfSlices(0)
        {}

      virtual ~StreamingWriter() {}

      /** Sets the 2D dimensions of the slices. */
      virtual void SetSliceDimensions(bonelab::Tuple<2,TIndex> arg) {m_SliceDimensions = arg;}
      bonelab::Tuple<2,TIndex> GetSliceDimensions() {return m_SliceDimensions;}

      /** Sets the origin of the slice stack (3D). */
      virtual void SetOrigin(bonelab::Tuple<3,TSpace> arg) {m_Origin = arg;}
      bonelab::Tuple<3,TSpace> GetOrigin() {return m_Origin;}

      /** Sets the 2D origin of a slice.
        * This method is useful when the 3D origin doesn't have any meaning,
        * for example, there is no 3rd space dimension in a stack of
        * projections, although there is in a volume which also consists of
        * slices.
        * The spacing of the third dimension is set to zero.
        */
      virtual void SetSliceOrigin(bonelab::Tuple<2,TSpace> arg)
        { m_Origin = bonelab::Tuple<3, TSpace>(0, arg[0], arg[1]); }

      /** Sets the 3D element size. */
      virtual void SetElementSize(bonelab::Tuple<3,TSpace> arg) {m_ElementSize = arg;}
      bonelab::Tuple<3,TSpace> GetElementSize() {return m_ElementSize;}

      /** Sets the 2D slice element size.
        * This method is useful when the 3D element size has no meaning.
        */
      virtual void SetPixelSize(bonelab::Tuple<2,TSpace> arg)
        { m_ElementSize = bonelab::Tuple<3, TSpace>(0, arg[0], arg[1]);}

      // virtual void SetParametersFromImage(const TSlice& image);

      /** Return the number of slices written so far. */
      int GetNumberOfSlices() {return m_NumberOfSlices;}

      /** Initialization.
        * Must be called before writing any data.
        */
      virtual void Initialize() = 0;
      
      /** Writes a single slice. */
      virtual void WriteSlice(TSlice& image) = 0;

      /** Writes a stack of slices. */
      virtual void WriteStack(TStack& stack) = 0;
      
      /** Cleans up.
        * Typically meta data gets written at this point.
        */
      virtual void Close() = 0;

    protected:

      bonelab::Tuple<2,TIndex> m_SliceDimensions;
      bonelab::Tuple<3,TSpace> m_Origin;
      bonelab::Tuple<3,TSpace> m_ElementSize;
      int m_NumberOfSlices;

    };  // class StreamingWriter

  } // namespace bonelab

#endif
