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

#ifndef BONELAB_IMAGE_HPP_INCLUDED
#define BONELAB_IMAGE_HPP_INCLUDED

#include "Array.hpp"
#include "Tuple.hpp"

namespace bonelab
  {

  /**
   * An Image is a subclass Array that additional contains spatial information,
   * such as pixel spacing.
   *
   * As for Arrays, Images  are indexed with the fastest-changing index last,
   * according to memory layout.  Thus if you have a 3D array where x is the
   * fastest-changing index according to memory layout, it would be indexed as
   * A[z,y,x].
   *
   * Origin is defined as the center of the zero-indexed pixel.  (And not for
   * example as the lower left corner - in 2D - of that pixel, which is
   * another common convention).  Here "lower left" means index (0,0); which
   * is not necessary either lower or left, depending on your coordinate
   * system.
   *
   * Extents are indexed [i][j], where index i is the dimension, and j is 0
   * for mimimum of extent, and is 1 for maximum extent.
   *
   * We haven't put any particular thought into what ought to happen if any
   * spacings are negative, so behaviour in that case is undefined, and
   * may change in future.
   *
   * When compiled with DEBUG defined, bounds and other checking is
   * performed.
   * When compiled without DEBUG defined, no checking is performed, and there
   * is no additional overhead in indexing or dereferencing.
   */
  template <int N, typename TValue, typename TIndex=size_t, typename TSpace=double>
  class Image : public Array<N,TValue,TIndex>
    {

    public:
      
      typedef TSpace space_type;
      typedef Tuple<N, Tuple<2,TSpace> > TExtents;

    protected:
      
      Tuple<N,TSpace> m_spacing;
      Tuple<N,TSpace> m_origin;
      TExtents m_interior_extents;
      TExtents m_exterior_extents;

    public:

      /** Empty constructor.
        * You must subsequently call construct or construct_reference explicitly.
        */
      Image ()
        :
        Array<N,TValue,TIndex>(),
        m_spacing(Tuple<N,TSpace>::zeros()),
        m_origin(Tuple<N,TSpace>::zeros())
        { this->calculate_extents(); }

      /** Constructor to allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      Image
        (
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        :
        Array<N,TValue,TIndex>(dims),
        m_spacing(spacing),
        m_origin(origin)
        { this->calculate_extents(); }

      /** Constructor to create reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param data  A pointer to the data.
        * @param dims  The dimensions of the array.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      Image
        (
        const TValue* data,
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        :
        Array<N,TValue,TIndex>(const_cast<TValue*>(data), dims),
        m_spacing(spacing),
        m_origin(origin)
        { this->calculate_extents(); }

      /** Constructor to create reference to existing data in Array object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Array remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Array object.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      Image
        (
        const Array<N,TValue,TIndex>& array,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        :
        Array<N,TValue,TIndex>(array),
        m_spacing(spacing),
        m_origin(origin)
        { this->calculate_extents(); }

      /** Constructor to create reference to existing data in Image object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Image remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Image object.
        */
      Image (const Image& source)
        :
        Array<N,TValue,TIndex>(source),
        m_spacing(source.spacing()),
        m_origin(source.origin())
        { this->calculate_extents(); }

      /** Allocate space.
        *
        * @param dims  The dimensions of the array to allocate.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      void construct
        (
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        Array<N,TValue,TIndex>::construct(dims);
        this->m_spacing = spacing;
        this->m_origin = origin;
        this->calculate_extents();
        }

      /** Create a reference to existing data defined by a pointer.
        * The referenced data will never be freed by this object.  You
        * must ensure that the referenced memory remains valid, and perform
        * appropriate clean up outside this object.
        *
        * @param data  A pointer to the data.
        * @param dims  The dimensions of the array.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      void construct_reference
        (
        const TValue* data,
        Tuple<N,TIndex> dims,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        Array<N,TValue,TIndex>::construct_reference(data, dims);
        this->m_spacing = spacing;
        this->m_origin = origin;
        this->calculate_extents();
        }

      /** Create a reference to existing data in Array object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Array remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Array object.
        * @param spacing  The pixel spacing.
        * @param origin  The origin.
        */
      void construct_reference
        (
        const Array<N,TValue,TIndex>& array,
        Tuple<N,TSpace> spacing,
        Tuple<N,TSpace> origin
        )
        {
        Array<N,TValue,TIndex>::construct_reference(array);
        this->m_spacing = spacing;
        this->m_origin = origin;
        this->calculate_extents();
        }

      /** Create reference to existing data in Image object.
        * No automatic reference counting is performed.  You
        * must ensure that the referenced Image remains in existence for as
        * long as you want to use this reference.
        *
        * @param source  An existing Image object.
        */
      void construct_reference (const Image& source)
        {
        this->construct_reference(
            source.ptr(),
            source.dims(),
            source.spacing(),
            source.origin());
        }

      void destruct ()
        {
        Array<N,TValue,TIndex>::destruct();
        this->m_spacing = Tuple<N,TSpace>::zeros();
        this->m_origin = Tuple<N,TSpace>::zeros();
        this->calculate_extents();
        }

      /** Calculate the extents.
       *
       * It is not normally required to call this unless you change the spacing
       * or the origin, as it is called by the the construction methods
       */
      inline void calculate_extents()
        {
        for (int i=0; i<N; i++)
          {
          this->m_interior_extents[i][0] = this->m_origin[i];
          this->m_interior_extents[i][1] = this->m_origin[i] + (this->m_dims[i] - 1)*this->m_spacing[i];
          this->m_exterior_extents[i][0] = this->m_interior_extents[i][0] - 0.5*this->m_spacing[i];
          this->m_exterior_extents[i][1] = this->m_interior_extents[i][1] + 0.5*this->m_spacing[i];
          }
        }

      /** Copy data from an existing Image.
        * This Image must either have existing dimensions the same as the
        * source array, or it can be unconstructed, in which case it will
        * be constructed with the same dimensions.
        * Spacing and origin will be assumed from the source; be aware that
        * previous values will be discarded.
        */
      inline void copy(const Image& rhs)
        {
        if (!this->m_is_constructed)
          { this->construct(rhs.dims(), rhs.spacing(), rhs.origin()); }
        else
          {
          if (this->m_dims != rhs.dims())
            { throw_bonelab_exc("cannot copy different sized Images"); }
          this->m_spacing = rhs.spacing();
          this->m_origin = rhs.origin();
          this->calculate_extents();
          }
        Array<N,TValue,TIndex>::copy(rhs);
        }
      
      /** Returns the pixel spacing. */
      inline Tuple<N,TSpace> spacing() const
        { return this->m_spacing; }
      
      /** Returns the origin.
        * The origin defines the center (not the corner) of the zero-indexed
        * pixel.
        */
      inline Tuple<N,TSpace> origin() const
        { return this->m_origin; }

      /** Returns the interior extents.
        * The interior extents are from the origin (i.e. in 2D the center of
        * the lower-left pixel, actually pixel (0,0)) to the center of the
        * upper-right pixel.
        */
      inline TExtents interior_extents() const
        { return this->m_interior_extents; }

      /** Returns the exterior extents.
        * The exterior extents are from the outer corner of the zero-indexed
        * pixel (i.e. in 2D the lower left corner of the lower-left pixel,
        * actually pixel (0,0)) to the outer corner of the upper-right pixel.
        */
      inline TExtents exterior_extents() const
        { return this->m_exterior_extents; }

    }; // class Image

  } // namespace bonelab

#endif
