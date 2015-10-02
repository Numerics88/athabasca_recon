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

#ifndef BONELAB_sysinfo_hpp_INCLUDED
#define BONELAB_sysinfo_hpp_INCLUDED

#ifdef _WINDOWS
#include <Windows.h>
#elif defined __APPLE__
//#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined __linux__
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

namespace bonelab
  {

  /** Returns the total installed system memory. */
  inline long long int GetSystemTotalMemory();

#ifdef _WINDOWS

  inline long long int GetSystemTotalMemory()
    {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
    }

#elif defined __APPLE__

  inline long long int GetSystemTotalMemory()
    {
    uint64_t total = 0;
    size_t size = sizeof(total);
    sysctlbyname("hw.memsize", &total, &size, NULL, 0);
    return total;
    }

#elif defined __linux__

  inline long long int GetSystemTotalMemory()
    {
    struct sysinfo memInfo;
  
    sysinfo (&memInfo);
    return static_cast<long long int>(memInfo.totalram)
         * static_cast<long long int>(memInfo.mem_unit);
    }

#else

#error Unrecognized OS.

#endif

  }   // namespace bonelab

#endif
