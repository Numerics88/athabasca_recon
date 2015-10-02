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

namespace athabasca_recon
  {

  template <typename TProjection>
  void StreamingMetaImageProjectionsReader<TProjection>::Initialize()
    {
    MetaImageReader<TIndex,TSpace> meta_reader;
    meta_reader.SetFileName(this->m_FileName);
    meta_reader.GetInformation();
    this->m_ProjectionsFileName = meta_reader.GetRawFileName();
    this->m_BigEndian = meta_reader.IsBigEndian();
    StreamingRawProjectionsReader<TProjection>::Initialize();
    }

  }  // namespace athabasca_recon
