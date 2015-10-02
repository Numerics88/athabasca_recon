#include "bonelab/Image.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class ImageTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (ImageTests, PointerConstructor)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Tuple<2,int> dims(2,3);
  Tuple<2,double> spacing(0.5,2.0);
  Tuple<2,double> origin(-10.0,5.0);
  Image<2,double,int,double> A(a, dims, spacing, origin);
  ASSERT_EQ(A.size(), 6);
  ASSERT_EQ(A.dims(), dims);
  ASSERT_EQ(A.spacing(), spacing);
  ASSERT_EQ(A.origin(), origin);
  ASSERT_EQ(A.ptr(), a);
  ASSERT_EQ(A(0,2), 3.0);
  ASSERT_EQ(A(1,2), 6.0);
  }

TEST_F (ImageTests, ReferenceConstructor)
  {
  double a[] = {1.0,2.0,3.0,4.0,5.0,6.0};
  Tuple<2,int> dims(2,3);
  Tuple<2,double> spacing(0.5,2.0);
  Tuple<2,double> origin(-10.0,5.0);
  Image<2,double,int,double> A(a, dims, spacing, origin);
  Image<2,double,int,double> B(A);
  ASSERT_EQ(B.size(), 6);
  ASSERT_EQ(B.dims(), dims);
  ASSERT_EQ(B.spacing(), spacing);
  ASSERT_EQ(B.origin(), origin);
  ASSERT_EQ(A.ptr(), B.ptr());
  ASSERT_EQ(B(0,2), 3.0);
  ASSERT_EQ(B(1,2), 6.0);
  }

TEST_F (ImageTests, Extents2)
  {
  Tuple<2,int> dims(3,4);
  Tuple<2,double> spacing(0.5,2.0);
  Tuple<2,double> origin(-10.0,5.0);
  Image<2,double,int,double> A(dims, spacing, origin);
  ASSERT_DOUBLE_EQ(A.exterior_extents()[0][0], origin[0] - 0.5*spacing[0]);
  ASSERT_DOUBLE_EQ(A.exterior_extents()[0][1], origin[0] + 2.5*spacing[0]);
  ASSERT_DOUBLE_EQ(A.exterior_extents()[1][0], origin[1] - 0.5*spacing[1]);
  ASSERT_DOUBLE_EQ(A.exterior_extents()[1][1], origin[1] + 3.5*spacing[1]);
  ASSERT_DOUBLE_EQ(A.interior_extents()[0][0], origin[0]);
  ASSERT_DOUBLE_EQ(A.interior_extents()[0][1], origin[0] + 2*spacing[0]);
  ASSERT_DOUBLE_EQ(A.interior_extents()[1][0], origin[1]);
  ASSERT_DOUBLE_EQ(A.interior_extents()[1][1], origin[1] + 3*spacing[1]);
  }
