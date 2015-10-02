#include "bonelab/types.hpp"
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class TypesTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (TypesTests, Stream)
  {
  DataType_t dt;
  std::stringstream s;
  std::string str;

  dt = BONELAB_INT8;
  s << dt;
  s >> str;
  ASSERT_EQ(str, "INT8");
  s.clear();
  
  dt = BONELAB_FLOAT32;
  s << dt;
  s >> str;
  ASSERT_EQ(str, "FLOAT32");
  s.clear();
  
  s << "INT8";
  s >> dt;
  ASSERT_EQ (dt, BONELAB_INT8);
  s.clear();

  s << "FLOAT32";
  s >> dt;
  ASSERT_EQ (dt, BONELAB_FLOAT32);
  s.clear();

  s << "FLOATing down the river";
  s >> dt;
  ASSERT_TRUE (s.fail());
  s.clear();
  }
