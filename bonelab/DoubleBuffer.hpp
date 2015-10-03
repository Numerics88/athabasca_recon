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

#ifndef BONELAB_DoubleBuffer_hpp_INCLUDED
#define BONELAB_DoubleBuffer_hpp_INCLUDED

#include "Image.hpp"

namespace bonelab
  {

  /** A base class for DoubleBuffer.
    * This class provides methods that are used in all
    * specializations of MetaImageWriter.
    */
  template <class TObject>
  class DoubleBufferBase : private boost::noncopyable
    {

    protected:

      TObject m_A;
      TObject m_B;
      TObject* m_Loading;
      TObject* m_Active;

    public:

      DoubleBufferBase()
        :
        m_Loading(&m_A),
        m_Active(&m_B)
        {}

      inline const TObject& active() const
        { return *this->m_Active; }
      inline TObject& active()
        { return *this->m_Active; }

      inline const TObject& loading() const
        { return *this->m_Loading; }
      inline TObject& loading()
        { return *this->m_Loading; }

      void swap()
        { 
        TObject* tmp = this->m_Loading;
        this->m_Loading = this->m_Active;
        this->m_Active = tmp;
        }
      
    };


  template <class TObject>
  class DoubleBuffer : public DoubleBufferBase<TObject>
    {};


  /**
    */
  template <int N, typename TValue, typename TIndex>
  class DoubleBuffer<Array<N,TValue,TIndex> > : public DoubleBufferBase<Array<N,TValue,TIndex> >
    {

    public:

      /** Empty constructor.
        * You must subsequently call construct or construct_reference explicitly.
        */
      DoubleBuffer()
        {}

      /** Constructor to allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        */
      DoubleBuffer(Tuple<N,TIndex> dims)
        {
        this->m_A.construct(dims);
        this->m_B.construct(dims);
        }

      /** Constructor to create reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param dataA  A pointer to the data for one buffer.
        * @param dataB  A pointer to the data for the other buffer.
        * @param dims  The dimensions of the array.
        */
      DoubleBuffer(TValue* dataA, TValue* dataB, Tuple<N,TIndex> dims)
        {
        this->m_A.construct(dataA, dims);
        this->m_B.construct(dataB, dims);          
        }

      /** Constructor to create reference to existing data in Array objects.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Arrays remain in existence for as
        * long as you want to use this reference.
        *
        * @param sourceA  An existing Array object.
        * @param sourceB  An existing Array object.
        */
      DoubleBuffer(const Array<N,TValue,TIndex>& sourceA,
                   const Array<N,TValue,TIndex>& sourceB)
        {
        this->m_A.construct(sourceA, sourceA.dims());
        this->m_B.construct(sourceB, sourceB.dims());
        }

      /** Allocate space.
        *
        * @param dims  The dimensions of the arrays to allocate.
        */
      void construct(Tuple<N,TIndex> dims)
        {
        this->m_A.construct(dims);
        this->m_B.construct(dims);
        }

      /** Create a reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param dataA  A pointer to the data for one buffer.
        * @param dataB  A pointer to the data for the other buffer.
        * @param dims  The dimensions of the array.
        */
      void construct_reference(const TValue* dataA, const TValue* dataB, Tuple<N,TIndex> dims)
        {
        this->m_A.construct(dataA, dims);
        this->m_B.construct(dataB, dims);
        }

      /** Create a reference to to existing data in Array objects.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Arrays remain in existence for as
        * long as you want to use this reference.
        *
        * @param sourceA  An existing Array object.
        * @param sourceB  An existing Array object.
        */
      void construct_reference(const Array<N,TValue,TIndex>& sourceA,
                               const Array<N,TValue,TIndex>& sourceB)
        {
        this->m_A.construct(sourceA);
        this->m_B.construct(sourceB);
        }

      void destruct()
        {
        this->m_A.destruct();
        this->m_B.destruct();
        }

      /** Returns true if the DoubleBuffer Arrays have been constructed. */
      inline bool is_constructed() const
        { return this->m_A.is_constructed(); }

      /** Returns the flattened (1D) size of the array. */
      inline size_t size() const
        { return this->m_A.size(); }

      /** Returns the dimensions of the DoubleBuffer Arrays. */
      inline Tuple<N,TIndex> dims() const
        { return this->m_A.dims(); }

    }; // class DoubleBuffer<Array>


  /**
    */
  template <int N, typename TValue, typename TIndex, typename TSpace>
  class DoubleBuffer<Image<N,TValue,TIndex,TSpace> > : public DoubleBufferBase<Image<N,TValue,TIndex,TSpace> >
    {

    public:

      /** Empty constructor.
        * You must subsequently call construct or construct_reference explicitly.
        */
      DoubleBuffer()
        {}

      /** Constructor to allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      DoubleBuffer
        (
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        this->m_A.construct(dims, spacing, origin);
        this->m_B.construct(dims, spacing, origin);
        }

      /** Constructor to create reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param dataA  A pointer to the data for one buffer.
        * @param dataB  A pointer to the data for the other buffer.
        * @param dims  The dimensions of the array.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      DoubleBuffer
        (
        TValue* dataA,
        TValue* dataB,
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        this->m_A.construct(dataA, dims, spacing, origin);
        this->m_B.construct(dataB, dims, spacing, origin);          
        }

      /** Constructor to create reference to existing data in Image objects.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Images remain in existence for as
        * long as you want to use this reference.
        *
        * @param sourceA  An existing Image object.
        * @param sourceB  An existing Image object.
        */
      DoubleBuffer(const Image<N,TValue,TIndex,TSpace>& sourceA,
                   const Image<N,TValue,TIndex,TSpace>& sourceB)
        {
        this->m_A.construct(sourceA, sourceA.dims());
        this->m_B.construct(sourceB, sourceB.dims());
        }

      /** Allocate space.
        *
        * @param dims  The dimensions of the arrays to allocate.
        */
      void construct
        (
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        this->m_A.construct(dims, spacing, origin);
        this->m_B.construct(dims, spacing, origin);
        }

      /** Create a reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param dataA  A pointer to the data for one buffer.
        * @param dataB  A pointer to the data for the other buffer.
        * @param dims  The dimensions of the array.
        */
      void construct_reference
        (
        const TValue* dataA,
        const TValue* dataB,
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        this->m_A.construct(dataA, dims, spacing, origin);
        this->m_B.construct(dataB, dims, spacing, origin);
        }

      /** Create a reference to existing data in Image objects.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Images remain in existence for as
        * long as you want to use this reference.
        *
        * @param sourceA  An existing Image object.
        * @param sourceB  An existing Image object.
        */
      void construct_reference(const Image<N,TValue,TIndex,TSpace>& sourceA,
                               const Image<N,TValue,TIndex,TSpace>& sourceB)
        {
        this->m_A.construct(sourceA);
        this->m_B.construct(sourceB);
        }

      void destruct()
        {
        this->m_A.destruct();
        this->m_B.destruct();
        }

      /** Returns true if the DoubleBuffer has been constructed. */
      inline bool is_constructed() const
        { return this->m_A.is_constructed(); }

      /** Returns the flattened (1D) size of the array. */
      inline size_t size() const
        { return this->m_A.size(); }

      /** Returns the dimensions of the DoubleBuffer. */
      inline Tuple<N,TIndex> dims() const
        { return this->m_A.dims(); }

      /** Returns the pixel spacing. */
      inline Tuple<N,TSpace> spacing() const
        { return this->m_A.spacing(); }
      
      /** Returns the origin.
        * The origin defines the center (not the corner) of the zero-indexed
        * pixel.
        */
      inline Tuple<N,TSpace> origin() const
        { return this->m_A.origin(); }

    }; // class DoubleBuffer<Image>

  } // namespace bonelab

#endif
