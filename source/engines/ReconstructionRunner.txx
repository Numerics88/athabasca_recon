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

#include <boost/format.hpp>

using boost::format;

namespace athabasca_recon
  {

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  ReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>::ReconstructionRunner()
    :
    m_Calibration(NULL),
    m_RawProjectionsReader(NULL),
    m_AttenuationProjectionsReader(NULL),
    m_FilteredProjectionsReader(NULL),
    m_BadPixelCorrector(NULL),
    m_BeamPowerCorrector(NULL),
    m_AttenuationCorrectionsFile(NULL),
    m_VolumeWriter(NULL),
    m_AttenuationProjectionsWriter(NULL),
    m_FilteredProjectionsWriter(NULL),
    m_ProjectionsAtBothLimits(true),
    m_ReverseRotation(false),
    m_ScalingFactor(1.0),
    m_FilteringModule(FILTERING_MODULE_VDSP),
    m_PixelInterpolation(PIXEL_INTERPOLATION_BILINEAR),
    m_SmoothingFilter(NULL)
    {}

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void ReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>::Print
    (const std::string& arg)
    {
    std::cout << arg;
    std::cout.flush();
    }

  template <class TProjectionIn, class TProjectionOut, class TVolume>
  void ReconstructionRunner<TProjectionIn, TProjectionOut, TVolume>::Print
    (const format& arg)
    { this->Print(arg.str()); }

  } // namespace athabasca_recon

