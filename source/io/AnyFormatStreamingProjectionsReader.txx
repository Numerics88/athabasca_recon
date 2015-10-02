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

#define BOOST_FILESYSTEM_VERSION 3
#include "StreamingRawProjectionsReader.hpp"
#include "StreamingMetaImageProjectionsReader.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

using boost::algorithm::to_lower_copy;

namespace athabasca_recon
  {

  template <typename TProjection>
  AnyFormatStreamingProjectionsReader<TProjection>::AnyFormatStreamingProjectionsReader()
    :
    m_Worker(NULL)
    {}

  template <typename TProjection>
  AnyFormatStreamingProjectionsReader<TProjection>::~AnyFormatStreamingProjectionsReader()
    {
    delete(this->m_Worker);
    this->m_Worker = NULL;
    }

  template <typename TProjection>
  void AnyFormatStreamingProjectionsReader<TProjection>::Initialize()
    {
    athabasca_assert(!this->m_FileName.empty());
    athabasca_assert(!this->m_Worker);
    std::string extension = to_lower_copy(boost::filesystem::extension(this->m_FileName));
    if (extension == ".raw")
      { this->m_Worker = new StreamingRawProjectionsReader<TProjection>; }
    else if (extension == ".mhd")
      { this->m_Worker = new StreamingMetaImageProjectionsReader<TProjection>; }
    else
      { throw_athabasca_exception("Unsupported file type."); }
    this->m_Worker->SetFileName(this->m_FileName);
    this->m_Worker->SetDimensions(this->m_Dimensions);
    this->m_Worker->SetOrigin(this->m_Origin);
    this->m_Worker->SetPixelSize(this->m_PixelSize);
    this->m_Worker->SetNumberOfProjections(this->m_NumberOfProjections);
    this->m_Worker->SetProjectionStride(this->m_ProjectionStride);
    this->m_Worker->Initialize();
    }

  template <typename TProjection>
  void AnyFormatStreamingProjectionsReader<TProjection>::GetNextProjection(TProjection& image)
    {
    athabasca_assert(this->m_Worker);
    this->m_Worker->GetNextProjection(image);
    }

  template <typename TProjection>
  void AnyFormatStreamingProjectionsReader<TProjection>::Rewind()
    {
    athabasca_assert(this->m_Worker);
    this->m_Worker->Rewind();
    }

  template <typename TProjection>
  void AnyFormatStreamingProjectionsReader<TProjection>::CloseReadingProjections()
    {
    athabasca_assert(this->m_Worker);
    this->m_Worker->CloseReadingProjections();
    }

  } // namespace bonelab
