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

namespace athabasca_recon
  {

  template <typename TProjectionIn, typename TProjectionOut>
  CTCalibration<TProjectionIn,TProjectionOut>::CTCalibration()
    :
    m_SensitivityLimit(0),
    m_IsCorrectedForPowerDecay(false)
    {}

  template <typename TProjectionIn, typename TProjectionOut>
  void CTCalibration<TProjectionIn,TProjectionOut>::LoadCalibrationFields()
    {
    athabasca_verbose_assert(!this->m_DarkFieldFileName.empty(), "No dark field defined");
    athabasca_verbose_assert(!this->m_BrightFieldFileName.empty(), "No bright field defined");
      {   // scope for reader : dark field
      std::cout << "Reading dark field data.\n";
      MetaImageReader<TIndex,TSpace> reader;
      reader.SetFileName(this->m_DarkFieldFileName);
      reader.GetInformation();
      if (reader.GetNDims() == 2)
        {
        this->ReadSingle(reader, this->m_DarkField);
        }
      else if (reader.GetNDims() == 3)
        {
        this->ReadStackAndAverage(reader, this->m_DarkField);
        std::cout << "Averaged stack of " << reader.GetDimSize()[0] << " fields.\n";
        }
      else
        { throw_athabasca_exception("Unexpected dimensions in dark field.\n"); }
      }

      {   // scope for reader : bright field
      std::cout << "Reading bright field data.\n";
      MetaImageReader<TIndex,TSpace> reader;
      reader.SetFileName(this->m_BrightFieldFileName);
      reader.GetInformation();
      if (reader.GetNDims() == 2)
        {
        this->ReadSingle(reader, this->m_FlatField);
        if (!this->m_BrightFieldAdvanceInterval)
          {this->m_BrightFieldAdvanceInterval = 1;}
        }
      else if (reader.GetNDims() == 3)
        {
        this->ReadStackAndAverage(reader, this->m_FlatField);
        std::cout << "Averaged stack of " << reader.GetDimSize()[0] << " fields.\n";
        if (!this->m_BrightFieldAdvanceInterval)
          {this->m_BrightFieldAdvanceInterval = (reader.GetDimSize()[0] + 1)/2.0;}
        }
      else
        { throw_athabasca_exception("Unexpected dimensions in bright field.\n"); }

      athabasca_verbose_assert(this->m_FlatField.dims() == this->m_DarkField.dims(),
                               "Bright field and dark field dimensions don't match.");
      for (size_t i=0; i < this->m_DarkField.size(); ++i)
        { this->m_FlatField[i] -= this->m_DarkField[i]; }
      }

    if (!this->m_PostScanBrightFieldFileName.empty())
      { 
      std::cout << "Reading post-scan bright field data.\n";
      MetaImageReader<TIndex,TSpace> reader;
      reader.SetFileName(this->m_PostScanBrightFieldFileName);
      reader.GetInformation();
      if (reader.GetNDims() == 2)
        {
        this->ReadSingle(reader, this->m_PostScanFlatField);
        if (!this->m_PostScanBrightFieldInterval)
          {this->m_PostScanBrightFieldInterval = 1;}
        }
      else if (reader.GetNDims() == 3)
        {
        this->ReadStackAndAverage(reader, this->m_PostScanFlatField);
        std::cout << "Averaged stack of " << reader.GetDimSize()[0] << " fields.\n";
        if (!this->m_PostScanBrightFieldInterval)
          {this->m_PostScanBrightFieldInterval = (reader.GetDimSize()[0] + 1)/2.0;}
        }
      else
        { throw_athabasca_exception("Unexpected dimensions in post-scan bright field.\n"); }

      athabasca_verbose_assert(this->m_PostScanFlatField.dims() == this->m_DarkField.dims(),
                               "Post-scan bright field and dark field dimensions don't match.");
      for (size_t i=0; i < this->m_DarkField.size(); ++i)
        { this->m_PostScanFlatField[i] -= this->m_DarkField[i]; }
      }

    }

  template <typename TProjectionIn, typename TProjectionOut>
  void CTCalibration<TProjectionIn,TProjectionOut>::ReadSingle
    (
    MetaImageReader<TIndex,TSpace>& reader,
    TProjectionOut& field
    )
    {
    TProjectionIn fieldIn;
    reader.GetData(fieldIn);
    if (field.is_constructed())
      { athabasca_assert(field.dims() == fieldIn.dims()); }
    else
      { field.construct(fieldIn.dims(), fieldIn.spacing(), fieldIn.origin()); }
    for (size_t i=0; i<fieldIn.size(); ++i)
      {
      field[i] = fieldIn[i];  // Type conversion
      }
    }

  template <typename TProjectionIn, typename TProjectionOut>
  void CTCalibration<TProjectionIn,TProjectionOut>::ReadStackAndAverage
    (
    MetaImageReader<TIndex,TSpace>& reader,
    TProjectionOut& field
    )
    {
    typedef bonelab::Image<3,TProjectionInValue,TIndex,TSpace> TProjectionInStack;
    TProjectionInStack projectionStack;
    reader.GetData(projectionStack);
    bonelab::Tuple<2,TIndex> dims(projectionStack.dims()[1], projectionStack.dims()[2]);
    bonelab::Tuple<2,TSpace> spacing(projectionStack.spacing()[1], projectionStack.spacing()[2]);
    bonelab::Tuple<2,TSpace> origin(projectionStack.origin()[1], projectionStack.origin()[2]);
    if (field.is_constructed())
      { athabasca_assert(field.dims() == dims); }
    else
      { field.construct(dims, spacing, origin); }
    field.zero();
    int N = projectionStack.dims()[0];
    TProjectionOutValue factor = 1.0/N;
    for (int p=0; p<N; ++p)
      {
      for (int i=0; i < dims[0]; ++i)
        {
        for (int j=0; j < dims[1]; ++j)
          {
          field(i,j) += factor*projectionStack(p,i,j);
          }
        }
      }
    }

  }  // namespace athabasca_recon
