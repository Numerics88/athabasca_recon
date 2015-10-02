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

#include "AthabascaException.hpp"
#include <cmath>

namespace athabasca_recon
  {

  template <typename TProjection>
  template <typename TProjectionIn>
  void BeforeAndAfterBrightFieldBeamPowerCorrector<TProjection>::Initialize
    (
    CTCalibration<TProjectionIn,TProjection>* calibration,
    int numberOfProjections,
    int projectionStride,
    BadPixelCorrector<TProjection>* badPixelCorrector
    )
    {
    athabasca_assert(calibration);
    athabasca_assert(calibration->GetFlatField().is_constructed());
    athabasca_assert(calibration->GetPostScanFlatField().is_constructed());
    athabasca_assert(numberOfProjections > 0);
    athabasca_assert(projectionStride > 0);
    // Make copies so we can set bad pixels to zero.
    // (This is faster than checking IsBadPixel inside a loop.)
    TProjection flatField;
    flatField.copy(calibration->GetFlatField());
    TProjection postScanFlatField;
    postScanFlatField.copy(calibration->GetPostScanFlatField());
    athabasca_assert(flatField.dims() == postScanFlatField.dims());
    if (badPixelCorrector)
      {
      badPixelCorrector->ZeroBadPixels(flatField);
      badPixelCorrector->ZeroBadPixels(postScanFlatField);
      }
    double integratedFlatField = 0;
    for (size_t i=0; i<flatField.size(); ++i)
      { integratedFlatField += flatField[i]; }
    double integratedPostScanFlatField = 0;
    for (size_t i=0; i<postScanFlatField.size(); ++i)
      { integratedPostScanFlatField += postScanFlatField[i]; }
    double projectionIntervals = numberOfProjections - 1
      + (calibration->GetBrightFieldAdvanceInterval()
         + calibration->GetPostScanBrightFieldInterval()) / projectionStride;
    this->m_BeamDecayLinearTerm = log(integratedFlatField/integratedPostScanFlatField)
                                   / projectionIntervals;
    this->m_BeamDecayConstantTerm = calibration->GetBrightFieldAdvanceInterval()
                            *this->m_BeamDecayLinearTerm/projectionStride;
    }

  } // namespace athabasca_recon
