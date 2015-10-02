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

#ifndef BONELAB_safe_stream_write_hpp_INCLUDED
#define BONELAB_safe_stream_write_hpp_INCLUDED

#include <fstream>

const size_t safe_stream_write_max_count = 1 << 30;

inline void safe_stream_write(std::ostream& s, const void* ptr, size_t count)
  {
#ifdef _WINDOWS
  // Writing files larger than 4GB using C++ standard calls is irredeemably broken with
  // Visual Studio 2008 and earlier.  The following lines are not a solution,
  // although curiously they do tend to increase the file sizes which cause crashes.
  // The only real solution is to use Visual Studio 2010 or later.  Actually, the best
  // solution is clearly to avoid Windows altogether, but that option is not
  // available to everyone.
  while (count > 0)
    {
    size_t write_count = std::min(safe_stream_write_max_count, count);
    s.write((const char*)ptr, write_count);
    ptr = (const char*)ptr + write_count;
    count -= write_count;
    }
#else        
    s.write((const char*)ptr, count);
#endif
  }

#endif
