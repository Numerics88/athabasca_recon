#include "util/LinearFit.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef bonelab::Array<1,float,int> TArray;

// Create a test fixture class.
class LinearFitTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (LinearFitTests, TestTwoPoints)
  {
  TArray x(2);
  TArray y(2);
  x[0] = 2; y[0] = 1;
  x[1] = 3; y[1] = 2;
  LinearFit<TArray> fit(x,y);
  ASSERT_FLOAT_EQ(fit.GetConstantTerm(), -1.0);
  ASSERT_FLOAT_EQ(fit.GetLinearTerm(), 1.0);
  LinearFit<TArray> fit2(y,x);
  ASSERT_FLOAT_EQ(fit2.GetConstantTerm(), 1.0);
  ASSERT_FLOAT_EQ(fit2.GetLinearTerm(), 1.0);
  x[0] = 2; y[0] = 1;
  x[1] = 3; y[1] = 1.5;
  LinearFit<TArray> fit3(x,y);
  ASSERT_FLOAT_EQ(fit3.GetConstantTerm(), 0.0);
  ASSERT_FLOAT_EQ(fit3.GetLinearTerm(), 0.5);
  }

TEST_F (LinearFitTests, TestThreePoints)
  {
  TArray x(3);
  TArray y(3);
  x[0] = 2; y[0] = 1;
  x[1] = 3; y[1] = 2;
  x[2] = 4; y[2] = 3;
  LinearFit<TArray> fit(x,y);
  ASSERT_FLOAT_EQ(fit.GetConstantTerm(), -1.0);
  ASSERT_FLOAT_EQ(fit.GetLinearTerm(), 1.0);
  }

TEST_F (LinearFitTests, TestFourPoints)
  {
  TArray x(4);
  TArray y(4);
  x[0] = 2; y[0] = 1;
  x[1] = 3; y[1] = 1.8;
  x[2] = 3; y[2] = 2.2;
  x[3] = 4; y[3] = 3;
  LinearFit<TArray> fit(x,y);
  ASSERT_FLOAT_EQ(fit.GetConstantTerm(), -1.0);
  ASSERT_FLOAT_EQ(fit.GetLinearTerm(), 1.0);
  }
