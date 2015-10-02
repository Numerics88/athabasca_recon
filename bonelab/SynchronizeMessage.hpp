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

#ifndef BONELAB_SynchronizeMessage_hpp_INCLUDED
#define BONELAB_SynchronizeMessage_hpp_INCLUDED

#include "Message.hpp"
#include "ThreadRunner.hpp"
#include "ObjectGroup.hpp"
#include <boost/thread.hpp>

namespace bonelab
  {

  /** A message that will cause threads to synchronize.
    *
    * At a synchronization barrier, all threads will stop until all threads
    * have reached the barrier.
    *
    * You must create a boost::barrier object to pass to the Synchronize
    * message.
    *
    * Send one SynchronizeMessage (with the same barrier object) to each
    * thread that you want to synchronize.
    */
  class SynchronizeMessage : public Message
    {
    public:

      SynchronizeMessage(boost::barrier &barrier);

      virtual ~SynchronizeMessage() {}

      virtual void Execute(ThreadWorker*& object);

    protected:

      boost::barrier& m_Barrier;
    };

  // ---------------------------------------------------------------------
  // Useful utility functions
  
  /** A utility function to synchronize a group of threads by sending a
    * Synchronize message to each one.
    *
    * This version does not block; that is, the thread from which you
    * call this method (typically the main thread) does not wait for
    * synchronization before proceeding.
    *
    * NOTE: Requires that the count of barrier be equal to the number of
    * ThreadRunners.  There is no way to verify this, but getting it
    * wrong will cause deadlock.
    */
  inline void SynchronizeAll
    (
    ObjectGroup<ThreadRunner>& threadRunners,
    boost::barrier& barrier
    )
    {
    for (int t=0; t<threadRunners.size(); ++t)
      { threadRunners[t].SetMessage(new SynchronizeMessage(barrier)); }
    }

  /** A utility function to synchronize a group of threads plus the main thread
    * by sending a Synchronize message to each one.
    *
    * This version blocks; that is, the thread from which you
    * call this method (typically the main thread) waits for
    * synchronization before proceeding.
    *
    * NOTE: Requires that the count of barrier be ONE MORE than the number of
    * ThreadRunners.  There is no way to verify this, but getting it
    * wrong will cause deadlock.
    */
  inline void SynchronizeAllAndWait
    (
    ObjectGroup<ThreadRunner>& threadRunners,
    boost::barrier& barrier
    )
    {
    SynchronizeAll(threadRunners, barrier);
    barrier.wait();
    }

  /** A utility function to synchronize the main thread with a single
    * worker thread.
    *
    * NOTE: Requires that the count of barrier be exactly two.
    */
  inline void SynchronizeOneAndWait
    (
    ThreadRunner& threadRunner,
    boost::barrier& barrier
    )
    {
    threadRunner.SetMessage(new SynchronizeMessage(barrier));
    barrier.wait();
    }

  }  // namespace bonelab

#endif
