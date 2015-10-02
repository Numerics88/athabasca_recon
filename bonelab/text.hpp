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

#ifndef __bonelab_text_hpp
#define __bonelab_text_hpp

#include <vector>
#include <string>
#include <boost/tokenizer.hpp>

namespace bonelab
  {

  /** Splits a string into tokens.
    *
    * @param s  The input string.
    * @param tokens  The resulting identified tokens.
    * @param separators  A list of separator characters.
    */
  inline void split_arguments
    (
    const std::string& s,
    std::vector<std::string>& tokens,
    const char* separators = NULL
    )
    {
    if (separators == NULL)
      { separators = " \t,"; }
    boost::char_separator<char> sep(separators);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer_t;
    tokens.clear();
    tokenizer_t tok(s, sep);
    for (tokenizer_t::iterator t = tok.begin(); t != tok.end(); ++t)
      { tokens.push_back(*t); }
    }

  }   // namespace bonelab

#endif  // #ifndef __bonelab_text_hpp
