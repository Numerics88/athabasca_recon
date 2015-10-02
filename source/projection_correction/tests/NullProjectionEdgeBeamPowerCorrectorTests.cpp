#include "projection_correction/NullProjectionEdgeBeamPowerCorrector.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef NullProjectionEdgeBeamPowerCorrector<TProjection> TBeamPowerCorrector;

// Create a test fixture class.
class NullProjectionEdgeBeamPowerCorrectorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (NullProjectionEdgeBeamPowerCorrectorTests, BasicTest)
  {
  bonelab::Tuple<2,TIndex> dims(2,4);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TValue projData[] = {2,1004,1006,8,10,1012,1014,16};
  TProjection proj(projData, dims, spacing, origin);
  TValue expected[8];
  TValue averageEdgeValue = 36/4.0;
  for (int i=0; i<8; ++i)
    {
    expected[i] = proj[i] - averageEdgeValue;
    }
  TBeamPowerCorrector beamPowerCorrector;
  beamPowerCorrector.SetEdgeWidth(1);
  beamPowerCorrector.ProcessProjection(9, proj);
  for (int i=0; i<proj.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj[i], expected[i]);
    }
  }

TEST_F (NullProjectionEdgeBeamPowerCorrectorTests, WiderBorderTest)
  {
  bonelab::Tuple<2,TIndex> dims(2,6);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TValue projData[] = {2,4,106,108,10,12,14,16,118,120,22,24};
  TProjection proj(projData, dims, spacing, origin);
  TValue expected[12];
  TValue averageEdgeValue = 104/8.0;
  for (int i=0; i<12; ++i)
    {
    expected[i] = proj[i] - averageEdgeValue;
    }
  TBeamPowerCorrector beamPowerCorrector;
  beamPowerCorrector.SetEdgeWidth(1);
  beamPowerCorrector.ProcessProjection(9, proj);
  for (int i=0; i<proj.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj[i], expected[i]);
    }
  }
