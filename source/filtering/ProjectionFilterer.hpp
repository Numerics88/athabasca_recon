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

#ifndef BONELAB_ProjectionFilterer_HPP_INCLUDED
#define BONELAB_ProjectionFilterer_HPP_INCLUDED

#include "TransferFunction.hpp"
#include "bonelab/Image.hpp"


namespace athabasca_recon
  {

  /** Base class to apply the ramp function to projections.
    *
    * This class is an abstract base class for convolving the input projection
    * rows with the ramp function kernel.  Concrete derived classes will
    * typically (although not necessarily) do this with FFTs, multiplying in
    * k-space.
    *
    * Derived classes that are implemented with FFTs should take care to
    * zero-pad the input in order to avoid wrap-around errors.
    *
    * Additionally, a smoothing filter may be specified, as there is no
    * additional cost (except set-up cost) to apply a smoothing filter
    * at this stage.
    *
    * Note that for multi-threading, an instance will be created in each
    * worker thread.  Implementors of derived classes must ensure thread-
    * safety in this scenario (not the FFT libraries typically have fairly
    * specific requirements for thread-safety).
    */
  template <typename TProjection>
  class ProjectionFilterer
    {
    public:
      
      typedef typename TProjection::value_type TValue;
      typedef typename TProjection::index_type TIndex;
      typedef typename TProjection::space_type TSpace;
      typedef bonelab::Array<1,TValue,TIndex> TFunction1D;

      /** Default constructor.
        */
      ProjectionFilterer()
        :
        m_PixelSpacing(1),
        m_Weight(1),
        m_SmoothingFilter(NULL)
        {}

      virtual ~ProjectionFilterer() {}

      /** Set/get the dimensions of the projections.
        * Must be set before calling Initialize.
        */
      void SetDimensions(bonelab::Tuple<2,TIndex> arg) {this->m_Dimensions = arg;}
      bonelab::Tuple<2,TIndex> GetDimensions() {return this->m_Dimensions;}

      /** Set/get the weight (scaling).
        *
        * The usual weight factor is delta theta, where delta theta is the
        * angular separation between projections (in radians).
        *
        * Must be set before calling Initialize.
        */
      void SetWeight(TValue weight) { this->m_Weight = weight; }
      TValue GetWeight() { return this->m_Weight; }

      /** Set/get the pixel spacing of the projections.
        * Must be set before calling Initialize.
        */
      void SetPixelSpacing(TSpace pixelSpacing) { this->m_PixelSpacing = pixelSpacing; }
      TSpace GetPixelSpacing() { return this->m_PixelSpacing; }

      /** Set a smoothing filter.
        *
        * Specified as a TransferFunction, which generates a concrete
        * numerical filter profile of a certain length on request.
        *
        * Must be called before Initialize.
        *
        * The default is NULL (i.e. no smoothing filter).
        */
      void SetSmoothingFilter(TransferFunction<TFunction1D>* arg) {this->m_SmoothingFilter = arg;}

      /** Initializes.
        *
        * Typically FFT libraries have some initialization functions that must
        * be called before performing any FFTs.
        * Also does memory allocation. (Note that memory is allocated freed
        * in the destructor.)
        *
        * This function must be called before the FilterProjection method
        * can be used.
        */
      virtual void Initialize() = 0;

      /** Apply the ramp filter (and optional smoothing filter) to the input
        * projection.
        *
        * This is not done in-place, because a change of type may be required
        * at this stage.
        */
      virtual void FilterProjection(const TProjection& in, TProjection& out) = 0;

    protected:

      bonelab::Tuple<2,TIndex> m_Dimensions;      
      TSpace m_PixelSpacing;
      TValue m_Weight;
      TransferFunction<TFunction1D>* m_SmoothingFilter;
      
    };  // class ProjectionFilterer

  } // namespace athabasca_recon

#endif
