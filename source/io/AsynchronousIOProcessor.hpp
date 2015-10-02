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

#ifndef BONELAB_AsynchronousIOProcessor_hpp_INCLUDED
#define BONELAB_AsynchronousIOProcessor_hpp_INCLUDED

#include "io/StreamingIOBase.hpp"
#include "AthabascaException.hpp"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/noncopyable.hpp>
#include <queue>
#include <set>

namespace athabasca_recon
  {

  /** Class used by AsynchronousIOProcessor to keep track of Io Requests. */
  class IORequest
    {
    public:

      IORequest()
        :
        m_IOObject(NULL),
        m_Image(NULL)
        {}

      IORequest(StreamingIOBase* ioObject, void* image)
        :
        m_IOObject(ioObject),
        m_Image(image)
        {}

      StreamingIOBase* GetIOObject() const { return this->m_IOObject; }
      void* GetImagePointer() const { return this->m_Image; }
        
    protected:
    
      StreamingIOBase* m_IOObject;
      void* m_Image;
    };


  /** Class to perform IO asynchronously.
    *
    * This class launches a worker thread on construction that performs
    * all the I/O.
    *
    * I/O can be requested with RequestIOOperation.  Both read and write
    * operations are supported.  The requester must provide the
    * object for data storage.  This passed with a void pointer, which
    * is hideous.  However it seems to be much simpler than the equivalent
    * implementation with templates.
    *
    * I/O is performed in the order requested.
    *
    * Before attempting to access the data (in the case reading) or
    * modify the data (in the case of writing), requesters must call
    * WaitForBufferRelease, which blocks until the I/O operation completes.
    *
    * Exceptions that occur in the IO worker thread will be rethrown
    * in WaitForBufferRelease or WaitForCompletion; specifically all
    * currently waiting calls to those functions, and any subsequent calls.
    * Thus the exception will quite likely be rethrown in threads
    * unassociated with the IO request that originally threw the
    * exception.  This seems to work well enough for now, since our only
    * goal after any exception is an orderly stop and cleanup.
    *
    * AsynchronousIOProcessor cannot be deleted while
    * any worker threads are still waiting on data.  In practice, it is
    * sufficient if worker threads are synced before checking for and
    * throwing exceptions that might take AsynchronousIOProcessor out of
    * scope.
    *
    * The performance implications of using this class are sometimes not
    * obvious.  In principle, used together with double buffers,
    * it can speed up I/O by requesting reads before they are required,
    * and by allowing calculations to continue immediately on writes, without
    * waiting for the write to complete. This however largely duplicates the
    * function of the operating system's disk cache.  Therefore in many
    * cases, no improvement in speed is actually realized (and some additional
    * memory is required).  There are a couple of scenarios however where
    * a speed improvement is observed.  Firstly: reading projections
    * from a number of individual files.  No operating system's read-ahead
    * disk caching attempts to guess which file you will open next.
    * Secondly, in cases where memory is short, the operating system may
    * essentially abandon the disk cache.  Of course, double buffering uses
    * more memory, but it does cache exactly "the right" amount of data
    * on reads and writes, which the operating system cannot know.
    * Geeky users who like to optimize to perfection may want to
    * experiment with disabling the operating system's disk caching for
    * the files used by athabasca recon; this is possible in most
    * operating systems.
    *
    * Finally, note that using this class will spawn another thread which
    * can affect the balance of load distribution.  In some cases, this
    * thread does some calculation-intensive work (e.g. if you pass a
    * reader/writer than changes the endianess on read/write).  It can be
    * an advantage to have this work performed on a separate thread, but
    * in can also be a disadvantage (e.g. if it delays a single 
    * back-projection thread, since back-projection completes only when
    * all back-projection threads complete).
    */
  template <typename TRawProjection, typename TProcessedProjection>
  class AsynchronousIOProcessor : private boost::noncopyable
    {
    public:

      /** Constructor.
        *
        * Launches a worker thread for I/O.
        */
      AsynchronousIOProcessor();

      /** Destructor.
        *
        * Will abruptly stop all I/O; for normal operation call WaitForCompletion
        * first.
        */
      ~AsynchronousIOProcessor();

      /** Request an I/O operation.
        *
        * @param ioObject  A subclass of ioObject (ie. a subclass of either
        *                  StreamingReaderBase or StreamingWriterBase).  By
        *                  subclassing, you can support basically any kind
        *                  of I/O, and any file format.
        * @param image     A pointer to the object with the data.  The actual
        *                  type of this object depends on what ioObject requires,
        *                  typically a bonelab::Image.
        *
        * Note that issuing multiple requests with the same value of image
        * will typically result in deadlock unless there is an intervening
        * WaitForBufferRelease(image).
        */
      void RequestIOOperation(StreamingIOBase* ioObject, void* image);

      /** Blocks until the specified I/O request completes.
        *
        * @param image  Identifies the I/O operation: pass the same value as
        *               was used in the originating call to RequestIOOperation.
        *
        * Note that it is not actually necessary to have a corresponding
        * preceeding call to RequestIOOperation; if you fancy it, you can
        * call WaitForBufferRelease first, and subsequently issue a
        * corresponding RequestIOOperation on another thread.  Failing
        * however to call a corresponding RequestIOOperation either before or
        * after will result in this method waiting forever.
        */
      void WaitForBufferRelease(void* image);

      /** Returns only when the request queue is empty.
        *
        * If you are issuing any write requests, you must call this before
        * deleting this object, otherwise the write requests might never
        * be carried out.
        */
      void WaitForCompletion();

      /** Re-throws any stored exception.
        *
        * Users don't usually need to call this explicitly, as exceptions
        * are rethrown in WaitForBufferRelease or WaitForCompletion.
        */
      void CheckException();

    protected:

      void StartIOWorker();
      IORequest GetRequest();
      void ExecuteOperation(IORequest request);

      // Throws only first exception set; deletes subsequent exceptions.
      // Note that notify must be called as usual on m_CompletedCondition after
      // setting an exception; also the request must still be added to
      // m_CompletedRequests even the the request generates an error.
      // Otherwise, deadlock can result.
      void SetException(AthabascaException* e);
      
      std::queue<IORequest> m_Queue;
      std::set<const void*> m_CompletedRequests;
      boost::thread m_WorkerThread;
      boost::mutex m_RequestMutex;
      boost::condition_variable m_RequestCondition;
      boost::mutex m_CompletedMutex;
      boost::condition_variable m_CompletedCondition;
      bool m_Stopped;
      AthabascaException* m_Exception;

    };  // class AsynchronousIOProcessor

  } // namespace bonelab

// Include the template definitions so we don't require explicit instantiations
#include "AsynchronousIOProcessor.txx"

#endif
