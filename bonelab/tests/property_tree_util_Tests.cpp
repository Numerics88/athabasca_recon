#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include "bonelab/boost/property_tree/util.hpp"
#include <string>


// Create a test fixture class.
class PropertyTreeUtilTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (PropertyTreeUtilTests, MergeNoOverwrite)
  {
  // Start with value
  boost::property_tree::ptree pt;
  pt.put("Some String", std::string("42 and a bit"));
  pt.put<int>("A subtree.Some Integer", 42);
  pt.put<int>("A subtree.Some Other Integer", 43);
  boost::property_tree::ptree pt2;
  pt2.put("Some String", std::string("42 and a lot"));
  merge(pt2,pt);
  ASSERT_EQ(pt2.size(), 2);
  ASSERT_EQ(pt2.get<std::string>("Some String"), "42 and a lot");
  ASSERT_EQ(pt2.get<int>("A subtree.Some Integer"), 42);
  ASSERT_EQ(pt2.get<int>("A subtree.Some Other Integer"), 43);
  }

TEST_F (PropertyTreeUtilTests, MergeOverwrite)
  {
  // Start with value
  boost::property_tree::ptree pt;
  pt.put("Some String", std::string("42 and a bit"));
  pt.put<int>("A subtree.Some Integer", 42);
  pt.put<int>("A subtree.Some Other Integer", 43);
  boost::property_tree::ptree pt2;
  pt2.put("Some String", std::string("42 and a lot"));
  merge(pt2,pt,true);
  ASSERT_EQ(pt2.size(), 2);
  ASSERT_EQ(pt2.get<std::string>("Some String"), "42 and a bit");
  ASSERT_EQ(pt2.get<int>("A subtree.Some Integer"), 42);
  ASSERT_EQ(pt2.get<int>("A subtree.Some Other Integer"), 43);
  }
