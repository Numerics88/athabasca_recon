#ifndef BONELAB_property_tree_util_INCLUDED
#define BONELAB_property_tree_util_INCLUDED

#include <boost/property_tree/ptree.hpp>

inline void merge
  (
  boost::property_tree::ptree& a,
  const boost::property_tree::ptree& b,
  bool overwrite=false,
  std::string parent_keys=std::string()
  )
  {
  for (boost::property_tree::ptree::const_iterator it = b.begin(), end = b.end();
       it != end;
       ++it)
    {
    if (it->second.empty())
      {
      // Data value
      std::string key = parent_keys + it->first;
      std::string value = it->second.get_value<std::string>();
      bool exists = a.get_optional<std::string>(key);
      if (overwrite || !exists)
        { a.put(key, value); }
      }
    else
      {
      // Child ptree
      merge(a, it->second, overwrite, parent_keys + it->first + "." );
      }
    }
  }

#endif
