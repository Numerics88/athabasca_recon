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

/** Some utilites for dealing with templates of numerical types and specifying
  * numerical types.
  */

#ifndef BONELAB_types_HPP_INCLUDED
#define BONELAB_types_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <iostream>
#include <sstream>
#include <cstring>


namespace bonelab
  {

  typedef boost::int8_t int8_t;
  typedef boost::uint8_t uint8_t;
  typedef boost::int16_t int16_t;
  typedef boost::uint16_t uint16_t;
  typedef boost::int32_t int32_t;
  typedef boost::uint32_t uint32_t;
  typedef boost::int64_t int64_t;
  typedef boost::uint64_t uint64_t;

  /** Values enumerating numerical types. */
  enum DataType_t
    {
    BONELAB_INT8,
    BONELAB_UINT8,
    BONELAB_INT16,
    BONELAB_UINT16,
    BONELAB_INT32,
    BONELAB_UINT32,
    BONELAB_INT64,
    BONELAB_UINT64,
    BONELAB_FLOAT32,
    BONELAB_FLOAT64,
    BONELAB_NUMBER_OF_DataType
    };

  /** String tokens corresponding to DataType_t values. */
  const char* const DataType_s[] =
    {
    "INT8",
    "UINT8",
    "INT16",
    "UINT16",
    "INT32",
    "UINT32",
    "INT64",
    "UINT64",
    "FLOAT32",
    "FLOAT64"
    };

  /** Returns the string token of a DataType_t. */
  inline const char* DataTypeAsString(DataType_t arg)
    {
    if (arg >=0 && arg < BONELAB_NUMBER_OF_DataType)
      { return DataType_s[arg]; }
    else 
      { return NULL; }
    }

  /** Matches a string to tokens for DataType_t and returns the matching
    * DataType_t value.
    * Returns -1 if no match.
    */
  inline DataType_t DataTypeFromString(const char* s)
    {
    for (int i=0; i < static_cast<int>(BONELAB_NUMBER_OF_DataType); ++i)
      {
      if (strcmp(DataTypeAsString(static_cast<DataType_t>(i)), s) == 0)
        { return static_cast<DataType_t>(i); }
      }
    return static_cast<DataType_t>(-1);
    }

  template<typename T> inline DataType_t TemplatedDataType();
  template<> inline DataType_t TemplatedDataType<int8_t>() {return BONELAB_INT8;}
  template<> inline DataType_t TemplatedDataType<uint8_t>() {return BONELAB_UINT8;}
  template<> inline DataType_t TemplatedDataType<int16_t>() {return BONELAB_INT16;}
  template<> inline DataType_t TemplatedDataType<uint16_t>() {return BONELAB_UINT16;}
  template<> inline DataType_t TemplatedDataType<int32_t>() {return BONELAB_INT32;}
  template<> inline DataType_t TemplatedDataType<uint32_t>() {return BONELAB_UINT32;}
  template<> inline DataType_t TemplatedDataType<int64_t>() {return BONELAB_INT64;}
  template<> inline DataType_t TemplatedDataType<uint64_t>() {return BONELAB_UINT64;}
  template<> inline DataType_t TemplatedDataType<float>() {return BONELAB_FLOAT32;}
  template<> inline DataType_t TemplatedDataType<double>() {return BONELAB_FLOAT64;}

  // These stream operators are required for correct operation of boost::property_tree

  inline std::ostream& operator<<(std::ostream& s, DataType_t t)
    {
    s << DataTypeAsString(t);
    return s;
    }

  inline std::istream& operator>>(std::istream& s, DataType_t& t)
    {
    std::string str;
    s >> str;
    t = DataTypeFromString(str.c_str());
    if (t == static_cast<DataType_t>(-1))
      { s.setstate(std::ios::failbit); }
    return s;
    }

  }  // namespace bonelab

#endif
