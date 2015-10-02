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

#ifndef BONELAB_MessageQueue_hpp_INCLUDED
#define BONELAB_MessageQueue_hpp_INCLUDED

#include "Message.hpp"
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <deque>


namespace bonelab
  {

  /** A message queue for inter-thread messaging.
    *
    * This MessageQueue is intended to be used with a single ThreadRunner.
    * In other words it supports messages targetted at a specific thread.
    * It is not suitable to be used as a shared message queue.
    *
    * Users should not set messages directly to this object, but instead
    * set messages through ThreadRunner.
    */
  class MessageQueue : private boost::noncopyable
    {
    public:

      MessageQueue();

      ~MessageQueue();

      /** Add a message to the queue. */
      void SetMessage(Message* msg);

      /** Retreive a message from the queue.
        * This blocks until a message is available.
        */
      Message* GetMessage();

    protected:

      std::deque<Message*> m_MessageQueue;
      boost::mutex m_Mutex;
      boost::condition_variable m_Condition;
      bool m_Stopped;

      void Stop();
      void Destroy();

    };

  }  // namespace bonelab

#endif
