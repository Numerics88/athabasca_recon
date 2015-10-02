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

#ifndef BONELAB_ReconstructionLauncher_hpp_INCLUDED
#define BONELAB_ReconstructionLauncher_hpp_INCLUDED

#include "ReconConfiguration.hpp"
#include "bonelab/Image.hpp"
#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** Class to create required processing objects based on a
    * ReconConfiguration, and launch a ReconstructionRunner.
    *
    * There is only one implementation of this class, in contrast to
    * the ReconstructionRunner, which has multiple implementations (e.g.
    * single-threaded and multi-threaded).
    *
    * The ReconConfiguration is not passed on to the ReconstructionRunner,
    * and must therefore be fully processed here.  Processing objects are
    * created here if possible, otherwise postponed to ReconstructionRunner,
    * or worker threads as required.
    */
  template <class TProjectionIn, class TProjectionOut, class TVolume>
  class ReconstructionLauncher : private boost::noncopyable
    {
    public:

      typedef typename TProjectionOut::space_type TSpace;
      typedef typename TProjectionOut::index_type  TIndex;
      typedef typename TProjectionIn::value_type TProjectionInValue;
      typedef typename TProjectionOut::value_type TProjectionOutValue;
      typedef typename TVolume::value_type TVolumeValue;
      typedef bonelab::Image<2,TVolumeValue,TIndex,TSpace> TVolumeSlice;

      // Some paranoid checks that TSpace and TIndex is in fact the same for all the
      // objects.  (Because for simplicity we're going to assume it,
      // although a more general approach could use implemented if necessary.)
      BOOST_STATIC_ASSERT(sizeof(typename TProjectionIn::space_type) == sizeof(TSpace));
      BOOST_STATIC_ASSERT(sizeof(typename TVolume::space_type) == sizeof(TSpace));
      BOOST_STATIC_ASSERT(sizeof(typename TProjectionIn::index_type) == sizeof(TIndex));
      BOOST_STATIC_ASSERT(sizeof(typename TVolume::index_type) == sizeof(TIndex));
      
      /** Constructor. */
      ReconstructionLauncher(const ReconConfiguration& config);
      
      /** Create processing objects and start a ReconstructionRunner. */
      void Launch();

    protected:
      
      const ReconConfiguration& m_Config;
      
    };  // class ReconstructionLauncher

  }  // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "ReconstructionLauncher.txx"

#endif
