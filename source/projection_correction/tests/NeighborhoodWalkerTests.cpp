#include "projection_correction/NeighborhoodWalker.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef NeighborhoodWalker<TProjection> TNeighborhoodWalker;

// Create a test fixture class.
class NeighborhoodWalkerTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (NeighborhoodWalkerTests, StartInterior)
  {
  bonelab::Tuple<2,TIndex> image_dims(12,12);
  bonelab::Tuple<2,TIndex> starting_point(6,6);
  TNeighborhoodWalker walker(image_dims, starting_point);
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,9)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,8)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,7)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,6)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(5,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(6,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(7,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(8,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(9,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(10,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(10,4)));
  }

TEST_F (NeighborhoodWalkerTests, StartNearCorner)
  {
  bonelab::Tuple<2,TIndex> image_dims(12,12);
  bonelab::Tuple<2,TIndex> starting_point(0,2);
  TNeighborhoodWalker walker(image_dims, starting_point);
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,2)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(0,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(0,1)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,1)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,1)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,2)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(0,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(0,0)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,0)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,0)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,0)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,1)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,2)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,3)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,4)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(3,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(2,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(1,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(0,5)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,0)));
  ASSERT_EQ(walker.GetNextLocation(), (bonelab::Tuple<2,TIndex>(4,1)));
  }
