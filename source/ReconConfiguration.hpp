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

#ifndef BONELAB_ReconConfiguration_hpp_INCLUDED
#define BONELAB_ReconConfiguration_hpp_INCLUDED

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <utility>
#include <string>

using boost::int64_t;

namespace athabasca_recon
  {

   /** A class to read, parse, store and write configuration data.
     *
     * This class contains a boost::property_tree::ptree object and largely
     * has a similar interface.  The boost ptree stores configuration data
     * as key/value pairs using the stream operators.  This is highly flexible,
     * as configuration parameters can be interpreted as different types as
     * required. Furthermore, values may themselves be ptrees, which leads to
     * a tree structure for the configuration.
     *
     * Note that it would be a misuse to store large amounts
     * of data, as it is neither particularly space-efficient or fast,
     * neither of which matter for configuration data.
     *
     * This class adds some knowledge of specific reconstruction
     * configuration to a generic ptree.  In particular, VerifyConfiguration is
     * invaluable for saving users from themselves.
     *
     * Only a single level of section nesting is used in Athabasca Recon.
     */
  class ReconConfiguration : private boost::noncopyable
    {
    public:
      
      void ReadProjectionsInfo();

      /** Read a configuration file in the conf format (see conf_parser.hpp).
        * This will also read any meta data from input data files
        * specified in the configuration file.
        * Some default values are added to the configuration file at this point,
        * although where possible that is deferred until VerifyConfiguration.
        */
      void ReadConfiguration(const char* config_file);

      /** Verify the configuration.
        * Ensures that all required values are present, and that they are
        * consistent.  Adds any required default values.
        */
      void VerifyConfiguration();

      /** Print the complete configuration to the specified stream.
        * The configuration is written in the conf format, which allows it
        * to be copied and pasted into a configuration file.
        */
      void ReportConfiguration(std::ostream& os);

      /** Utility function to return the memory value given by the specified
        * key.  There is a dedicated method for this, because the configuration
        * value can have different units (e.g. "512 MB", "2GB").  The units are
        * parsed and the returned value has units of bytes.
        */
      int64_t ParseMemoryValue(std::string key) const;

      /** Returns the value corresponding to path as type T.
        * path gives the complete nested key, with periods as delimiters
        * e.g. "SectionA.AnImportantValue".
        * Throws an exception if the specified key does not exist.
        */
      template<class T> T get(const char* path) const
        { return this->m_PropertyTree.get<T>(path); }

      /** Returns the value corresponding to path as type T.
        * path gives the complete nested key, with periods as delimiters
        * e.g. "SectionA.AnImportantValue".
        * If the key does not exist, then default_value is returned.
        */
      template<class T> T get(const char* path, const T& default_value) const
        { return this->m_PropertyTree.get<T>(path, default_value); }

      /** Returns the value corresponding to path as type T, wrapped in
        * boost::optional.
        * path gives the complete nested key, with periods as delimiters
        * e.g. "SectionA.AnImportantValue".
        * If the specified key does not exist, the returned value evaluates
        * to false; otherwise the configuration value can be obtained
        * with the value-of operator (*).
        */
      template<class T> boost::optional<T> get_optional(const char* path) const
        { return this->m_PropertyTree.get_optional<T>(path); }

    protected:   

      boost::property_tree::ptree m_PropertyTree;

      template <typename T> T RequireParameter(const char* key);

      template <typename T> T OptionalParameter (const char* key, T def);

      void CheckForUnknown(
        const boost::property_tree::ptree& section,
        const char* sectionKey,
        const char** validKeys,
        int numberOfValidKeys);
      
      void Die(std::string message) const;
      void Die(const boost::format& message) const;

    };  // class

  }  // namespace athabasca_recon

#endif
