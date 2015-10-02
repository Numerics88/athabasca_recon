#include "filtering/RampFilterRealSpace.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef bonelab::Array<1,TValue,TIndex> TArray;
typedef RampFilterRealSpace<TArray> TRampFilter;

// Create a test fixture class.
class PROJECTION_FILTERER_TESTS : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (PROJECTION_FILTERER_TESTS, Null)
  {
  // Create all-zero input projection; output must also be all zeros.
  bonelab::Tuple<2,TIndex> dims(2,32);
  bonelab::Tuple<2,TSpace> spacing(1,1);
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TProjectionFilterer filterer;
  filterer.SetDimensions(dims);
  filterer.SetWeight(1);
  filterer.SetPixelSpacing(1);
  filterer.Initialize();
  TProjection projIn(dims, spacing, origin);
  TProjection projOut(dims, spacing, origin);
  projIn.zero();
  filterer.FilterProjection(projIn, projOut);
  for (size_t i=0; i<projOut.size(); ++i)
    {
    ASSERT_FLOAT_EQ(projOut[i], 0);
    }
  }

TEST_F (PROJECTION_FILTERER_TESTS, DeltaFunction)
  {
  // Input a delta function.
  bonelab::Tuple<2,TIndex> dims(2,32);
  bonelab::Tuple<2,TSpace> spacing(1,1);
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TProjectionFilterer filterer;
  filterer.SetDimensions(dims);
  filterer.SetWeight(1);
  filterer.SetPixelSpacing(spacing[1]);
  filterer.Initialize();
  TProjection projIn(dims, spacing, origin);
  TProjection projOut(dims, spacing, origin);
  projIn.zero();
  projIn(0,5) = 1;   // One pixel in first row gets value 1.
  filterer.FilterProjection(projIn, projOut);
  TRampFilter rampFilterGenerator;
  rampFilterGenerator.SetLength(64);  // Needs to be at least twice the length, as is designed as wrap-around function
  rampFilterGenerator.SetSpacing(spacing[1]);
  rampFilterGenerator.SetWeight(1);
  TArray rampFilter;
  rampFilterGenerator.ConstructRealSpaceFilter(rampFilter);
  // The output must be the ramp filter real-space kernel; verify this.
  for (int i=0; i<dims[1]; ++i)
    {
    int relativeIndex = abs(5-i);
    // std::cout << "i: " << i << " " << projOut(0,i) << " " << rampFilter(relativeIndex) << "\n";
    ASSERT_NEAR(projOut(0,i), rampFilter(relativeIndex), 1E-7);
    }
  // Nothing in the second row.
  for (int i=0; i<dims[1]; ++i)
    {
    ASSERT_FLOAT_EQ(projOut(1,i), 0);
    }
  }

TEST_F (PROJECTION_FILTERER_TESTS, DoubleDeltaFunction)
  {
  // Input the sum of two delta-functions
  bonelab::Tuple<2,TIndex> dims(2,32);
  bonelab::Tuple<2,TSpace> spacing(1,1);
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TProjectionFilterer filterer;
  filterer.SetDimensions(dims);
  filterer.SetWeight(1);
  filterer.SetPixelSpacing(spacing[1]);
  filterer.Initialize();
  TProjection projIn(dims, spacing, origin);
  TProjection projOut(dims, spacing, origin);
  projIn.zero();
  projIn(0,5) = 1;
  projIn(0,6) = 0.5;
  filterer.FilterProjection(projIn, projOut);
  TRampFilter rampFilterGenerator;
  rampFilterGenerator.SetLength(64);  // Needs to be at least twice the length, as is designed as wrap-around function
  rampFilterGenerator.SetSpacing(spacing[1]);
  rampFilterGenerator.SetWeight(1);
  TArray rampFilter;
  rampFilterGenerator.ConstructRealSpaceFilter(rampFilter);
  // The output must be the sum of two ramp filter real-space kernel; verify this.
  for (int i=0; i<dims[1]; ++i)
    {
    int relativeIndex1 = abs(5-i);
    int relativeIndex2 = abs(6-i);
    // std::cout << "i: " << i << " " << projOut(0,i) << " " << rampFilter(relativeIndex) << "\n";
    ASSERT_NEAR(projOut(0,i), rampFilter(relativeIndex1) + 0.5*rampFilter(relativeIndex2), 1E-7);
    }
  for (int i=0; i<dims[1]; ++i)
    {
    ASSERT_FLOAT_EQ(projOut(1,i), 0);
    }
  }

// Basic test, to make sure setting the weight scales the output.
TEST_F (PROJECTION_FILTERER_TESTS, Weight)
  {
  bonelab::Tuple<2,TIndex> dims(2,32);
  bonelab::Tuple<2,TSpace> spacing(1,1);
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TProjectionFilterer filterer;
  filterer.SetDimensions(dims);
  filterer.SetWeight(3);
  filterer.SetPixelSpacing(spacing[1]);
  filterer.Initialize();
  TProjection projIn(dims, spacing, origin);
  TProjection projOut(dims, spacing, origin);
  projIn.zero();
  projIn(0,5) = 1;
  filterer.FilterProjection(projIn, projOut);
  TRampFilter rampFilterGenerator;
  rampFilterGenerator.SetLength(64);  // Needs to be at least twice the length, as is designed as wrap-around function
  rampFilterGenerator.SetSpacing(spacing[1]);
  rampFilterGenerator.SetWeight(1);
  TArray rampFilter;
  rampFilterGenerator.ConstructRealSpaceFilter(rampFilter);
  for (int i=0; i<dims[1]; ++i)
    {
    int relativeIndex = abs(5-i);
    // std::cout << "i: " << i << " " << projOut(0,i) << " " << rampFilter(relativeIndex) << "\n";
    ASSERT_NEAR(projOut(0,i), 3*rampFilter(relativeIndex), 1E-7);
    }
  for (int i=0; i<dims[1]; ++i)
    {
    ASSERT_FLOAT_EQ(projOut(1,i), 0);
    }
  }

// Basic test, to make sure setting pixel spacing has the expected effect.
// In fact, setting pixel spacing should have no effect on the result.
TEST_F (PROJECTION_FILTERER_TESTS, PixelSpacing)
  {
  bonelab::Tuple<2,TIndex> dims(2,32);
  bonelab::Tuple<2,TSpace> spacing(0.2,0.2);
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TProjectionFilterer filterer;
  filterer.SetDimensions(dims);
  filterer.SetWeight(1);
  filterer.SetPixelSpacing(spacing[1]);
  filterer.Initialize();
  TProjection projIn(dims, spacing, origin);
  TProjection projOut(dims, spacing, origin);
  projIn.zero();
  projIn(0,5) = 1;
  filterer.FilterProjection(projIn, projOut);
  TRampFilter rampFilterGenerator;
  rampFilterGenerator.SetLength(64);  // Needs to be at least twice the length, as is designed as wrap-around function
  rampFilterGenerator.SetSpacing(spacing[1]);
  rampFilterGenerator.SetWeight(1);
  TArray rampFilter;
  rampFilterGenerator.ConstructRealSpaceFilter(rampFilter);
  for (int i=0; i<dims[1]; ++i)
    {
    int relativeIndex = abs(5-i);
    // std::cout << "i: " << i << " " << projOut(0,i) << " " << rampFilter(relativeIndex) << "\n";
    ASSERT_NEAR(projOut(0,i), rampFilter(relativeIndex), 1E-7);
    }
  for (int i=0; i<dims[1]; ++i)
    {
    ASSERT_FLOAT_EQ(projOut(1,i), 0);
    }
  }
