#include "back_projection/ParallelBackProjector_cpu.hpp"
#include "back_projection/PixelInterpolator.hpp"
#include "back_projection/RayTracer.hpp"
#include <gtest/gtest.h>
#include <boost/math/constants/constants.hpp>
#include <cmath>

using boost::math::constants::pi;

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef bonelab::Image<3,TValue,TIndex,TSpace> TVolume;
typedef BilinearPixelInterpolator<TProjection> TPixelInterpolator;
typedef ParallelRayTracer<TSpace> TRayTracer;
typedef ParallelBackProjector_cpu<
          TProjection,TVolume,TPixelInterpolator,TRayTracer> TBackProjector;

// Create a test fixture class.
class ParallelBackProjectorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

// Simple ray-tracing test with very small projection and single voxel volume,
// both centered.
TEST_F (ParallelBackProjectorTests, SingleVoxel)
  {
  bonelab::Tuple<3,TIndex> volDims(1,1,1);
  bonelab::Tuple<3,TSpace> volSpacing(0.1,0.1,0.1);
  bonelab::Tuple<3,TSpace> volOrigin(0,0,0);
  bonelab::Tuple<2,TIndex> projDims(3,3);
  bonelab::Tuple<2,TSpace> projSpacing(0.4,0.4);
  bonelab::Tuple<2,TSpace> projOrigin(-0.5*(projDims[0]-1)*projSpacing[0],
                                      -0.5*(projDims[1]-1)*projSpacing[1]);
  TValue projData[] = {1000,1000,1000,10,20,30,1000,1000,1000};
  TProjection projection(projData, projDims, projSpacing, projOrigin);
  TBackProjector backProjector;
  backProjector.AllocateVolume(volDims, volSpacing, volOrigin);
  TSpace angle = pi<TSpace>()/4;  // 45 degrees
  backProjector.ApplyProjection(projection, angle);
  TVolume volume(backProjector.GetVolume());
  // Ray trace back to center pixel: has value 20.
  ASSERT_FLOAT_EQ(volume(0,0,0), 20);
  angle = 0;
  backProjector.ApplyProjection(projection, angle);
  // Added the center pixel again, should now have 40.
  ASSERT_FLOAT_EQ(volume(0,0,0), 40);
  }

// As above, but now the projection is not centered.
TEST_F (ParallelBackProjectorTests, OffsetProjection)
  {
  bonelab::Tuple<3,TIndex> volDims(1,1,1);
  bonelab::Tuple<3,TSpace> volSpacing(0.1,0.1,0.1);
  bonelab::Tuple<3,TSpace> volOrigin(0,0,0);
  bonelab::Tuple<2,TIndex> projDims(3,3);
  bonelab::Tuple<2,TSpace> projSpacing(0.4,0.4);
  bonelab::Tuple<2,TSpace> projOrigin(-0.5*(projDims[0]-1)*projSpacing[0],
                                      -0.5*(projDims[1]-1)*projSpacing[1] + 0.5*projSpacing[1]);
  TValue projData[] = {1000,1000,1000,10,20,30,1000,1000,1000};
  TProjection projection(projData, projDims, projSpacing, projOrigin);
  TBackProjector backProjector;
  backProjector.AllocateVolume(volDims, volSpacing, volOrigin);
  TSpace angle = pi<TSpace>()/4;  // 45 degrees
  backProjector.ApplyProjection(projection, angle);
  TVolume volume(backProjector.GetVolume());
  // Ray-tracing back should be exactly between pixels with value 10 and 20.
  ASSERT_FLOAT_EQ(volume(0,0,0), 15);
  angle = 0;
  backProjector.ApplyProjection(projection, angle);
  // Angle doesn't matter at (0,0,0); ray-tracing should hit
  // with value 15 again - total is 30.
  ASSERT_FLOAT_EQ(volume(0,0,0), 30);
  }

// As above, but now volume is not centered.
TEST_F (ParallelBackProjectorTests, OffsetVolume)
  {
  bonelab::Tuple<3,TIndex> volDims(1,1,1);
  bonelab::Tuple<3,TSpace> volSpacing(0.1,0.1,0.1);
  bonelab::Tuple<3,TSpace> volOrigin(0,0.1,0);
  bonelab::Tuple<2,TIndex> projDims(3,3);
  bonelab::Tuple<2,TSpace> projSpacing(0.4,0.4);
  bonelab::Tuple<2,TSpace> projOrigin(-0.5*(projDims[0]-1)*projSpacing[0],
                                      -0.5*(projDims[1]-1)*projSpacing[1]);
  TValue projData[] = {1000,1000,1000,10,20,30,1000,1000,1000};
  TProjection projection(projData, projDims, projSpacing, projOrigin);
  TBackProjector backProjector;
  backProjector.AllocateVolume(volDims, volSpacing, volOrigin);
  TSpace angle = pi<TSpace>()/4;  // 45 degrees
  backProjector.ApplyProjection(projection, angle);
  TVolume volume(backProjector.GetVolume());
  // Hits at pixel with value 20, plus adjustment equivalent to 45 degrees
  // to shift of 0.1 .
  TValue expectedValue = 20 + 10*0.1*sqrt(0.5)/0.4;
  ASSERT_FLOAT_EQ(volume(0,0,0), expectedValue);
  angle = pi<TSpace>()/2;  // 90 degrees;
  backProjector.ApplyProjection(projection, angle);
  // 90 degrees is along the shift direction, so looks centered; add
  // center pixel value to result.
  ASSERT_FLOAT_EQ(volume(0,0,0), expectedValue + 20);
  }

// Test with 1x2x2 volume.
TEST_F (ParallelBackProjectorTests, TwoSquaredVoxels)
  {
  bonelab::Tuple<3,TIndex> volDims(1,2,2);
  bonelab::Tuple<3,TSpace> volSpacing(0.1,0.1,0.1);
  bonelab::Tuple<3,TSpace> volOrigin(-0.5*(volDims[0]-1)*volSpacing[0],
                                      -0.5*(volDims[1]-1)*volSpacing[1],
                                      -0.5*(volDims[2]-1)*volSpacing[2]);
  bonelab::Tuple<2,TIndex> projDims(3,3);
  bonelab::Tuple<2,TSpace> projSpacing(0.4,0.4);
  bonelab::Tuple<2,TSpace> projOrigin(-0.5*(projDims[0]-1)*projSpacing[0],
                                      -0.5*(projDims[1]-1)*projSpacing[1]);
  TValue projData[] = {1000,1000,1000,10,20,30,1000,1000,1000};
  TProjection projection(projData, projDims, projSpacing, projOrigin);
  TBackProjector backProjector;
  backProjector.AllocateVolume(volDims, volSpacing, volOrigin);
  TSpace angle = 0;
  backProjector.ApplyProjection(projection, angle);
  TVolume volume(backProjector.GetVolume());
  TValue expected1 = 10.0+((0.4-0.05)/0.4)*10.0;
  TValue expected2 = 20.0+(0.05/0.4)*10.0;
  ASSERT_FLOAT_EQ(volume(0,0,0), expected1);
  ASSERT_FLOAT_EQ(volume(0,0,1), expected1);
  ASSERT_FLOAT_EQ(volume(0,1,0), expected2);
  ASSERT_FLOAT_EQ(volume(0,1,1), expected2);
  angle = pi<TSpace>()/2;  // 90 degrees
  backProjector.ApplyProjection(projection, angle);
  ASSERT_FLOAT_EQ(volume(0,0,0), expected1 + expected2);
  ASSERT_FLOAT_EQ(volume(0,0,1), 2*expected1);
  ASSERT_FLOAT_EQ(volume(0,1,0), 2*expected2);
  ASSERT_FLOAT_EQ(volume(0,1,1), expected2 + expected1);
  }
