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

#ifndef BONELAB_EXCEPTION_INCLUDED
#define BONELAB_EXCEPTION_INCLUDED

#include <boost/format.hpp>
#include <string>
#include <exception>


/** Throws bonelab_exception and sets the file name and line number. */
#define throw_bonelab_exc(x)                                                 \
    if(!std::uncaught_exception())                                          \
        throw bonelab::bonelab_exception (x, __FILE__, __LINE__)

/** If the argument is false, throws bonelab_exception and sets the file name and line number. */
#define bonelab_assert(x)                                                  \
    if (!(x))                                                               \
        throw bonelab::bonelab_exception ("Assertion failure", __FILE__, __LINE__)

/** If the argument is true, throws bonelab_exception and sets the file name and line number. */
#define bonelab_negative_assert(x)                                         \
    if (x)                                                                  \
        throw bonelab::bonelab_exception ("Assertion failure", __FILE__, __LINE__)


namespace bonelab
  {

  /** An exception class for the bonelab utilities. */
  class bonelab_exception : public std::exception
    {
    public:
      
      /** Constructor.
        *
        * @param what  Description of the exception.
        */
      explicit bonelab_exception (const std::string& what) throw()
        :
        m_what(what),
        m_file("Unknown"),
        m_line(0)
        {}

      /** Constructor.
        *
        * @param what  Description of the exception.
        * @param file  The source code file where this occurred.
        * @param file  The line number where this occurred.
        */
      explicit bonelab_exception
        (
        const std::string& what,
        const std::string& file,
        int line
        ) throw()
        :
        m_what(what),
        m_file(file),
        m_line(line)
        {}

      virtual ~bonelab_exception() throw()
        {}

      /** Returns the description of the exception. */
      virtual const char* what() const throw()
        { return m_what.c_str(); }

     /** Returns source code file where the exception occurred. */
      virtual const char* file() const throw()
        { return m_file.c_str(); }

     /** Returns line number where the exception occurred. */
      virtual int line() const throw()
        { return m_line; }

    protected:
      std::string m_what;
      std::string m_file;
      int m_line;

    };

  } // namespace bonelab

#endif
