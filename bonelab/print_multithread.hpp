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

#ifndef BONELAB_print_multithread_hpp_INCLUDED
#define BONELAB_print_multithread_hpp_INCLUDED

#include <boost/format.hpp>
#include <iostream>
#include <string>

namespace bonelab
  {

  /** Simple print functions that use a common static mutex to serialize
    * access.  The purpose is to prevent messages from different threads
    * becoming garbled together.
    */
  void print_mt(std::ostream& s, const char* msg);
  void print_mt(std::ostream& s, const std::string& msg);
  void print_mt(std::ostream& s, const boost::format& msg);

  }  // namespace bonelab

#endif
