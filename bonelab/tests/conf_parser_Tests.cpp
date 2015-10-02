#include <gtest/gtest.h>
#include <boost/property_tree/ptree.hpp>
#include "../boost/property_tree/conf_parser.hpp"
#include <sstream>

// Create a test fixture class.
class ConfParserTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

const char test_string[] =
"Some String = 42 and a bit\n"
"\n"
"[A subtree]\n"
"Some Integer = 42\n"
"\n"
"[A few of my favourite things]\n"
"Raindrops = 2\n"
"Roses = red\n"
;

TEST_F (ConfParserTests, Write)
  {
  boost::property_tree::ptree pt;
  pt.put("Some String", std::string("42 and a bit"));
  pt.put<int>("A subtree.Some Integer", 42);
  pt.put<int>("A few of my favourite things.Raindrops", 2);
  pt.put<std::string>("A few of my favourite things.Roses", std::string("red"));
  std::ostringstream stream;
  write_conf(stream, pt);
  ASSERT_EQ(stream.str(), test_string);
  }

TEST_F (ConfParserTests, Read)
  {
  boost::property_tree::ptree pt;
  std::istringstream stream(test_string);
  read_conf(stream, pt);
  ASSERT_EQ(pt.size(), 3);
  ASSERT_EQ(pt.get<std::string>("Some String"), "42 and a bit");
  ASSERT_EQ(pt.get<int>("A subtree.Some Integer"), 42);
  ASSERT_EQ(pt.get<int>("A few of my favourite things.Raindrops"), 2);
  ASSERT_EQ(pt.get<std::string>("A few of my favourite things.Roses"), std::string("red"));
  }
