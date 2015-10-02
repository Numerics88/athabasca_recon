#include "util/Subvolumer.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<3,TValue,TIndex,TSpace> TVolume;

// Create a test fixture class.
class SubvolumerTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (SubvolumerTests, OneIntoOne)
  {
  bonelab::Tuple<3,TIndex> dims(1,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 1);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0), dims);
  ASSERT_EQ(subvolumer.GetSubvolumeOrigin(0), origin);
  }

TEST_F (SubvolumerTests, OneIntoTwo)
  {
  bonelab::Tuple<3,TIndex> dims(1,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 2);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0)[0] + subvolumer.GetSubvolumeDims(1)[0], 1);
  TIndex index = (subvolumer.GetSubvolumeDims(0)[0] == 1) ? 0 : 1;
  ASSERT_EQ(subvolumer.GetSubvolumeDims(index), dims);
  ASSERT_EQ(subvolumer.GetSubvolumeOrigin(index), origin);
  }

TEST_F (SubvolumerTests, TwoIntoOne)
  {
  bonelab::Tuple<3,TIndex> dims(2,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 1);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0), dims);
  ASSERT_EQ(subvolumer.GetSubvolumeOrigin(0), origin);
  }

TEST_F (SubvolumerTests, TwoIntoTwo)
  {
  bonelab::Tuple<3,TIndex> dims(2,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 2);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0), (bonelab::Tuple<3,TIndex>(1,3,4)));
  ASSERT_EQ(subvolumer.GetSubvolumeDims(1), (bonelab::Tuple<3,TIndex>(1,3,4)));
  ASSERT_EQ(subvolumer.GetSubvolumeOrigin(0), origin);
  ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(1)[0], -10+0.1);
  ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(1)[1], 2);
  ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(1)[2], 5);
  }

TEST_F (SubvolumerTests, TwoIntoThree)
  {
  bonelab::Tuple<3,TIndex> dims(2,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 3);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0)[0]
           + subvolumer.GetSubvolumeDims(1)[0]
           + subvolumer.GetSubvolumeDims(2)[0], 2);
  bonelab::Tuple<3,TSpace> subvolumeOrigin(origin);
  for (int i=0; i<3; ++i)
    {
    ASSERT_GE(subvolumer.GetSubvolumeDims(i)[0], 0);
    ASSERT_LE(subvolumer.GetSubvolumeDims(i)[0], 1);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[0], subvolumeOrigin[0]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[1], subvolumeOrigin[1]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[2], subvolumeOrigin[2]);
    subvolumeOrigin[0] += subvolumer.GetSubvolumeDims(i)[0] * 0.1;
    }  
  }

TEST_F (SubvolumerTests, ThreeIntoOne)
  {
  bonelab::Tuple<3,TIndex> dims(3,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 1);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0), dims);
  ASSERT_EQ(subvolumer.GetSubvolumeOrigin(0), origin);
  }

TEST_F (SubvolumerTests, ThreeIntoTwo)
  {
  bonelab::Tuple<3,TIndex> dims(3,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 2);
  ASSERT_EQ(subvolumer.GetSubvolumeDims(0)[0]
           + subvolumer.GetSubvolumeDims(1)[0], 3);
  bonelab::Tuple<3,TSpace> subvolumeOrigin(origin);
  for (int i=0; i<2; ++i)
    {
    ASSERT_GE(subvolumer.GetSubvolumeDims(i)[0], 1);
    ASSERT_LE(subvolumer.GetSubvolumeDims(i)[0], 2);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[0], subvolumeOrigin[0]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[1], subvolumeOrigin[1]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[2], subvolumeOrigin[2]);
    subvolumeOrigin[0] += subvolumer.GetSubvolumeDims(i)[0] * 0.1;
    }  
  }

TEST_F (SubvolumerTests, ThreeIntoThree)
  {
  bonelab::Tuple<3,TIndex> dims(3,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, 3);
  bonelab::Tuple<3,TSpace> subvolumeOrigin(origin);
  for (int i=0; i<3; ++i)
    {
    ASSERT_EQ(subvolumer.GetSubvolumeDims(i)[0], 1);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[0], subvolumeOrigin[0]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[1], subvolumeOrigin[1]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[2], subvolumeOrigin[2]);
    subvolumeOrigin[0] += subvolumer.GetSubvolumeDims(i)[0] * 0.1;
    }  
  }

TEST_F (SubvolumerTests, ThreeIntoFour)
  {
  TIndex dimZ = 3;
  TIndex slices = 4;
  bonelab::Tuple<3,TIndex> dims(dimZ,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, slices);
  bonelab::Tuple<3,TSpace> subvolumeOrigin(origin);
  TIndex sum = 0;
  for (int i=0; i<slices; ++i)
    {
    ASSERT_GE(subvolumer.GetSubvolumeDims(i)[0], 0);
    ASSERT_LE(subvolumer.GetSubvolumeDims(i)[0], 1);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[0], subvolumeOrigin[0]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[1], subvolumeOrigin[1]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[2], subvolumeOrigin[2]);
    subvolumeOrigin[0] += subvolumer.GetSubvolumeDims(i)[0] * 0.1;
    sum += subvolumer.GetSubvolumeDims(i)[0];
    }  
  ASSERT_EQ(sum, dimZ);
  }

TEST_F (SubvolumerTests, ThreeIntoFive)
  {
  TIndex dimZ = 3;
  TIndex slices = 5;
  bonelab::Tuple<3,TIndex> dims(dimZ,3,4);
  bonelab::Tuple<3,TSpace> spacing(0.1,0.2,0.4);
  bonelab::Tuple<3,TSpace> origin(-10,2,5);
  Subvolumer<TVolume> subvolumer(dims, spacing, origin, slices);
  bonelab::Tuple<3,TSpace> subvolumeOrigin(origin);
  TIndex sum = 0;
  for (int i=0; i<slices; ++i)
    {
    ASSERT_GE(subvolumer.GetSubvolumeDims(i)[0], 0);
    ASSERT_LE(subvolumer.GetSubvolumeDims(i)[0], 1);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[0], subvolumeOrigin[0]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[1], subvolumeOrigin[1]);
    ASSERT_FLOAT_EQ(subvolumer.GetSubvolumeOrigin(i)[2], subvolumeOrigin[2]);
    subvolumeOrigin[0] += subvolumer.GetSubvolumeDims(i)[0] * 0.1;
    sum += subvolumer.GetSubvolumeDims(i)[0];
    }  
  ASSERT_EQ(sum, dimZ);
  }

