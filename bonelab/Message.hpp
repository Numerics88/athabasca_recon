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

#ifndef BONELAB_Message_hpp_INCLUDED
#define BONELAB_Message_hpp_INCLUDED

#include "ThreadWorker.hpp"
#include <boost/noncopyable.hpp>


namespace bonelab
  {

  /** A base class for inter-thread messages.
    *
    * To create a message, subclass Message and implement Execute to perform
    * some task.  You may make use of ThreadWorker, which is typically used
    * to manage thread-local resources.  This is however not necessary.
    */
  class Message : private boost::noncopyable
    {
    public:

      Message() {}

      virtual ~Message() {}

      virtual void Execute(ThreadWorker*& object) {}

    };

  }  // namespace bonelab

#endif
