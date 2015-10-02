#include "io/StreamingProjectionsReader.hpp"
#include "io/StreamingWriter.hpp"
#include "AthabascaException.hpp"
#include "bonelab/print_multithread.hpp"
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

using boost::format;
using bonelab::print_mt;

namespace athabasca_recon
  {

  template <typename TRawProjection, typename TProcessedProjection>
  AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::AsynchronousIOProcessor()
    :
    m_Stopped(false),
    m_Exception(NULL)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, "Launching AsynchronousIOProcessor worker thread\n");
#endif 
    this->m_WorkerThread = boost::thread(boost::bind(
          &AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::StartIOWorker,
          this));
    }

  template <typename TRawProjection, typename TProcessedProjection>
  AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::~AsynchronousIOProcessor()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, "Stopping AsynchronousIOProcessor\n");
#endif
    this->m_Stopped = true;
    this->m_RequestCondition.notify_all();
    this->m_CompletedCondition.notify_all();
    this->m_WorkerThread.join();
    delete this->m_Exception;
    this->m_Exception = NULL;
#ifdef TRACE_THREADING
    print_mt(std::cout, "Exiting ~AsynchronousIOProcessor\n");
#endif 
    }

  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::RequestIOOperation
    (
    StreamingIOBase* ioObject,
    void* image
    )
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("AsynchronousIOProcessor::RequestIOOperation(%d) called.\n") % image);
#endif 
    // scope for Request lock
      {
      boost::lock_guard<boost::mutex> lock(this->m_RequestMutex);
      if (this->m_Stopped)
        { return; }
      this->m_Queue.push(IORequest(ioObject, image));
      }
    this->m_RequestCondition.notify_all();
    }


  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::WaitForBufferRelease(void* image)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("AsynchronousIOProcessor::WaitForBufferRelease(%d) called.\n") % image);
#endif 
    CheckException();
    boost::unique_lock<boost::mutex> lock(this->m_CompletedMutex);
    if (this->m_Stopped)
      { throw_athabasca_exception("AsynchronousIOProcessor stopped with active request"); }
    while (!(this->m_CompletedRequests.count(image)))
      {
      this->m_CompletedCondition.wait(lock);
      if (this->m_Stopped)
        { throw_athabasca_exception("AsynchronousIOProcessor stopped with active request"); }
      CheckException();
      }
#ifdef TRACE_THREADING
    print_mt(std::cout, format("AsynchronousIOProcessor::WaitForBufferRelease(%d): proceeding.\n") % image);
#endif 
    this->m_CompletedRequests.erase(image);
    }


  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::WaitForCompletion()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, "AsynchronousIOProcessor::WaitForCompletion called.\n");
#endif 
    CheckException();
    boost::unique_lock<boost::mutex> lock(this->m_CompletedMutex);
    if (this->m_Stopped)
      { throw_athabasca_exception("AsynchronousIOProcessor stopped with active request"); }
    while (1)
      {
      size_t requestCount = 0;
        {
        boost::lock_guard<boost::mutex> lock(this->m_RequestMutex);
        requestCount = this->m_Queue.size();
        }
      if (requestCount == 0)
        { break; }    
#ifdef TRACE_THREADING
       print_mt(std::cout, format("AsynchronousIOProcessor::WaitForCompletion: waiting; "
                           " %d requests still in queue\n") % requestCount);
#endif 
      this->m_CompletedCondition.wait(lock);
      if (this->m_Stopped)
        { throw_athabasca_exception("AsynchronousIOProcessor stopped with active request"); }
      CheckException();
      }
#ifdef TRACE_THREADING
    print_mt(std::cout, "AsynchronousIOProcessor::WaitForCompletion: proceeding.\n");
#endif 
    }


  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::StartIOWorker()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, "AsynchronousIOProcessor::StartIOWorker called.\n");
#endif 
    while (!this->m_Stopped)
      {
      IORequest request = GetRequest();
      if (this->m_Stopped)
        { break; }
      ExecuteOperation(request);
      }
#ifdef TRACE_THREADING
    print_mt(std::cout, "AsynchronousIOProcessor::StartIOWorker: exiting.\n");
#endif 
    }


  template <typename TRawProjection, typename TProcessedProjection>
  IORequest AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::GetRequest()
    {
    boost::unique_lock<boost::mutex> lock(this->m_RequestMutex);
    while (!this->m_Stopped)
      {
      if (!this->m_Queue.empty())
        {
        IORequest request = this->m_Queue.front();
        this->m_Queue.pop();
        return request;
        }
      // No requests we can act on; wait for new incoming requests.
      this->m_RequestCondition.wait(lock);
      }
    return IORequest();  // Return dummy value - only get here if Stopped.
    }


  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::ExecuteOperation
    (IORequest request)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("AsynchronousIOProcessor::ExecuteOperation %d\n")
                        % request.GetImagePointer());
#endif
    try
      {
      if (!this->m_Exception)
        {
        if (StreamingProjectionsReader<TRawProjection>* reader
              = dynamic_cast<StreamingProjectionsReader<TRawProjection>*>(request.GetIOObject()))
          {
          reader->GetNextProjection(*reinterpret_cast<TRawProjection*>(request.GetImagePointer()));
          }
        else if (StreamingProjectionsReader<TProcessedProjection>* reader
              = dynamic_cast<StreamingProjectionsReader<TProcessedProjection>*>(request.GetIOObject()))
          {
          reader->GetNextProjection(*reinterpret_cast<TProcessedProjection*>(request.GetImagePointer()));
          }
        else if (StreamingWriter<TRawProjection>* writer
              = dynamic_cast<StreamingWriter<TRawProjection>*>(request.GetIOObject()))
          {
          writer->WriteSlice(*reinterpret_cast<TRawProjection*>(request.GetImagePointer()));
          }
        else if (StreamingWriter<TProcessedProjection>* writer
              = dynamic_cast<StreamingWriter<TProcessedProjection>*>(request.GetIOObject()))
          {
          writer->WriteSlice(*reinterpret_cast<TProcessedProjection*>(request.GetImagePointer()));
          }
        else
          { throw_athabasca_exception("Unexpected IO Object type."); }
        }
      }
    catch (AthabascaException& e)
      {
#ifdef TRACE_THREADING
      print_mt(std::cout, "Exception thrown in AsynchronousIOProcessor worker thread.\n");
#endif
      this->SetException(new AthabascaException(e));
      }
    catch (bonelab::bonelab_exception& e)
      {
#ifdef TRACE_THREADING
      print_mt(std::cout, "Exception thrown in AsynchronousIOProcessor worker thread.\n");
#endif
      this->SetException(new AthabascaException(e)); 
      }
    catch (std::exception& e)
      {
#ifdef TRACE_THREADING
      print_mt(std::cout, "Exception thrown in AsynchronousIOProcessor worker thread.\n");
#endif
      this->SetException(new AthabascaException(e.what()));
      }
    catch (...)
      {
#ifdef TRACE_THREADING
      print_mt(std::cout, "Exception thrown in AsynchronousIOProcessor worker thread.\n");
#endif
      this->SetException(new AthabascaException("Unknown exception in AsynchronousIOProcessor worker thread."));
      }

    // scope for completed lock starts here
    // Note that we must ALWAYS add the request to the CompletedRequests,
    // even if an exception occurred.  Otherwise, an exception will cause
    // deadlock.
      {
      boost::lock_guard<boost::mutex> lock(this->m_CompletedMutex);
#ifdef TRACE_THREADING
      print_mt(std::cout, format("AsynchronousIOProcessor::ExecuteOperation registering %d\n")
                          % request.GetImagePointer());
#endif 
      // Add this to the list of completed writes
      this->m_CompletedRequests.insert(request.GetImagePointer());
      }
    // Notify all threads waiting for data
    this->m_CompletedCondition.notify_all();
    }

  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::CheckException()
    {
    if (this->m_Exception)
      { throw *this->m_Exception; }
    }
  
  template <typename TRawProjection, typename TProcessedProjection>
  void AsynchronousIOProcessor<TRawProjection,TProcessedProjection>::SetException
    (AthabascaException* e)
    {
    // In case of multiple exceptions, keep first, discard subsequent
    if (!this->m_Exception)
      { this->m_Exception = e; }
    else
      { delete e; }
    }

  } // namespace bonelab
