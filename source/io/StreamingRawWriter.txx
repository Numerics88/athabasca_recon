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
#include "bonelab/safe_stream_write.hpp"

namespace athabasca_recon
  {

  template <typename TSlice>
  void StreamingRawWriter<TSlice>::Initialize()
    {
    athabasca_assert(!this->m_DataFileName.empty());
    this->m_NumberOfSlices = 0;
    this->m_DataFileStream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
    try
      {
      this->m_DataFileStream.open(this->m_DataFileName.c_str(), std::ios::binary);
      }
    catch (...)
      { throw_athabasca_exception((format("Error opening file %s") % this->m_DataFileName).str()); }
    }

  template <typename TSlice>
  void StreamingRawWriter<TSlice>::WriteSlice(TSlice& image)
    {
    athabasca_assert(image.is_constructed());
    athabasca_assert(this->m_DataFileStream.is_open());
    athabasca_assert(image.size() == long_product(this->m_SliceDimensions));
    try
      {
      safe_stream_write(this->m_DataFileStream, image.ptr(), image.size()*sizeof(TValue));
      }
    catch (...)
      { throw_athabasca_exception((format("Error writing to file %s") % this->m_DataFileName).str()); }
    ++this->m_NumberOfSlices;
    }

  template <typename TSlice>
  void StreamingRawWriter<TSlice>::WriteStack(TStack& stack)
    {
    athabasca_assert(stack.is_constructed());
    athabasca_assert(this->m_DataFileStream.is_open());
    size_t sliceSize = long_product(this->m_SliceDimensions);
    athabasca_assert(stack.size() % sliceSize == 0);
    TIndex count = TIndex(stack.size() / sliceSize);
    try
      {
      safe_stream_write(this->m_DataFileStream, stack.ptr(), stack.size()*sizeof(TValue));
      }
    catch (...)
      { throw_athabasca_exception((format("Error writing to file %s") % this->m_DataFileName).str()); }
    this->m_NumberOfSlices += count;
    }

  template <typename TSlice>
  void StreamingRawWriter<TSlice>::Close()
    {
    this->m_DataFileStream.close();
    }

  }  // namespace athabasca_recon
