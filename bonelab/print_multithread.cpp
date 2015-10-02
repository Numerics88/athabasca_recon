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

#include "print_multithread.hpp"
#include <boost/thread.hpp>
#include <iostream>

namespace bonelab
  {

  static boost::mutex g_PrintMutex;

  void print_mt(std::ostream& s, const char* msg)
    {
    boost::lock_guard<boost::mutex> lock(g_PrintMutex);
    s << msg;
    }

  void print_mt(std::ostream& s, const std::string& msg)
    {
    boost::lock_guard<boost::mutex> lock(g_PrintMutex);
    s << msg;
    }

  void print_mt(std::ostream& s, const boost::format& msg)
    {
    boost::lock_guard<boost::mutex> lock(g_PrintMutex);
    s << msg;
    }

  }  // namespace bonelab
