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

#ifndef BONELAB_BeforeAndAfterBrightFieldBeamPowerCorrector_HPP_INCLUDED
#define BONELAB_BeforeAndAfterBrightFieldBeamPowerCorrector_HPP_INCLUDED

#include "ManualBeamPowerCorrector.hpp"
#include "CTCalibration.hpp"
#include "BadPixelCorrector.hpp"

namespace athabasca_recon
  {

  /** A class for performing beam power correction based on bright field
    * measurements done before and after the scan.
    *
    * This method assumes exponential decay of the beam power, which is
    * equivalent to a constant shift of the attenuations with time.
    *
    * Ideally, all the measurement times are known.  If not, a constant time
    * between scans is assumed, and the interval between the bright field done before
    * and the scan, as well as between the last scan and the bright field done
    * subsequently, can be specified.
    *
    * For further discussion of this method please refer to the manual.
    */
  template <typename TProjection>
  class BeforeAndAfterBrightFieldBeamPowerCorrector : public ManualBeamPowerCorrector<TProjection>
    {
    public:
      
      typedef typename TProjection::value_type TValue;

      /** Initializes.
        *
        * @param calibration  A CTCalibration object.  Provides the before and
        *                     after bright fields, as well as possibly
        *                     measurement times or intervals.
        * @param numberOfProjections  The number of projections that will be
        *                     processed.
        * @param projectionStride  The projection stride.  This needs to be
        *                     known in order that assumed intervals before and
        *                     after the bright field measurements don't depend
        *                     on the chosen projection stride.
        * @param badPixelCorrector  A bad pixel corrector, if used.  Is required
        *                     to exclude bad pixels from analysis of the
        *                     bright fields.
        */
      template <typename TProjectionIn>
      void Initialize(CTCalibration<TProjectionIn,TProjection>* calibration,
                      int numberOfProjections,
                      int projectionStride,
                      BadPixelCorrector<TProjection>* badPixelCorrector);

    };  // class BeforeAndAfterBrightFieldBeamPowerCorrector

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "BeforeAndAfterBrightFieldBeamPowerCorrector.txx"

#endif
