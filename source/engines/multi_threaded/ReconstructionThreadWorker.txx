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

namespace athabasca_recon
  {

  template <class TProjection, class TVolume>
  ReconstructionThreadWorker<TProjection,TVolume>::ReconstructionThreadWorker()
    :
    ProjectionFilterer(NULL),
    BackProjector(NULL)
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Creating ReconstructionThreadWorker %d\n") % this);
#endif
    }

  template <class TProjection, class TVolume>
  ReconstructionThreadWorker<TProjection,TVolume>::~ReconstructionThreadWorker()
    {
#ifdef TRACE_THREADING
    print_mt(std::cout, format("Destroying ReconstructionThreadWorker %d\n") % this);
#endif
    delete this->ProjectionFilterer;
    delete this->BackProjector;
    }

  }  // namespace athabasca_recon
