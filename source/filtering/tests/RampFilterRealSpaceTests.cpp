#include "filtering/RampFilterRealSpace.hpp"
#include <gtest/gtest.h>
#include <boost/math/constants/constants.hpp>

using boost::math::constants::pi;
using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Array<1,TValue,TIndex> TArray;
typedef RampFilterRealSpace<TArray> TRampFilter;

// Create a test fixture class.
class RampFilterRealSpaceTests : public ::testing::Test
  {
  };

template <typename T> inline T sqr(T x) { return x*x; }

TValue oddTerm(int k, TValue spacing)
  {
  return TValue(-1)/(sqr(pi<TValue>()*k)*spacing);
  }

// --------------------------------------------------------------------
// test implementations

TEST_F (RampFilterRealSpaceTests, Defaults)
  {
  TRampFilter rampFilter;
  rampFilter.SetLength(16);
  rampFilter.SetSpacing(1);
  rampFilter.SetWeight(1);
  TArray A;
  rampFilter.ConstructRealSpaceFilter(A);
  ASSERT_FLOAT_EQ(A(0), 0.25);
  for (int i=1; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), oddTerm(i, 1));
    }
  for (int i=2; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), 0);
    }
  for (int i=1; i<8; i++)
    {
    ASSERT_FLOAT_EQ(A(16-i), A(i));
    }
  ASSERT_FLOAT_EQ(A(8), 0);
  }

TEST_F (RampFilterRealSpaceTests, WithWeight)
  {
  TRampFilter rampFilter;
  rampFilter.SetLength(16);
  rampFilter.SetSpacing(1);
  rampFilter.SetWeight(0.25);
  TArray A;
  rampFilter.ConstructRealSpaceFilter(A);
  ASSERT_FLOAT_EQ(A(0), 0.25*0.25);
  for (int i=1; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), 0.25*oddTerm(i, 1));
    }
  for (int i=2; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), 0);
    }
  for (int i=1; i<8; ++i)
    {
    ASSERT_FLOAT_EQ(A(16-i), A(i));
    }
  ASSERT_FLOAT_EQ(A(8), 0);
  }

TEST_F (RampFilterRealSpaceTests, WithSpacing)
  {
  TRampFilter rampFilter;
  rampFilter.SetLength(16);
  rampFilter.SetSpacing(0.25);
  rampFilter.SetWeight(1);
  TArray A;
  rampFilter.ConstructRealSpaceFilter(A);
  ASSERT_FLOAT_EQ(A(0), 0.25/0.25);
  for (int i=1; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), oddTerm(i, 1)/0.25);
    }
  for (int i=2; i<8; i+=2)
    {
    ASSERT_FLOAT_EQ(A(i), 0);
    }
  for (int i=1; i<8; ++i)
    {
    ASSERT_FLOAT_EQ(A(16-i), A(i));
    }
  ASSERT_FLOAT_EQ(A(8), 0);
  }
