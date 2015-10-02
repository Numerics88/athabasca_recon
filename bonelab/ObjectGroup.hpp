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

#ifndef BONELAB_ObjectGroup_hpp_INCLUDED
#define BONELAB_ObjectGroup_hpp_INCLUDED

#include "exception.hpp"
#include <boost/noncopyable.hpp>
#include <vector>
#include <cstdlib>

namespace bonelab
  {

  /** A collection of objects (no copy constructor required).
    *
    * This container is typically used for resource-managing objects, which
    * cannot be copied and therefore do not have copy constructors.  Note
    * that STL containers require copy constructors.  The objects
    * stored in ObjectGroup must however have a default constructor (one
    * that takes no arguments).
    */
  template <typename TObject>
  class ObjectGroup : private boost::noncopyable
    {

    protected:

      std::vector<TObject*> m_Objects;

    public:

      /** Default constructor.  Creates an empty ObjectGroup. */
      ObjectGroup() {}
      
      /** Constructor that sets size of ObjectGroup to n, creating n objects. */
      ObjectGroup(size_t n)
        {
        this->construct<TObject>(n);
        }

      ~ObjectGroup()
        {
        this->destroy();
        }

      /** Sets the size of ObjectGroup to n, creating n objects.
        * Throws an exception if existing size is non-zero.
        */
      template <class TDerivedObject>
      void construct(size_t n)
        {
        bonelab_assert(this->m_Objects.size() == 0);
        for (size_t i=0; i<n; ++i)
          { this->m_Objects.push_back(new TDerivedObject()); }
        }

      /** Sets the size of ObjectGroup to 0, calling the destructors of
        * each object currently in the ObjectGroup.
        */
      void destroy()
        {
        while (!this->m_Objects.empty())
          {
          delete this->m_Objects.back();
          this->m_Objects.pop_back();
          }
        }

      /** Returns the size (number of objects) of ObjectGroup. */
      size_t size()
        { return this->m_Objects.size(); }

      /** Returns a reference to object i. */
      TObject& operator[](size_t n)
        {
#ifdef RANGE_CHECKING
        bonelab_assert(n < m_Objects.size());
#endif
        return *this->m_Objects[n];
        }
      const TObject& operator[](size_t n) const
        {
#ifdef RANGE_CHECKING
        bonelab_assert(n < m_Objects.size());
#endif
        return *this->m_Objects[n];
        }

    };

  } // namespace bonelab

#endif
