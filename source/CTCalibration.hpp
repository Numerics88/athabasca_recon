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

#ifndef BONELAB_CTCalibration_HPP_INCLUDED
#define BONELAB_CTCalibration_HPP_INCLUDED

#include "io/MetaImageReader.hpp"
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

namespace athabasca_recon
  {

  /** A class to store calibration data related to the scan.
    *
    * This will include things such as dark field and bright field, and
    * optionally other parameters such as the measurement times of these
    * fields.
    * 
    * Depending on the file format, either the dark and bright fields
    * are stored together with the projections data, or else they
    * are stored in separate files.  Thus typically either ProjectionsFileName
    * or DarkFieldFileName and BrightFieldFileName are set, but not both.
    *
    * Although a bright field is mandatory, a dark field ought to be
    * optional.  At present, I don't believe this to be the case however.
    *
    * Note that if multiple dark or bright fields are available, they will
    * be averaged together.
    */
  template <typename TProjectionIn, typename TProjectionOut>
  class CTCalibration : private boost::noncopyable
    {
    public:
      
      typedef typename TProjectionIn::value_type TProjectionInValue;
      typedef typename TProjectionOut::value_type TProjectionOutValue;
      typedef typename TProjectionIn::index_type TIndex;
      typedef typename TProjectionIn::space_type TSpace;
      
      CTCalibration();

      /** Set/get the Projections file name.
        * Only required if the dark/bright field are stored together in the
        * same file as the projections.
        */
      void SetProjectionsFileName(std::string fn) {this->m_ProjectionsFileName = fn;}
      std::string GetProjectionsFileName() const {return this->m_ProjectionsFileName;}

      /** Set/get the dark field file name. */
      void SetDarkFieldFileName(std::string fn) {this->m_DarkFieldFileName = fn;}
      std::string GetDarkFieldFileName() const {return this->m_DarkFieldFileName;}

      /** Set/get the bright field file name. */
      void SetBrightFieldFileName(std::string fn) {this->m_BrightFieldFileName = fn;}
      std::string GetBrightFieldFileName() const {return this->m_BrightFieldFileName;}

      /** Set/get the post-scan bright field file name.
        * At present a post-scan bright field is only required if doing
        * beam power correction based on before and after bright fields.
        * Otherwise not required.
        */
      void SetPostScanBrightFieldFileName(std::string fn) {this->m_PostScanBrightFieldFileName = fn;}
      std::string GetPostScanBrightFieldFileName() const {return this->m_PostScanBrightFieldFileName;}

      /** Set/get the bright field advance interval.
        * This is the "time" elapsed between obtaining the bright field and
        * obtaining the first projection.
        *
        * "time" may be in real time units,
        * or it may also be in units of projection index (thus, 0 for the instant
        * of first projection, 1 for the instant of the second, etc...)
        * provided time is defined constantly everywhere.
        *
        * Defaults to 1 if there is a single bright field.  If there are
        * multiple dark fields, defaults to (N+1)/2, where N is the number
        * of bright fields.  Note that this value (for one or multiple) is
        * certainly an under-estimate, since a finite amount of time is
        * required to move the sample into the beam.
        */
      void SetBrightFieldAdvanceInterval(double t) {this->m_BrightFieldAdvanceInterval = t;}
      double GetBrightFieldAdvanceInterval()
        {return this->m_BrightFieldAdvanceInterval ? *(this->m_BrightFieldAdvanceInterval) : 0;}

      /** Set/get the bright field advance interval.
        * This is the "time" elapsed between obtaining the last projection and
        * obtaining the post-scan bright field.
        *
        *"time" may be in real time units,
        * or it may also be in units of projection index (thus, 0 for the instant
        * of first projection, 1 for the instant of the second, etc...)
        * provided time is defined constantly everywhere.
        *
        * Defaults to 1 if there is a single bright field.  If there are
        * multiple dark fields, defaults to (N+1)/2, where N is the number
        * of bright fields.  Note that this value (for one or multiple) is
        * certainly an under-estimate, since a finite amount of time is
        * required to move the sample into the beam.
        */
      void SetPostScanBrightFieldInterval(double t) {this->m_PostScanBrightFieldInterval = t;}
      double GetPostScanBrightFieldInterval()
        {return this->m_PostScanBrightFieldInterval ? *(this->m_PostScanBrightFieldInterval) : 0;}

      /** Set/get the sensitivity limit.
        * The sensitivity limit is a floor for pixel values, which is equivalent
        * to a ceiling for attenuation values.  This doesn't correct for
        * the problem of extinction, but merely prevents things for getting
        * ridiculous when a few pixels experience extinction (e.g. near
        * infinite values for attenuation as the signal drops to the noise
        * level).  Practically, a suitably chosen sensitivity limit
        * can sometimes limit corruption of the reconstructed image due to
        * extinction to a smaller part of the image.
        *
        * The default is 0.
        */
      void SetSensitivityLimit(TProjectionInValue arg) {this->m_SensitivityLimit = arg;}
      TProjectionInValue GetSensitivityLimit() const {return m_SensitivityLimit;}

      /** Read the dark and bright fields from the data files.
        * You must previously have set the appropriate file names.
        */
      void LoadCalibrationFields();

      /** Returns a constant reference to the dark field. */
      const TProjectionOut& GetDarkField() const {return this->m_DarkField;}

      /** Returns a constant reference to the bright field. */
      const TProjectionOut& GetFlatField() const {return this->m_FlatField;}

      /** Returns a constant reference to the post scan bright field. */
      const TProjectionOut& GetPostScanFlatField() const {return this->m_PostScanFlatField;}

      /** Returns a reference to the dark field.
        * Non-constant version.  Should not be used in worker threads.
        */
      TProjectionOut& GetDarkFieldNonConst() {return this->m_DarkField;}

      /** Returns a reference to the bright field.
        * Non-constant version.  Should not be used in worker threads.
        */
      TProjectionOut& GetFlatFieldNonConst() {return this->m_FlatField;}

      /** Returns a reference to the post-scan bright field.
        * Non-constant version.  Should not be used in worker threads.
        */
      TProjectionOut& GetPostScanFlatFieldNonConst() {return this->m_PostScanFlatField;}

    protected:

      std::string m_ProjectionsFileName;
      std::string m_DarkFieldFileName;
      std::string m_BrightFieldFileName;
      std::string m_PostScanBrightFieldFileName;
      boost::optional<double> m_BrightFieldAdvanceInterval;  // as number of projection measurement intervals
      boost::optional<double> m_PostScanBrightFieldInterval;
      bool m_IsCorrectedForPowerDecay;
      TProjectionOut m_DarkField;
      TProjectionOut m_FlatField;
      TProjectionOut m_PostScanFlatField;
      TProjectionInValue m_SensitivityLimit;

      void ReadSingle(MetaImageReader<TIndex,TSpace>& reader,
                      TProjectionOut& field);
      void ReadStackAndAverage(MetaImageReader<TIndex,TSpace>& reader,
                               TProjectionOut& field);

    };  // class CTCalibration

  } // namespace athabasca_recon

// Template definitions so we don't require explicit instantiations
#include "CTCalibration.txx"

#endif
