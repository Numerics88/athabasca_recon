#include "bonelab/DoubleBuffer.hpp"
#include <gtest/gtest.h>

using namespace bonelab;

// Create a test fixture class.
class DoubleBufferTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (DoubleBufferTests, ArrayConstruction)
  {
  typedef Array<2,float,int> TArray;
  DoubleBuffer<TArray> db;
  db.construct(Tuple<2,int>(2,3));
  ASSERT_EQ(db.dims(), (Tuple<2,int>(2,3)));
  ASSERT_EQ(db.active().dims(), (Tuple<2,int>(2,3)));
  ASSERT_EQ(db.loading().dims(), (Tuple<2,int>(2,3)));
  ASSERT_NE(&(db.active()), &(db.loading()));
  }

TEST_F (DoubleBufferTests, ImageConstruction)
  {
  typedef Image<2,float,int,float> TImage;
  DoubleBuffer<TImage> db;
  Tuple<2,float> spacing(0.2, 0.4);
  Tuple<2,float> origin(-1,2.5);
  db.construct(Tuple<2,int>(2,3), spacing, origin);
  ASSERT_EQ(db.dims(), (Tuple<2,int>(2,3)));
  ASSERT_EQ(db.active().dims(), (Tuple<2,int>(2,3)));
  ASSERT_EQ(db.loading().dims(), (Tuple<2,int>(2,3)));
  ASSERT_EQ(db.spacing(), spacing);
  ASSERT_EQ(db.active().spacing(), spacing);
  ASSERT_EQ(db.loading().spacing(), spacing);
  ASSERT_EQ(db.origin(), origin);
  ASSERT_EQ(db.active().origin(), origin);
  ASSERT_EQ(db.loading().origin(), origin);
  ASSERT_NE(&(db.active()), &(db.loading()));
  }

TEST_F (DoubleBufferTests, Swap)
  {
  typedef Image<2,float,int,float> TImage;
  DoubleBuffer<TImage> db;
  Tuple<2,float> spacing(0.2, 0.4);
  Tuple<2,float> origin(-1,2.5);
  db.construct(Tuple<2,int>(2,3), spacing, origin);
  db.active()(1,2) = 10;
  db.active()(0,0) = 5;
  db.loading()(1,2) = 8;
  db.loading()(0,0) = 3;
  TImage* saveActivePtr = &(db.active());
  TImage* saveLoadingPtr = &(db.loading());
  // Swap and check that addresses of arrays has swapped.
  db.swap();
  ASSERT_EQ(&(db.loading()), saveActivePtr);
  ASSERT_EQ(&(db.active()), saveLoadingPtr);
  // Subsequent tests are kind of redundant.
  ASSERT_EQ(db.loading()(1,2), 10);
  ASSERT_EQ(db.loading()(0,0), 5);
  ASSERT_EQ(db.active()(1,2), 8);
  ASSERT_EQ(db.active()(0,0), 3);
  // Swap again
  db.swap();
  ASSERT_EQ(&(db.loading()), saveLoadingPtr);
  ASSERT_EQ(&(db.active()), saveActivePtr);
  // Subsequent tests are kind of redundant.
  ASSERT_EQ(db.active()(1,2), 10);
  ASSERT_EQ(db.active()(0,0), 5);
  ASSERT_EQ(db.loading()(1,2), 8);
  ASSERT_EQ(db.loading()(0,0), 3);
  }
