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

#include "ThreadRunner.hpp"

#ifdef TRACE_THREADING
#include "print_multithread.hpp"
#include <boost/format.hpp>
using bonelab::print_mt;
using boost::format;
#endif


namespace bonelab
  {

  ThreadRunner::ThreadRunner()
    :
    m_Worker(NULL),
    m_MessageQueue(NULL),
    m_Stopped(false),
    m_Exception(NULL)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating ThreadRunner %d\n") %  this);
#endif
    this->m_MessageQueue = new MessageQueue();
    }

  ThreadRunner::~ThreadRunner()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("ThreadRunner %d: Destructor\n") % this);
#endif
    }

  void ThreadRunner::Run()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("ThreadRunner %d: Running\n") % this);
#endif
    while (!this->m_Stopped)
      {
      Message *msg = this->m_MessageQueue->GetMessage();
      try
        {
#ifdef TRACE_THREADING
        print_mt(std::cout, format("ThreadRunner %d: Executing Message %d\n") % this % msg);
#endif
        msg->Execute(this->m_Worker);
        }
      catch (bonelab_exception& e)
        {
#ifdef TRACE_THREADING
        print_mt(std::cout, format("ThreadRunner %d: Exception thrown in Message %d\n") % this % msg);
#endif
        this->SetException(new bonelab_exception(e)); 
        }
      catch (std::exception& e)
        {
#ifdef TRACE_THREADING
        print_mt(std::cout, format("ThreadRunner %d: Exception thrown in Message %d\n") % this % msg);
#endif
        this->SetException(new bonelab_exception(e.what())); 
        }
      catch (...)
        {
#ifdef TRACE_THREADING
        print_mt(std::cout, format("ThreadRunner %d: Exception thrown in Message %d\n") % this % msg);
#endif
        this->SetException(new bonelab_exception("Unknown exception in thread."));
        }
      delete msg;
      }
    Destroy();
    }

  void ThreadRunner::Stop()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("ThreadRunner %d: Stop\n") % this);
#endif
    this->m_Stopped = true;
    // Send blank message to force message-reading loop in Run() to exit.
    SetMessage(new Message());
    }

  void ThreadRunner::SetMessage(Message* msg)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("ThreadRunner %d: setting Message %d\n") % this % msg);
#endif
    if (this->m_MessageQueue)
      { this->m_MessageQueue->SetMessage(msg); }
    }

  void ThreadRunner::CheckException()
    {
    if (this->m_Exception)
      { throw *this->m_Exception;  }
    }

  void ThreadRunner::Destroy()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("ThreadRunner %d: Destroy\n") % this);
#endif
    delete this->m_MessageQueue;
    this->m_MessageQueue = NULL;
    delete this->m_Worker;
    this->m_Worker = NULL;
    delete this->m_Exception;
    this->m_Exception = NULL;
    }

  void ThreadRunner::SetException(bonelab_exception* e)
    {
    // In case of multiple exceptions, keep first
    if (!this->m_Exception)
      { this->m_Exception = e; }
    else
      { delete e; }
    }

  }  // namespace bonelab
