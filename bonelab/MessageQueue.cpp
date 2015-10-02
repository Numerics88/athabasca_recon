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

#include "MessageQueue.hpp"
#include "exception.hpp"
#ifdef TRACE_THREADING
#include <boost/format.hpp>
#include "print_multithread.hpp"
using boost::format;
#endif
#include <iostream>

namespace bonelab
  {

  MessageQueue::MessageQueue()
    :
    m_Stopped(false)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating new MessageQueue %d\n") % this);
#endif
    }

  MessageQueue::~MessageQueue()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Destructor for MessageQueue %d\n") % this);
#endif
    try
      {
      Stop();
      Destroy();
      }
    catch (...)
      { std::cerr << "WARNING: Exception in MessageQueue destructor\n"; }
    }

  void MessageQueue::SetMessage(Message* msg)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("MessageQueue %d: setting Message %d\n") % this % msg);
#endif
      {  // scope for lock
      boost::lock_guard<boost::mutex> lock(this->m_Mutex);
      if (this->m_Stopped) return;
      this->m_MessageQueue.push_back(msg);
      }
    this->m_Condition.notify_one();
    }

  Message* MessageQueue::GetMessage()
    {
    if (this->m_Stopped) { return NULL; }
    boost::unique_lock<boost::mutex> lock(this->m_Mutex);
#ifdef TRACE_THREADING
    print_mt(std::cout, format("MessageQueue %d: GetMessage\n") % this);
#endif
    while (this->m_MessageQueue.empty())
      {
      this->m_Condition.wait(lock);
      if (this->m_Stopped) { return NULL; }
      }
    Message* msg = this->m_MessageQueue.front();
    this->m_MessageQueue.pop_front();
#ifdef TRACE_THREADING
    print_mt(std::cout, format("MessageQueue %d: fetched message %d\n") % this % msg);
#endif
    return msg;
    }

  void MessageQueue::Stop()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("MessageQueue %d: Stop\n") % this);
#endif
      {  // scope for lock
      boost::lock_guard<boost::mutex> lock(this->m_Mutex);
      this->m_Stopped = true;
      }
    this->m_Condition.notify_one();
    }

  void MessageQueue::Destroy()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("MessageQueue %d: Destroy\n") % this);
#endif
    // Discard all remaining messages
    while (!this->m_MessageQueue.empty())
      {
      delete this->m_MessageQueue.front();
      this->m_MessageQueue.pop_front();
      }
    }

  }  // namespace bonelab
