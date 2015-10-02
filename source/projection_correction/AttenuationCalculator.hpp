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

#ifndef BONELAB_AttenuationCalculator_hpp_INCLUDED
#define BONELAB_AttenuationCalculator_hpp_INCLUDED

#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** A class that converts raw projections to attenuation projections.
    */
  template <typename TProjectionIn, typename TProjectionOut>
  class AttenuationCalculator : private boost::noncopyable
    {
    public:
      
      typedef typename TProjectionIn::value_type TProjectionInValue;
      typedef typename TProjectionOut::value_type TProjectionOutValue;
      typedef typename TProjectionIn::index_type TIndex;
      typedef CTCalibration<TProjectionIn,TProjectionOut> TCalibration;
      
      AttenuationCalculator();
      
      /** Set the CTCalibration object.
        * Required; must be set before calling ProcessProjection.
        */
      void SetCalibration(const TCalibration* calibration) {this->m_Calibration = calibration;}

      /** Convert a raw projection to an attenuation projection.
        *
        * @param in  the input raw projection
        * @param out  the output attenuation projection
        */
      void ProcessProjection(const TProjectionIn& in, TProjectionOut& out) const;

    protected:

      const TCalibration* m_Calibration;

    };  // class AttenuationCalculator

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "AttenuationCalculator.txx"

#endif
