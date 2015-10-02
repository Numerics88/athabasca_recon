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

#ifndef BONELAB_ThreadWorker_hpp_INCLUDED
#define BONELAB_ThreadWorker_hpp_INCLUDED

namespace bonelab
  {

  /** An abstract class for managing thread-local resources on worker threads.
    *
    * Subclass the object to implement thread-local resource management
    * for your worker threads.  The resources may be allocated data, or
    * processing objects (for example each worker thread may require its
    * own FFT class).  The ThreadWorker is stored by the ThreadRunner.
    * You will need to create a Message the creates your ThreadWorker
    * and sets the ThreadRunners' Worker pointer to it.  It will then
    * be automatically de-allocated when ThreadRunner exits.  Once
    * created in this way, Messages have access to the ThreadWorker and
    * may use it as they see fit.
    */
  class ThreadWorker
    {
    public:

      ThreadWorker() {}
      virtual ~ThreadWorker() {}

    };

  }  // namespace bonelab

#endif
