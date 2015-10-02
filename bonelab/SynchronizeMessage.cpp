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

#include "SynchronizeMessage.hpp"
#ifdef TRACE_THREADING
#include "print_multithread.hpp"
#include <boost/format.hpp>
using bonelab::print_mt;
using boost::format;
#endif


namespace bonelab
  {

  SynchronizeMessage::SynchronizeMessage(boost::barrier &barrier)
    :
    m_Barrier(barrier)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating SynchronizeMessage %d\n") % this);
#endif
  }

  void SynchronizeMessage::Execute(ThreadWorker*& object)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("SynchronizeMessage %d waiting\n") % this);
#endif
    this->m_Barrier.wait();
#ifdef TRACE_THREADING
    print_mt(std::cout, format("SynchronizeMessage %d proceeding\n") % this);
#endif
    }

  }  // namespace bonelab
