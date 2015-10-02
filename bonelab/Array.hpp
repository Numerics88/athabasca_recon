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

#ifndef BONELAB_Array_hpp_INCLUDED
#define BONELAB_Array_hpp_INCLUDED

#include "Tuple.hpp"
#include "exception.hpp"
#include <boost/noncopyable.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace bonelab
  {

  /**
    * A base class for Array.
    *
    * The purpose of this base class is to provide a bunch of common methods
    * that can be inherited in partial specializations of Array.  Please
    * refer to Array for more details.
    */
  template <int N, typename TValue, typename TIndex=size_t>
  class ArrayBase : private boost::noncopyable
    {

    protected:

      bool            m_is_constructed;
      size_t          m_size;
      Tuple<N,TIndex> m_dims;
      TValue*         m_buffer;
      TValue*         m_base;
      const TValue*   m_end;

    public:

      enum {dimension = N};
      typedef TValue value_type;
      typedef TIndex index_type;

      /** Empty constructor.
        * You must subsequently call construct or construct_reference explicitly.
        */
      ArrayBase()
        :
        m_is_constructed   (false),
        m_size             (0),
        m_dims             (Tuple<N,TIndex>::zeros()),
        m_buffer           (NULL),
        m_base             (NULL),
        m_end              (NULL)
        {}

      /** Constructor to allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        */
      ArrayBase(Tuple<N,TIndex> dims)
        :
        m_is_constructed   (false),
        m_size             (0),
        m_dims             (Tuple<N,TIndex>::zeros()),
        m_buffer           (NULL),
        m_base             (NULL),
        m_end              (NULL)
        { construct(dims); }

      /** Constructor to create reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param data  A pointer to the data.
        * @param dims  The dimensions of the array.
        */
      ArrayBase(TValue* data, Tuple<N,TIndex> dims)
        :
        m_is_constructed   (true),
        m_size             (long_product(dims)),
        m_dims             (dims),
        m_buffer           (NULL),
        m_base             (data),
        m_end              (data + long_product(dims))
        {}

      /** Constructor to create reference to existing data in Array object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Array remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Array object.
        */
      ArrayBase(const ArrayBase<N,TValue,TIndex>& source)
        :
        m_is_constructed   (true),
        m_size             (source.size()),
        m_dims             (source.dims()),
        m_buffer           (NULL),
        m_base             (const_cast<TValue*>(source.ptr())),
        // There doesn't seem to be a good way to do this without
        // casting away const on source.ptr().
        m_end              (source.end())
        {}

      ~ArrayBase()
        { this->destruct(); }

      /** Allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        */
      void construct(Tuple<N,TIndex> dims)
        {
        if (this->m_is_constructed)
          { throw_bonelab_exc("Array is already constructed."); }
        this->m_is_constructed = true;
        this->m_size = long_product(dims);
        this->m_dims = dims;
        try 
          { this->m_buffer = new TValue[this->m_size]; }
        catch (...)
          { throw_bonelab_exc("Unable to allocate memory."); }
        // In OS's that use lazy allocation, calling memset may ensure that
        // memory is contiguous in real address space, which could be advantageous.
        memset (this->m_buffer, 0, this->m_size*sizeof(TValue));
        this->m_base = this->m_buffer;
        this->m_end = this->m_base + this->m_size;
        }

      /** Create a reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param data  A pointer to the data.
        * @param dims  The dimensions of the array.
        */
      void construct_reference(const TValue* data, Tuple<N,TIndex> dims)
        {
        if (this->m_is_constructed)
          { throw_bonelab_exc("Array is already constructed."); }
        this->m_is_constructed = true;
        this->m_size = long_product(dims);
        this->m_dims = dims;
        this->m_buffer = NULL;
        this->m_base = const_cast<TValue*>(data);
        this->m_end = this->m_base + this->m_size;
        }

      /** Create a reference to to existing data in Array object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Array remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Array object.
        */
      void construct_reference(const ArrayBase<N,TValue,TIndex>& source)
        { this->construct_reference (source.ptr(), source.dims()); }

      void destruct()
        {
        if (this->m_buffer)
          { delete[] this->m_buffer; }
        this->m_is_constructed = false;
        this->m_size = 0;
        this->m_dims = Tuple<N,TIndex>::zeros();
        this->m_buffer = NULL;
        this->m_base = NULL;
        this->m_end = NULL;
        }

      /** Returns true if the Array has been constructed. */
      inline bool is_constructed() const
        { return this->m_is_constructed; }

      /** Returns the flattened (1D) size of the array. */
      inline size_t size() const
        { return this->m_size; }

      /** Returns the dimensions of the Array. */
      inline Tuple<N,TIndex> dims() const
        { return this->m_dims; }

      /** Returns a pointer to the Array data. */
      inline const TValue* ptr() const
        {
#ifdef RANGE_CHECKING
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
#endif
        return this->m_base;
        }
      inline TValue* ptr()
        { return const_cast<TValue*>(static_cast<const ArrayBase*>(this)->ptr()); }

      /** A utility function that performs a run-time check of a pointer to
        * ensure that it points to an entry in the Array data.  Note
        * that high-performance code often must use raw pointers; this provides
        * a mechanism to do some verification and debugging of such code.
        * This check is performed only if compiled with RANGE_CHECKING
        * defined; naturally there is a substantial performance penalty.
        */
      inline const TValue* verify_ptr(const TValue* p) const
        {
#ifdef RANGE_CHECKING
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
        if ((p < this->m_base) || (p >= this->m_end))
          { throw_bonelab_exc("Array index out of bounds."); }
        if (((p - this->m_base) % sizeof(T)) != 0)
          { throw_bonelab_exc("Array pointer has incorrect offset."); }
#endif
        return p;
        }
      inline TValue* verify_ptr(TValue* p) const
        { return const_cast<TValue*>(this->check_ptr(static_cast<const TValue*>(p))); }

      /** Pointer to the last element plus one of the array data.  This
        * may be used in loops various STL algorithms that require an end
        * value.
        */
      inline const TValue* end() const
        {
#ifdef RANGE_CHECKING
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
#endif
        return this->m_end;
        }

      /** Converts a tuple index to the flattened 1D equivalent index. */
      inline size_t flat_index(Tuple<N,TIndex> indices) const
        {
        size_t index = indices[0];
        for (int i=1; i<N; i++)
          {
          index = index*static_cast<size_t>(this->m_dims[i]) + static_cast<size_t>(indices[i]);
          }
        return index;
        }

      /** Flat (1D) indexing of the array data. */
      inline const TValue& operator[](size_t i) const
        {
#ifdef RANGE_CHECKING
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
        if (i >= this->m_size)
          { throw_bonelab_exc("Array index out of bounds."); }
#endif
        return this->m_base[i];
        }
      inline TValue& operator[](size_t i)
        {
        return const_cast<TValue&>(static_cast<const ArrayBase*>(this)->operator[](i));
        }

      /** Indexing of the Array data using N-dimensional tuples. */
      inline const TValue& operator()(Tuple<N,TIndex> indices) const
        { return ArrayBase::operator[](this->flat_index(indices)); }
      inline TValue& operator()(Tuple<N,TIndex> indices)
        {
        return const_cast<TValue&>(static_cast<const ArrayBase*>(this)->operator()(indices));
        }

      /** Set all Array data to zero.
        * Note that this is done on constructing the data (for performance reasons),
        * so calling this immediately after is redundant.
        */
      inline void zero()
        {
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
        memset (this->m_base, 0, sizeof(TValue)*this->m_size);
        }

      /** Copy data from an existing Array.
        * This Array must either have existing dimensions the same as the
        * source array, or it can be unconstructed, in which case it will
        * be constructed with the same dimensions.
        */
      inline void copy(const ArrayBase& rhs)
        {
        if (!this->m_is_constructed)
          { this->construct(rhs.dims()); }
        else if (this->m_dims != rhs.dims())
          { throw_bonelab_exc("cannot copy different sized Arrays."); }
        memcpy (this->m_base, rhs.ptr(), this->m_size*sizeof(TValue));
        }

      /** Copy data from memory specified by a pointer.
        * This Array must be pre-constructed with the desired dimensions.
        * The entire array size will be copied.
        */
      inline void copy(const TValue * rhs)
        {
        if (!this->m_is_constructed)
          { throw_bonelab_exc("Array is not constructed."); }
        memcpy(this->m_base, rhs, this->m_size*sizeof(TValue));
        }

    }; // class ArrayBase

  // ---------------------------------------------------------------------

 /**
  * An efficient class for storing multi-dimensional data.
  *
  * This class is intended as a more convenient, but equally fast, alternative
  * to traditional C arrays:
  * @code
  *   float x = new float[100];
  * @endcode
  *
  * Arrays can be dynamically allocated on the heap. Allocation can be
  * postponed. Arrays cannot be resized.
  *
  * Arrays can be indexed with multi-dimensional indices.
  *
  * Arrays are indexed with the fastest-changing index last, according to memory
  * layout.  Thus if you have a 3D array where x is the fastest-changing
  * index according to memory layout, it would be indexed as A[z,y,x].
  * This implies the following equivalence: A[i,j] = A[i][j].
  * This indexing is consistent with python and numpy.
  *
  * Arrays can be references to existing memory or Arrays.
  *
  * When compiled with RANGE_CHECKING defined, bounds and other checking is
  * performed.
  * When compiled without RANGE_CHECKING defined, no checking is performed, and there
  * is no additional overhead in indexing or dereferencing.
  *
  * Note that there is no particular advantage to using a type smaller than
  * size_t for TIndex for Array itself (the storage difference is just a few
  * bytes in total, and the indexing speed is the same).  However, other
  * classes may want to store large numbers of indices and hence prefer to
  * use a smaller type for the indices.  It is then convenient to be able to
  * define an Array class that can be indexed with this smaller type so
  * no conversions need be performed.
  *
  * Note that on allocation Array memory is zeroed.  There is a performance-related
  * reason for this: In OS's that use lazy allocation, touching every allocated
  * address at allocation time may ensure that memory is contiguous in real 
  * address space, which does in fact sometimes result subsequently in better
  * performance for accessing the array.
  */
  template <int N, typename TValue, typename TIndex=size_t>
  class Array : public ArrayBase<N,TValue, TIndex>
    {
    public:

      /** Empty constructor.
        * You must subsequently call construct or construct_reference explicitly.
        */
      Array() : ArrayBase<N,TValue,TIndex>() {}

      /** Constructor to allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        */
      Array(Tuple<N,TIndex> dims)  : ArrayBase<N,TValue,TIndex>(dims) {}

      /** Constructor to create reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param data  A pointer to the data.
        * @param dims  The dimensions of the array.
        */
      Array(TValue* data, Tuple<N,TIndex> dims) : ArrayBase<N,TValue,TIndex>(data, dims) {}

      /** Constructor to create reference to existing data in Array object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Array remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Array object.
        */
      Array(const ArrayBase<N,TValue,TIndex>& source) : ArrayBase<N,TValue,TIndex>(source) {}

      // Need this??
      ~Array() { this->destruct(); }

    };

  // ---------------------------------------------------------------------

  template <typename TValue, typename TIndex>
  class Array<1,TValue,TIndex> : public ArrayBase<1,TValue,TIndex>
    {
    public:

      Array() : ArrayBase<1,TValue,TIndex>() {}

      Array(Tuple<1,TIndex> dims)  : ArrayBase<1,TValue,TIndex>(dims) {}
      Array(TIndex size)  : ArrayBase<1,TValue,TIndex>(Tuple<1,TIndex>(size)) {}

      Array(TValue* data, Tuple<1,TIndex> dims) : ArrayBase<1,TValue,TIndex>(data, dims) {}
      Array(TValue* data, TIndex size) : ArrayBase<1,TValue,TIndex>(data, Tuple<1,TIndex>(size)) {}

      Array(const ArrayBase<1,TValue,TIndex>& source) : ArrayBase<1,TValue,TIndex>(source) {}

      // Need this??
      ~Array() { this->destruct(); }

      inline size_t flat_index(Tuple<1,TIndex> indices) const
        {
        // Implied static cast from TIndex to size_t
        return indices[0];
        }

      inline const TValue& operator()(TIndex i) const
        { return ArrayBase<1,TValue,TIndex>::operator[](static_cast<size_t>(i)); }
      
      inline TValue& operator()(TIndex i)
        {
        return const_cast<TValue&>(static_cast<const Array<1,TValue,TIndex>*>(this)->operator()(i));
        }

      inline const TValue& operator()(Tuple<1,TIndex> indices) const
        { return ArrayBase<1,TValue,TIndex>::operator[](this->flat_index(indices)); }

      inline TValue& operator()(Tuple<1,TIndex> indices)
        {
        return const_cast<TValue&>(static_cast<const ArrayBase<1,TValue,TIndex>*>(this)->operator()(indices));
        }

    };

  // ---------------------------------------------------------------------

  template <typename TValue, typename TIndex>
  class Array<2,TValue,TIndex> : public ArrayBase<2,TValue,TIndex>
    {
    public:

      Array() : ArrayBase<2,TValue,TIndex>() {}

      Array(Tuple<2,TIndex> dims)  : ArrayBase<2,TValue,TIndex>(dims) {}
      Array(TIndex dim0, TIndex dim1)
          : ArrayBase<2,TValue,TIndex>(Tuple<2,TIndex>(dim0, dim1)) {}

      Array(TValue* data, Tuple<2,TIndex> dims) : ArrayBase<2,TValue,TIndex>(data, dims) {}
      Array(TValue* data, TIndex dim0, TIndex dim1)
          : ArrayBase<2,TValue,TIndex>(data, Tuple<2,TIndex>(dim0, dim1)) {}

      Array(const ArrayBase<2,TValue,TIndex>& source) : ArrayBase<2,TValue,TIndex>(source) {}

      // Need this??
      ~Array() { this->destruct(); }

      inline size_t flat_index(TIndex i, TIndex j) const
        {
        return static_cast<size_t>(i)*static_cast<size_t>(this->m_dims[1])
            + static_cast<size_t>(j);
        }

      inline size_t flat_index(Tuple<2,TIndex> indices) const
        {
        return static_cast<size_t>(indices[0])*static_cast<size_t>(this->m_dims[1])
            + static_cast<size_t>(indices[1]);
        }

      inline const TValue& operator()(TIndex i, TIndex j) const
        { return ArrayBase<2,TValue,TIndex>::operator[](this->flat_index(i,j)); }

      inline TValue& operator()(TIndex i, TIndex j)
        {
        return const_cast<TValue&>(static_cast<const Array<2,TValue,TIndex>*>(this)->operator()(i,j));
        }

      inline const TValue& operator()(Tuple<2,TIndex> indices) const
        { return ArrayBase<2,TValue,TIndex>::operator[](this->flat_index(indices)); }

      inline TValue& operator()(Tuple<2,TIndex> indices)
        {
        return const_cast<TValue&>(static_cast<const ArrayBase<2,TValue,TIndex>*>(this)->operator()(indices));
        }

    };

  // ---------------------------------------------------------------------

  template <typename TValue, typename TIndex>
  class Array<3,TValue,TIndex> : public ArrayBase<3,TValue,TIndex>
    {
    public:

      Array() : ArrayBase<3,TValue,TIndex>() {}

      Array(Tuple<3,TIndex> dims)  : ArrayBase<3,TValue,TIndex>(dims) {}
      Array(TIndex dim0, TIndex dim1, TIndex dim2)
          : ArrayBase<3,TValue,TIndex>(Tuple<3,TIndex>(dim0, dim1, dim2)) {}

      Array(TValue* data, Tuple<3,TIndex> dims) : ArrayBase<3,TValue,TIndex>(data, dims) {}
      Array(TValue* data, TIndex dim0, TIndex dim1, TIndex dim2)
          : ArrayBase<3,TValue,TIndex>(data, Tuple<3,TIndex>(dim0, dim1, dim2)) {}

      Array(const ArrayBase<3,TValue,TIndex>& source) : ArrayBase<3,TValue,TIndex>(source) {}

      // Need this??
      ~Array() { this->destruct(); }

      inline size_t flat_index(TIndex i, TIndex j, TIndex k) const
        {
        return (static_cast<size_t>(i)*static_cast<size_t>(this->m_dims[1])
                + static_cast<size_t>(j))*static_cast<size_t>(this->m_dims[2])
               + static_cast<size_t>(k);
        }

      inline size_t flat_index(Tuple<3,TIndex> indices) const
        {
        return (static_cast<size_t>(indices[0])*static_cast<size_t>(this->m_dims[1])
                + static_cast<size_t>(indices[1]))*static_cast<size_t>(this->m_dims[2])
               + static_cast<size_t>(indices[2]);
        }

      inline const TValue& operator()(TIndex i, TIndex j, TIndex k) const
        { return ArrayBase<3,TValue,TIndex>::operator[](this->flat_index(i,j,k)); }

      inline TValue& operator()(TIndex i, TIndex j, TIndex k)
        {
        return const_cast<TValue&>(static_cast<const Array<3,TValue,TIndex>*>(this)->operator()(i,j,k));
        }

      inline const TValue& operator()(Tuple<3,TIndex> indices) const
        { return ArrayBase<3,TValue,TIndex>::operator[](this->flat_index(indices)); }

      inline TValue& operator()(Tuple<3,TIndex> indices)
        {
        return const_cast<TValue&>(static_cast<const ArrayBase<3,TValue,TIndex>*>(this)->operator()(indices));
        }

    };

  } // namespace bonelab

#endif
