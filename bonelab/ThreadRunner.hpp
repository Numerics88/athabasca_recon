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

#ifndef BONELAB_ThreadRunner_hpp_INCLUDED
#define BONELAB_ThreadRunner_hpp_INCLUDED

#include "ThreadWorker.hpp"
#include "MessageQueue.hpp"
#include "exception.hpp"
#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <deque>


namespace bonelab
  {

  /** An object that manages worker threads.
    *
    * This object is equivalent to "main" for a worker thread.  It manages
    * the sending and processing of messages to the thread.
    *
    * Thread-specific resources are managed with a ThreadWorker object.  There
    * is no default ThreadWorker - create a subclass for your particular
    * application and then create the ThreadWorker with a custom Message.
    *
    * To create a thread with a thread runner, use code similar to the
    * following example.  In practice multiple threads (and ThreadRunners)
    * will typically be used.
    *
    * @code
    *    bonelab::ThreadRunner threadRunner;
    *    boost::thread_group threads;
    *    threads.create_thread(boost::bind(&ThreadRunner::Run, &threadRunner));
    *    threadRunner.SetMessage(new CreateThreadWorkerMessage());
    * @endcode
    *
    * To terminate a thread, first synchronize to finish execution on all
    * threads (see SynchronizeMessage).  Then execute 
    *
    * @code
    *    threadRunner.Stop();
    *    threads.join_all();
    * @endcode
    *
    * If an exception occurs during the processing of a Message, the exception
    * will be caught and stored.  It is the responsibility of the control
    * thread to call CheckException at appropriate intervals.  This will
    * cause the exception to be re-thrown in the control thread.
    * You must catch the exception and properly shut down all the threads;
    * you cannot allow the destructor of a ThreadRunner to potenially be
    * called before join.
    */
  class ThreadRunner : private boost::noncopyable
  {
    public:

      ThreadRunner();

      ~ThreadRunner();

      /** The method that should be executed on the thread. */
      void Run();

      /** Sets a message to be processed.
        *
        * This method is thread-safe and may be called from other threads.
        */
      void SetMessage(Message* msg);

      /** Stops the message queue and cleans up.
        *
        * The destructor of the ThreadWorker will be called; typically it
        * will in turn clean up any thread-specific resources.
        *
        * This method is thread-safe and may be called from other threads.
        */
      void Stop();

      /** Re-throws any stored exception. */
      void CheckException();

      /** Returns a pointer to the ThreadWorker.
        * Note that Messages may change this pointer (which is in any case
        * intially NULL).
        */
      ThreadWorker* Worker() {return m_Worker;}

    protected:

      ThreadWorker* m_Worker;
      MessageQueue* m_MessageQueue;
      bool m_Stopped;
      bonelab_exception* m_Exception;

      // Notice that this gets called on the worker thread when Run exits.
      void Destroy();
      
      // Throws only first exception set; deletes subsequent exceptions.
      void SetException(bonelab_exception* e);

    };

  }  // namespace bonelab

#endif
