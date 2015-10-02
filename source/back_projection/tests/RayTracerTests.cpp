#include "back_projection/RayTracer.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TSpace;
typedef ParallelRayTracer<TSpace> TRayTracer;

// Create a test fixture class.
class RayTracerTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (RayTracerTests, AtCenter1)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,0);
  bonelab::Tuple<2,TSpace> normal(0,1);  // along x
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }

TEST_F (RayTracerTests, AtCenter2)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,0);
  bonelab::Tuple<2,TSpace> normal(1,0); // along y
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }

TEST_F (RayTracerTests, AtCenter3)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,0);
  bonelab::Tuple<2,TSpace> normal(1/sqrt(TSpace(2)),1/sqrt(TSpace(2)));
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }

TEST_F (RayTracerTests, OnXAxis1)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,1);
  bonelab::Tuple<2,TSpace> normal(0,1);  // along x
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }

TEST_F (RayTracerTests, OnXAxis2)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,1);
  bonelab::Tuple<2,TSpace> normal(1,0); // along y
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], -1);
  }

TEST_F (RayTracerTests, OnXAxis3)
  {
  bonelab::Tuple<3,TSpace> position(1.5,0,1);
  bonelab::Tuple<2,TSpace> normal(1/sqrt(TSpace(2)),1/sqrt(TSpace(2)));
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], -1/sqrt(TSpace(2)));
  }

TEST_F (RayTracerTests, OnYAxis1)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,0);
  bonelab::Tuple<2,TSpace> normal(0,1);  // along x
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 1);
  }

TEST_F (RayTracerTests, OnYAxis2)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,0);
  bonelab::Tuple<2,TSpace> normal(1,0); // along y
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }

TEST_F (RayTracerTests, OnYAxis3)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,0);
  bonelab::Tuple<2,TSpace> normal(1/sqrt(TSpace(2)),1/sqrt(TSpace(2)));
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 1/sqrt(TSpace(2)));
  }

TEST_F (RayTracerTests, AtOneOne1)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,1);
  bonelab::Tuple<2,TSpace> normal(0,1);  // along x
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 1);
  }

TEST_F (RayTracerTests, AtOneOne2)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,1);
  bonelab::Tuple<2,TSpace> normal(1,0); // along y
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], -1);
  }

TEST_F (RayTracerTests, AtOneOne3)
  {
  bonelab::Tuple<3,TSpace> position(1.5,1,1);
  bonelab::Tuple<2,TSpace> normal(1/sqrt(TSpace(2)),1/sqrt(TSpace(2)));
  bonelab::Tuple<2,TSpace> reverse_normal(-normal[0],normal[1]);
  bonelab::Tuple<2,TSpace> intersection = TRayTracer::Intersect(position, reverse_normal);
  ASSERT_FLOAT_EQ(intersection[0], 1.5);
  ASSERT_FLOAT_EQ(intersection[1], 0);
  }
