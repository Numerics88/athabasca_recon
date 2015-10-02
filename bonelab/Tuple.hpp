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

#ifndef BONELAB_VECTOR_HPP_INCLUDED
#define BONELAB_VECTOR_HPP_INCLUDED

#include "exception.hpp"
#include <vector>
#include <ostream>
#include <iostream>
#include <cmath>

namespace bonelab
  {

  /** A base class for Tuple.
    *
    * The purpose of this base class is to provide a bunch of common methods
    * that can be inherited in partial specializations of Tuple.  Please
    * refer to Tuple for more details.
    */
  template <int N, typename T>
  class TupleBase
    {

    protected:

      T m_Data[N];
      
    public:

      typedef T value_type;

      /** Constructor.
        * No initialization of the data; assume random memory garbage.
        */
      TupleBase() {}

      /** Constructor that copies data from a pointer. */
      TupleBase(const T* p)
        { for (int i=0; i<N; i++) {this->m_Data[i] = p[i];}}

      /** Constructor that copies data from a vector. */
      TupleBase(const std::vector<T>& v)
        {
        bonelab_assert(v.size() == N);
        for (int i=0; i<N; i++) {this->m_Data[i] = v[i];}
        }

      /** Returns element i. */
      inline const T& operator[](size_t i) const
        {
#ifdef RANGE_CHECKING
        if (i >= N)
          { throw_bonelab_exc("Tuple index out of bounds"); }
#endif
        return this->m_Data[i];
        }
      inline T& operator[](size_t i)
        {
#ifdef RANGE_CHECKING
        if (i >= N)
          { throw_bonelab_exc("Tuple index out of bounds"); }
#endif
        return this->m_Data[i];
        }

      /** Returns a pointer to the data. */
      inline const T* ptr() const
        { return this->m_Data; }
      inline T* ptr()
        { return this->m_Data; }

      /** Pointer to the last element plus one of the data.  This
        * may be used in loops various STL algorithms that require an end
        * value.
        */
      inline const T* end() const
        { return this->m_Data + N;  }
      
      /** Assignment operator : from a Tuple. */  
      inline TupleBase operator=(const TupleBase& rhs)
        {
        for (int i=0; i<N; i++) {this->m_Data[i] = rhs[i];}
        return *this;
        }

      /** Assignment operator : from a pointer. */  
      inline TupleBase operator=(const T* const p)
        {
        for (int i=0; i<N; i++) {this->m_Data[i] = p[i];}
        return *this;
        }

      /** Assignment operator : from a scalar.
        * Note that every element of the tuple will assume the scalar value.
        */  
      inline TupleBase operator=(T x)
        {
        for (int i=0; i<N; i++) {this->m_Data[i] = x;}
        return *this;
        }

      /** Equality operator : to a Tuple */
      inline bool operator==(const TupleBase& rhs) const
        {
        for (int i=0; i<N; i++)
          {if (this->m_Data[i] != rhs[i]) {return false;}}
        return true;
        }

      /** Inequality operator : to a Tuple */
      inline bool operator!=(const TupleBase& rhs) const
        {
        for (int i=0; i<N; i++)
          {if (this->m_Data[i] != rhs[i]) {return true;}}
        return false;
        }

      /** Negation operator */
      inline TupleBase<N,T> operator-() const
        {
        TupleBase<N,T> x;
        for (int i=0; i<N; i++) {x[i] = -this->m_Data[i];}
        return x;
        }

    }; // class TupleBase <int N, typename T>


  /** A class for fixed-length tuples (vectors) of simple types.
    *
    * Tuples are intended to be a replacement for C static arrays, e.g.
    *    float x[3]; 
    *
    * Tuples have no additional storage requirements compared with static arrays
    * and are equally fast to access.  Unlike C static arrays, the length
    * can be checked at compile time in places it is used.
    *
    * Additionally, for debugging purposes, range checking is performed if
    * RANGE_CHECKING is defined at compile time.
    *
    * It can be faster to pass small (e.g. size 2, 3 or 4) Tuples on the stack 
    * by value than it is to pass them by reference.
    */
  template <int N, typename T>
  class Tuple : public TupleBase<N,T>
    {
    public:

      /** Constructor.
        * No initialization of the data; assume random memory garbage.
        */
      Tuple () {}
      
      /** Constructor that copies data from a Tuple. */
      Tuple(const TupleBase<N,T>& t) : TupleBase<N,T>(t) {}

      /** Constructor that copies data from a pointer. */
      Tuple(const T* p) : TupleBase<N,T>(p) {}

      /** Constructor that copies data from a vector. */
      Tuple(const std::vector<T>& v) : TupleBase<N,T>(v) {}

      /** Returns a Tuple filled with zeros. */
      inline static Tuple<N,T> zeros()
        {
        Tuple<N,T> t;
        for (int i=0; i<N; i++) {t[i] = 0;}
        return t;
        }

      /** Returns a Tuple filled with ones. */
      inline static Tuple<N,T> ones()
        {
        Tuple<N,T> t;
        for (int i=0; i<N; i++) {t[i] = 1;}
        return t;
        }

    }; // class Tuple <int N, typename T>
  
  template <typename T>
  class Tuple<1,T> : public TupleBase<1,T>
    {
    public:
  
      Tuple() {}
  
      Tuple(const TupleBase<1,T>& t) : TupleBase<1,T>(t) {}

      Tuple(const T* p) : TupleBase<1,T>(p) {}
  
      Tuple(const std::vector<T>& v) : TupleBase<1,T>(v) {}

      Tuple(T x0)
        {
        this->m_Data[0] = x0;
        }

      inline static Tuple<1,T> zeros()
        {
        Tuple<1,T> t;
        t[0] = 0;
        return t;
        }
    
      inline static Tuple<1,T> ones()
        {
        Tuple<1,T> t;
        t[0] = 1;
        return t;
        }

    }; // class Tuple <1, typename T>

  template <typename T>
  class Tuple<2,T> : public TupleBase<2,T>
    {
    public:
  
      Tuple() {}
  
      Tuple(const TupleBase<2,T>& t) : TupleBase<2,T>(t) {}

      Tuple(const T* p) : TupleBase<2,T>(p) {}
  
      Tuple(const std::vector<T>& v) : TupleBase<2,T>(v) {}
  
      Tuple(T x0, T x1)
        {
        this->m_Data[0] = x0;
        this->m_Data[1] = x1;
        }

      inline static Tuple<2,T> zeros()
        {
        Tuple<2,T> t;
        t[0] = 0;
        t[1] = 0;
        return t;
        }

      inline static Tuple<2,T> ones()
        {
        Tuple<2,T> t;
        t[0] = 1;
        t[1] = 1;
        return t;
        }

    }; // class Tuple <2, typename T>

  template <typename T>
  class Tuple<3,T> : public TupleBase<3,T>
    {
    public:
  
      Tuple() {}
  
      Tuple(const TupleBase<3,T>& t) : TupleBase<3,T>(t) {}

      Tuple(const T* p) : TupleBase<3,T>(p) {}
  
      Tuple(T x0, T x1, T x2)
        {
        this->m_Data[0] = x0;
        this->m_Data[1] = x1;
        this->m_Data[2] = x2;
        }

      Tuple(const std::vector<T>& v) : TupleBase<3,T>(v) {}

      inline static Tuple<3,T> zeros()
        {
        Tuple<3,T> t;
        t[0] = 0;
        t[1] = 0;
        t[2] = 0;
        return t;
        }

      inline static Tuple<3,T> ones()
        {
        Tuple<3,T> t;
        t[0] = 1;
        t[1] = 1;
        t[2] = 1;
        return t;
        }

    }; // class Tuple <3, typename T>

  template <typename T>
  class Tuple<4,T> : public TupleBase<4,T>
    {
    public:
  
      Tuple() {}
  
      Tuple(const TupleBase<4,T>& t) :TupleBase<4,T>(t) {}

      Tuple(const T* p) : TupleBase<4,T>(p) {}
    
      Tuple(const std::vector<T>& v) : TupleBase<4,T>(v) {}

      Tuple(T x0, T x1, T x2, T x3)
        {
        this->m_Data[0] = x0;
        this->m_Data[1] = x1;
        this->m_Data[2] = x2;
        this->m_Data[3] = x3;
        }

      inline static Tuple<4,T> zeros()
        {
        Tuple<4,T> t;
        t[0] = 0;
        t[1] = 0;
        t[2] = 0;
        t[3] = 0;
        return t;
        }

      inline static Tuple<4,T> ones()
        {
        Tuple<4,T> t;
        t[0] = 1;
        t[1] = 1;
        t[2] = 1;
        t[3] = 1;
        return t;
        }

    }; // class Tuple <4, typename T>

  /** Returns the product of all elements of Tuple. */
  template <int N, typename T> inline T product(Tuple<N,T> x)
    {
    T p = x[0];
    for (int i=1; i<N; i++) {p *= x[i];}
    return p;
    }

  /** Returns the product of all elements of Tuple as size_t.
    * The return value is size_t in order to avoid overflow
    * (e.g. as could happen if Tuple elements were of type int).
    */
  template <int N, typename T> inline size_t long_product(Tuple<N,T> x)
    {
    size_t p = x[0];
    for (int i=1; i<N; i++) {p *= x[i];}
    return p;
    }

  /** Returns the sum of all elements of Tuple. */
  template <int N, typename T> inline T sum(Tuple<N,T> x)
    {
    T s = x[0];
    for (int i=1; i<N; i++) {s += x[i];}
    return s;
    }

  /** Stream output operator.
    * Tuples are output in brackets, with individual elements separated
    * by commas.  For example, Tuple<3,int> might be output like this:
    * (4,24,1) .
    */
  template <int N, typename T>
  std::ostream& operator<<(std::ostream& s, const Tuple<N,T>& t)
    {
    s << "(" << t[0];
    for (int i=1; i<N; i++) {s << "," << t[i];}
    s << ")";
    return s;
    }

  /** Stream input operator.
    * Converts a text representation of a Tuple to a Tuple.
    * The text representation may or may not include enclosing brackets,
    * which may be either round or square, but must match.
    * White space is ignored.
    * Elements must be separated by commas.
    * The input stream operator of the element type is used to parse the
    * elements.
    */
  template <int N, typename T>
  std::istream& operator>>(std::istream& s, Tuple<N,T>& t)
    {
    char c = s.get();
    if (s.fail() || s.bad() || s.eof())
      {
      return s;
      s.setstate(std::ios::failbit);
      }
    // Skip white space 
    while (c == ' ')
      {
      c = s.get();
      if (s.fail() || s.bad() || s.eof())
        {
        return s;
        s.setstate(std::ios::failbit);
        }
      }
    // optional bracket
    char match_bracket = 0;
    if (c == '(') { match_bracket = ')'; }
    if (c == '[') { match_bracket = ']'; }
    if (match_bracket)
      {
      // Skip white space after bracket
      c = s.get();
      if (s.fail() || s.bad() || s.eof())
        {
        return s;
        s.setstate(std::ios::failbit);
        }
      while (c == ' ')
        {
        c = s.get();
        if (s.fail() || s.bad() || s.eof())
          {
          return s;
          s.setstate(std::ios::failbit);
          }
        }
      }

    // Loop over dimensions
    for (int i=0; i<N; i++)
      {
      // We have picked off one too many characters, so put it back
      s.unget();
      s >> t[i];
      if (s.fail() || s.bad())
        {return s;}
      if (s.eof())
        {
        if (match_bracket || i != N-1)
          { s.setstate(std::ios::failbit); }
        return s;
        }
      // Skip white space
      c = s.get();
      if (s.fail() || s.bad())
        {return s;}
      if (s.eof())
        {
        if (match_bracket || i != N-1)
          { s.setstate(std::ios::failbit); }
        return s;
        }
      while (c == ' ')
        {
        c = s.get();
        if (s.fail() || s.bad())
          {return s;}
        if (s.eof())
          {
          if (match_bracket || i != N-1)
            { s.setstate(std::ios::failbit); }
          return s;
          }
        }
      // Optional comma
      if (c == ',' && i < N-1)
        {
        // Move past the comma
        c = s.get();
        if (s.fail() || s.bad())
          {return s;}
        if (s.eof())
          {
          if (match_bracket || i != N-1)
            { s.setstate(std::ios::failbit); }
          return s;
          }
        }
      // Skip white space again
      while (c == ' ')
        {
        c = s.get();
        if (s.fail() || s.bad())
          {return s;}
        if (s.eof())
          {
          if (match_bracket || i != N-1)
            { s.setstate(std::ios::failbit); }
          return s;
          }
        }
      // Note: at end of for loop have read next character already.
      }
    
    // If we don't need to match the bracket, will have already returned.
    bonelab_assert(match_bracket);
    
    // Skip white space
    while (c == ' ')
      {
      c = s.get();
      if (s.fail() || s.bad() || s.eof())
        {
        // No matching bracket found.
        s.setstate(std::ios::failbit);
        return s;
        }
      }
    if (c != match_bracket)
      {
      s.setstate(std::ios::failbit);
      }
      
    // Note: Correct behaviour seems to be to return with eof set if
    //       we used all the input stream, so read one more char to
    //       ensure this.
    s.peek();
    return s;
    }

  /** Returns the norm of a Tuple.
    * The norm is sqrt(sum_i(x_i^2)) .
    */
  template <int N, typename T> inline T norm(Tuple<N,T> a)
    {
    T x = a[0]*a[0];
    for (int i=1; i<N; i++) {x += a[i]*a[i];}
    return sqrt(x);
    }
  template <typename T> inline T norm(Tuple<1,T> a)
    {
    return sqrt(a[0]*a[0]);
    }
  template <typename T> inline T norm(Tuple<2,T> a)
    {
    return sqrt(a[0]*a[0] + a[1]*a[1]);
    }
  template <typename T> inline T norm(Tuple<3,T> a)
    {
    return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    }

  /** Adds a scalar and a Tuple. */
  template <int N, typename T> inline Tuple<N,T> operator+
  (const Tuple<N,T>& a, T s)
    {
    Tuple<N,T> x;
    for (int i=0; i<N; i++) {x[i] = a[i] + s;}
    return x;
    }
  template <typename T> inline Tuple<1,T> operator+
  (const Tuple<1,T>& a, T s)
    {
    return Tuple<1,T>(a[0]+s);
    }
  template <typename T> inline Tuple<2,T> operator+
  (const Tuple<2,T>& a, T s)
    {
    return Tuple<2,T>(a[0]+s,a[1]+s);
    }
  template <typename T> inline Tuple<3,T> operator+
  (const Tuple<3,T>& a, T s)
    {
    return Tuple<3,T>(a[0]+s,a[1]+s,a[2]+s);
    }

  /** Adds two Tuples. */
  template <int N, typename T> inline Tuple<N,T> operator+
  (const Tuple<N,T>& a, const Tuple<N,T>& b)
    {
    Tuple<N,T> x;
    for (int i=0; i<N; i++) {x[i] = a[i] + b[i];}
    return x;
    }
  template <typename T> inline Tuple<1,T> operator+
  (const Tuple<1,T>& a, const Tuple<1,T>& b)
    {
    return Tuple<1,T>(a[0]+b[0]);
    }
  template <typename T> inline Tuple<2,T> operator+
  (const Tuple<2,T>& a, const Tuple<2,T>& b)
    {
    return Tuple<2,T>(a[0]+b[0],a[1]+b[1]);
    }
  template <typename T> inline Tuple<3,T> operator+
  (const Tuple<3,T>& a, const Tuple<3,T>& b)
    {
    return Tuple<3,T>(a[0]+b[0],a[1]+b[1],a[2]+b[2]);
    }

  /** Subtracts a scalar from a Tuple. */
  template <int N, typename T> inline Tuple<N,T> operator-
  (const Tuple<N,T>& a, T s)
    {
    Tuple<N,T> x;
    for (int i=0; i<N; i++) {x[i] = a[i] - s;}
    return x;
    }
  template <typename T> inline Tuple<1,T> operator-
  (const Tuple<1,T>& a, T s)
    {
    return Tuple<1,T>(a[0]-s);
    }
  template <typename T> inline Tuple<2,T> operator-
  (const Tuple<2,T>& a, T s)
    {
    return Tuple<2,T>(a[0]-s,a[1]-s);
    }
  template <typename T> inline Tuple<3,T> operator-
  (const Tuple<3,T>& a, T s)
    {
    return Tuple<3,T>(a[0]-s,a[1]-s,a[2]-s);
    }

  /** Subtracts two Tuples. */
  template <int N, typename T> inline Tuple<N,T> operator-
  (const Tuple<N,T>& a, const Tuple<N,T>& b)
    {
    Tuple<N,T> x;
    for (int i=0; i<N; i++) {x[i] = a[i] - b[i];}
    return x;
    }
  template <typename T> inline Tuple<1,T> operator-
  (const Tuple<1,T>& a, const Tuple<1,T>& b)
    {
    return Tuple<1,T>(a[0]-b[0]);
    }
  template <typename T> inline Tuple<2,T> operator-
  (const Tuple<2,T>& a, const Tuple<2,T>& b)
    {
    return Tuple<2,T>(a[0]-b[0],a[1]-b[1]);
    }
  template <typename T> inline Tuple<3,T> operator-
  (const Tuple<3,T>& a, const Tuple<3,T>& b)
    {
    return Tuple<3,T>(a[0]-b[0],a[1]-b[1],a[2]-b[2]);
    }

  /** Multiplies a scalar by a Tuple. */
  template <int N, typename T> inline Tuple<N,T> operator*
  (T x, const Tuple<N,T>& a)
    {
    Tuple<N,T> b;
    for (int i=0; i<N; i++) {b[i] = x*a[i];}
    return b;
    }
  template <typename T> inline Tuple<1,T> operator*
  (T x, const Tuple<1,T>& a)
    {
    return Tuple<1,T>(x*a[0]);
    }
  template <typename T> inline Tuple<2,T> operator*
  (T x, const Tuple<2,T>& a)
    {
    return Tuple<2,T>(x*a[0],x*a[1]);
    }
  template <typename T> inline Tuple<3,T> operator*
  (T x, const Tuple<3,T>& a)
    {
    return Tuple<3,T>(x*a[0],x*a[1],x*a[2]);
    }

  /** Multiplies a Tuple by a scalar. */
  template <int N, typename T> inline Tuple<N,T> operator*
  (const Tuple<N,T>& a, T x)
    {
    Tuple<N,T> b;
    for (int i=0; i<N; i++) {b[i] = x*a[i];}
    return b;
    }
  template <typename T> inline Tuple<1,T> operator*
  (const Tuple<1,T>& a, T x)
    {
    return Tuple<1,T>(x*a[0]);
    }
  template <typename T> inline Tuple<2,T> operator*
  (const Tuple<2,T>& a, T x)
    {
    return Tuple<2,T>(x*a[0],x*a[1]);
    }
  template <typename T> inline Tuple<3,T> operator*
  (const Tuple<3,T>& a, T x)
    {
    return Tuple<3,T>(x*a[0],x*a[1],x*a[2]);
    }

  /** Multiplies two Tuples. */
  template <int N, typename T> inline Tuple<N,T> operator*
  (const Tuple<N,T>& a, const Tuple<N,T>& b)
    {
    Tuple<N,T> c;
    for (int i=0; i<N; i++) {c[i] = a[i] * b[i];}
    return c;
    }
  template <typename T> inline Tuple<1,T> operator*
  (const Tuple<1,T>& a, const Tuple<1,T>& b)
    {
    return Tuple<1,T>(a[0]*b[0]);
    }
  template <typename T> inline Tuple<2,T> operator*
  (const Tuple<2,T>& a, const Tuple<2,T>& b)
    {
    return Tuple<2,T>(a[0]*b[0],a[1]*b[1]);
    }
  template <typename T> inline Tuple<3,T> operator*
  (const Tuple<3,T>& a, const Tuple<3,T>& b)
    {
    return Tuple<3,T>(a[0]*b[0],a[1]*b[1],a[2]*b[2]);
    }

  /** Divides one Tuple by another. */
  template <int N, typename T> inline Tuple<N,T> operator/
  (const Tuple<N,T>& a, const Tuple<N,T>& b)
    {
    Tuple<N,T> c;
    for (int i=0; i<N; i++) {c[i] = a[i] / b[i];}
    return c;
    }
  template <typename T> inline Tuple<1,T> operator/
  (const Tuple<1,T>& a, const Tuple<1,T>& b)
    {
    return Tuple<1,T>(a[0]/b[0]);
    }
  template <typename T> inline Tuple<2,T> operator/
  (const Tuple<2,T>& a, const Tuple<2,T>& b)
    {
    return Tuple<2,T>(a[0]/b[0],a[1]/b[1]);
    }
  template <typename T> inline Tuple<3,T> operator/
  (const Tuple<3,T>& a, const Tuple<3,T>& b)
    {
    return Tuple<3,T>(a[0]/b[0],a[1]/b[1],a[2]/b[2]);
    }

  /** Divides a Tuple by a scalar. */
  template <int N, typename T> inline Tuple<N,T> operator/
  (const Tuple<N,T>& a, T b)
    {
    Tuple<N,T> c;
    for (int i=0; i<N; i++) {c[i] = a[i] / b;}
    return c;
    }
  template <typename T> inline Tuple<1,T> operator/
  (const Tuple<1,T>& a, T b)
    {
    return Tuple<1,T>(a[0]/b);
    }
  template <typename T> inline Tuple<2,T> operator/
  (const Tuple<2,T>& a, T b)
    {
    return Tuple<2,T>(a[0]/b,a[1]/b);
    }
  template <typename T> inline Tuple<3,T> operator/
  (const Tuple<3,T>& a, T b)
    {
    return Tuple<3,T>(a[0]/b,a[1]/b,a[2]/b);
    }

  /** Returns the dot product of a Tuple. */
  template <int N, typename T> inline T dot(Tuple<N,T> a, Tuple<N,T> b)
    {
    T x = a[0]*b[0];
    for (int i=1; i<N; i++) {x += a[i]*b[i];}
    return x;
    }
  template <typename T> inline T dot(Tuple<1,T> a, Tuple<1,T> b)
    {
    return a[0]*b[0];
    }
  template <typename T> inline T dot(Tuple<2,T> a, Tuple<2,T> b)
    {
    return a[0]*b[0] + a[1]*b[1];
    }
  template <typename T> inline T dot(Tuple<3,T> a, Tuple<3,T> b)
    {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    }

  /** Returns a Tuple which has the elements in the reverse order. */
  template <int N, typename T> inline Tuple<N,T> reverse(Tuple<N,T> a)
    {
    Tuple<N,T> b;
    for (int i=0; i<N; i++) {b[N-1-i] = a[i];}
    return b;
    }
  template <typename T> inline Tuple<1,T> reverse(Tuple<1,T> a)
    {
    return a;
    }
  template <typename T> inline Tuple<2,T> reverse(Tuple<2,T> a)
    {
    return Tuple<2,T>(a[1],a[0]);
    }
  template <typename T> inline Tuple<3,T> reverse(Tuple<3,T> a)
    {
    return Tuple<3,T>(a[2],a[1],a[0]);
    }

  } // namespace bonelab

#endif
