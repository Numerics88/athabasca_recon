#include "bonelab/endian.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class EndianTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (EndianTests, SwapShort)
  {
  short int x = 0x3B12;
  ASSERT_EQ ((SwapBytes<short int,sizeof(short int)>(x)), (short int)(0x123B));
  }

TEST_F (EndianTests, SwapInt)
  {
  int x = 0x3B12C821;
  ASSERT_EQ ((SwapBytes<int,sizeof(int)>(x)), int(0x21C8123B));
  }

TEST_F (EndianTests, LittleToLittle)
  {
  int x = 0x3B12C821;
  ASSERT_EQ ((EndianConversion<LITTLE_ENDIAN_ORDER,LITTLE_ENDIAN_ORDER,int>(x)), int(0x3B12C821));
  }

TEST_F (EndianTests, LittleToBig)
  {
  int x = 0x3B12C821;
  ASSERT_EQ ((EndianConversion<LITTLE_ENDIAN_ORDER,BIG_ENDIAN_ORDER,int>(x)), int(0x21C8123B));
  }

TEST_F (EndianTests, BigToLittle)
  {
  int x = 0x3B12C821;
  ASSERT_EQ ((EndianConversion<BIG_ENDIAN_ORDER,LITTLE_ENDIAN_ORDER,int>(x)), int(0x21C8123B));
  }

TEST_F (EndianTests, BigToBig)
  {
  int x = 0x3B12C821;
  ASSERT_EQ ((EndianConversion<BIG_ENDIAN_ORDER,BIG_ENDIAN_ORDER,int>(x)), int(0x3B12C821));
  }

TEST_F (EndianTests, HostOrder)
  {
  int x = 0x3B12C821;
  ASSERT_NE ((EndianConversion<HOST_ENDIAN_ORDER,BIG_ENDIAN_ORDER,int>(x)),
             (EndianConversion<HOST_ENDIAN_ORDER,LITTLE_ENDIAN_ORDER,int>(x)));
  }

TEST_F (EndianTests, SwappingPointer)
  {
  char blurb[] = "Mary had a little lamb";
  EndianConversion<LITTLE_ENDIAN_ORDER,BIG_ENDIAN_ORDER,int>(reinterpret_cast<int*>(blurb),2);
  ASSERT_EQ (std::string("yraMdah  a little lamb"), blurb);
  }

TEST_F (EndianTests, NonswappingPointer)
  {
  char blurb[] = "Mary had a little lamb";
  EndianConversion<LITTLE_ENDIAN_ORDER,LITTLE_ENDIAN_ORDER,int>(reinterpret_cast<int*>(blurb),2);
  ASSERT_EQ (std::string("Mary had a little lamb"), blurb);
  }
