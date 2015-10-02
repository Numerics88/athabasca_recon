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

#ifndef BONELAB_AthabascaException_hpp_INCLUDED
#define BONELAB_AthabascaException_hpp_INCLUDED

#include "bonelab/exception.hpp"


/** Throws athabasca_exception and sets the file name and line number. */
#define throw_athabasca_exception(x)                                        \
    if(!std::uncaught_exception())                                          \
        throw athabasca_recon::AthabascaException (x, __FILE__, __LINE__)

/** If the argument is false, throws athabasca_exception and sets the file name and line number. */
#define athabasca_assert(x)                                                 \
    if (!(x))                                                               \
        throw athabasca_recon::AthabascaException ("Assertion failure", __FILE__, __LINE__)

/** If the argument is false, throws athabasca_exception and sets the file name and line number.
  * Also sets an informative message.
  */
#define athabasca_verbose_assert(x, msg)                                    \
    if (!(x))                                                               \
        throw athabasca_recon::AthabascaException (                           \
          std::string("Assertion failure : ") + msg, __FILE__, __LINE__);

/** If the argument is true, throws athabasca_exception and sets the file name and line number. */
#define athabasca_negative_assert(x)                                        \
    if (x)                                                                  \
        throw athabasca_recon::AthabascaException ("Assertion failure", __FILE__, __LINE__)


namespace athabasca_recon
  {
   
  /** An exception class for athabasca_recon. */
  class AthabascaException : public bonelab::bonelab_exception
    {
    public:

      /** Constructor.
        *
        * @param what  Description of the exception.
        */
      explicit AthabascaException (const std::string& what) throw()
        :
        bonelab_exception(what)
        {}

      /** Constructor.
        *
        * @param what  Description of the exception.
        * @param file  The source code file where this occurred.
        * @param file  The line number where this occurred.
        */
      explicit AthabascaException
        (
        const std::string& what,
        const std::string& file,
        int line
        ) throw()
        :
        bonelab_exception(what, file, line)
        {}

      /** Copy constructor from bonelab exception.
        *
        * @param e  bonelab_exception to be copied.
        */
      explicit AthabascaException
        (
        const bonelab::bonelab_exception& e
        ) throw()
        :
        bonelab_exception(e)
        {}

    };

  } // namespace bonelab

#endif
