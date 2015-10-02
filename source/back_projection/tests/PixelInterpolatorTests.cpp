#include "back_projection/PixelInterpolator.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef bonelab::Image<2,float,int,float> TImage;

// Utility functions for evaluating results.

bool compare_nearest(const TImage& image, float y, float x, float val)
  {
  float ival = NearestNeighborPixelInterpolator<TImage>::Interpolate(
    image, bonelab::Tuple<2,float>(y,x));
  // std::cout << ival << " " << val << '\n';
  return (fabs(ival - val) < 1E-7);
  }

bool compare_bilinear(const TImage& image, float y, float x, float val)
  {
  float ival = BilinearPixelInterpolator<TImage>::Interpolate(
    image, bonelab::Tuple<2,float>(y,x));
  // std::cout << ival << " " << val << '\n';
  return (fabs(ival - val) < 1E-7);
  }

bool compare_bilinear_with_fallback(const TImage& image, float y, float x, float val)
  {
  float ival = BilinearPixelInterpolatorWithFallback<TImage>::Interpolate(
    image, bonelab::Tuple<2,float>(y,x));
  // std::cout << ival << " " << val << '\n';
  return (fabs(ival - val) < 1E-5);
  }

// Create a test fixture class.
class PixelInterpolatorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (PixelInterpolatorTests, TestNearest)
  {
  bonelab::Tuple<2,int> dims(4,4);
  bonelab::Tuple<2,float> origin(-1.5,1.75);
  bonelab::Tuple<2,float> spacing(0.5,0.25);
  TImage image(dims, spacing, origin);
  image(1,1) = 5.0;
  image(1,2) = 6.0;
  image(2,1) = 7.0;
  image(2,2) = 8.0;
  ASSERT_TRUE(compare_nearest(image, 0, 0, 0));
  ASSERT_TRUE(compare_nearest(image, 0, -10, 0));
  ASSERT_TRUE(compare_nearest(image, 0, 10, 0));
  ASSERT_TRUE(compare_nearest(image, -10, 0, 0));
  ASSERT_TRUE(compare_nearest(image, 10, 0, 0));

  ASSERT_TRUE(compare_nearest(image, -1.0, 2.0, 5.0));
  ASSERT_TRUE(compare_nearest(image, -1.01, 2.0, 5.0));
  ASSERT_TRUE(compare_nearest(image, -0.99, 2.0, 5.0));
  ASSERT_TRUE(compare_nearest(image, -1.0, 2.01, 5.0));
  ASSERT_TRUE(compare_nearest(image, -1.0, 1.99, 5.0));

  ASSERT_TRUE(compare_nearest(image, -1.0, 2.25, 6.0));
  ASSERT_TRUE(compare_nearest(image, -1.01, 2.25, 6.0));
  ASSERT_TRUE(compare_nearest(image, -0.99, 2.25, 6.0));
  ASSERT_TRUE(compare_nearest(image, -1.0, 2.26, 6.0));
  ASSERT_TRUE(compare_nearest(image, -1.0, 2.24, 6.0));

  ASSERT_TRUE(compare_nearest(image, -0.5, 2.0, 7.0));
  ASSERT_TRUE(compare_nearest(image, -0.51, 2.0, 7.0));
  ASSERT_TRUE(compare_nearest(image, -0.49, 2.0, 7.0));
  ASSERT_TRUE(compare_nearest(image, -0.5, 2.01, 7.0));
  ASSERT_TRUE(compare_nearest(image, -0.5, 1.99, 7.0));

  ASSERT_TRUE(compare_nearest(image, -0.5, 2.25, 8.0));
  ASSERT_TRUE(compare_nearest(image, -0.51, 2.25, 8.0));
  ASSERT_TRUE(compare_nearest(image, -0.49, 2.25, 8.0));
  ASSERT_TRUE(compare_nearest(image, -0.5, 2.24, 8.0));
  ASSERT_TRUE(compare_nearest(image, -0.5, 2.26, 8.0));
  }

TEST_F (PixelInterpolatorTests, TestBilinear)
  {
  bonelab::Tuple<2,int> dims(4,4);
  bonelab::Tuple<2,float> origin(-1.5,1.75);
  bonelab::Tuple<2,float> spacing(0.5,0.25);
  TImage image(dims, spacing, origin);
  image(1,1) = 5.0;
  image(1,2) = 6.0;
  image(2,1) = 7.0;
  image(2,2) = 8.0;
  ASSERT_TRUE(compare_bilinear(image, 0, 0, 0));
  ASSERT_TRUE(compare_bilinear(image, 0, -10, 0));
  ASSERT_TRUE(compare_bilinear(image, 0, 10, 0));
  ASSERT_TRUE(compare_bilinear(image, -10, 0, 0));
  ASSERT_TRUE(compare_bilinear(image, 10, 0, 0));
  ASSERT_TRUE(compare_bilinear(image, -1.0, 2.0, 5.0));
  ASSERT_TRUE(compare_bilinear(image, -1.0, 2.25, 6.0));
  ASSERT_TRUE(compare_bilinear(image, -0.5, 2.0, 7.0));
  ASSERT_TRUE(compare_bilinear(image, -0.5, 2.25, 8.0));
  ASSERT_TRUE(compare_bilinear(image, -0.75, 2.125, 6.5));
  ASSERT_TRUE(compare_bilinear(image, -1.0, 2.125, 5.5));
  ASSERT_TRUE(compare_bilinear(image, -0.5, 2.125, 7.5));
  ASSERT_TRUE(compare_bilinear(image, -0.75, 2.0, 6.0));
  ASSERT_TRUE(compare_bilinear(image, -0.75, 2.25, 7.0));
  }

TEST_F (PixelInterpolatorTests, TestBilinearWithFallback)
  {
  bonelab::Tuple<2,int> dims(2,2);
  bonelab::Tuple<2,float> origin(-1.0,2.00);
  bonelab::Tuple<2,float> spacing(0.5,0.25);
  TImage image(dims, spacing, origin);
  image(0,0) = 5.0;
  image(0,1) = 6.0;
  image(1,0) = 7.0;
  image(1,1) = 8.0;
  ASSERT_TRUE(compare_bilinear_with_fallback(image, 0, 0, 0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, 0, -10, 0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, 0, 10, 0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -10, 0, 0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, 10, 0, 0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.5, 2.0, 7.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.5, 2.25, 8.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.75, 2.125, 6.5));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.500001, 2.125, 7.5));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.75, 2.000001, 6.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.75, 2.249999, 7.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -1.1, 2.249999, 6.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.9, 2.3, 6.0));
  ASSERT_TRUE(compare_bilinear_with_fallback(image, -0.9, 1.9, 5.0));
  }
