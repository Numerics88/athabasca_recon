#include "bonelab/FFT_util.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class FFTUtilTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (FFTUtilTests, NextPowerOfTwo)
  {
  ASSERT_EQ(FFT_util::NextPowerOfTwo(1) , 0);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(2) , 1);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(3) , 2);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(4) , 2);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(5) , 3);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(6) , 3);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(8) , 3);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(9) , 4);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(10) , 4);
  ASSERT_EQ(FFT_util::NextPowerOfTwo(16) , 4);
}

TEST_F (FFTUtilTests, NextNumberWithFactors2And3)
  {
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(1) , 1);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(2) , 2);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(3) , 3);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(4) , 4);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(5) , 6);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(6) , 6);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(7) , 8);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(8) , 8);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(9) , 9);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(10) , 12);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(11) , 12);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(12) , 12);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(13) , 16);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(14) , 16);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(15) , 16);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(16) , 16);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(17) , 18);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(18) , 18);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(19) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(20) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(21) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(22) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(23) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(24) , 24);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(25) , 27);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(26) , 27);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(27) , 27);
  ASSERT_EQ(FFT_util::NextNumberWithFactors2And3(28) , 32);
  }
