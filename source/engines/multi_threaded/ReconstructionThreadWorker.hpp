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

#ifndef ATHABASCA_RECON_ReconstructionThreadWorker_hpp_INCLUDED
#define ATHABASCA_RECON_ReconstructionThreadWorker_hpp_INCLUDED

#include "filtering/ProjectionFilterer.hpp"
#include "back_projection/ParallelBackProjector.hpp"
#include "bonelab/ThreadWorker.hpp"
#include <boost/noncopyable.hpp>


namespace athabasca_recon
  {

  /** An implementation of ThreadWorker that manages the thread-local
    * resources for a reconstruction.
    *
    * As currrently implemented only the ProjectionFilterer and the
    * BackProjector (which stores a portion of the volume) are thread-local.
    */
  template <class TProjection, class TVolume>
  class ReconstructionThreadWorker
    :
    public bonelab::ThreadWorker,
    private boost::noncopyable
    {
    public:

      typedef ParallelBackProjector<TProjection,TVolume> TBackProjector;
      typedef ProjectionFilterer<TProjection> TProjectionFilterer;

      ReconstructionThreadWorker();

      virtual ~ReconstructionThreadWorker();

      TProjectionFilterer* ProjectionFilterer;
      TBackProjector* BackProjector;

    };

  }  // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ReconstructionThreadWorker.txx"

#endif
