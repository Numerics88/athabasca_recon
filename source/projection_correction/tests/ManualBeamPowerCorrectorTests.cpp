#include "projection_correction/ManualBeamPowerCorrector.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef ManualBeamPowerCorrector<TProjection> TBeamPowerCorrector;

// Create a test fixture class.
class ManualBeamPowerCorrectorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (ManualBeamPowerCorrectorTests, BasicTest)
  {
  bonelab::Tuple<2,TIndex> dims(2,4);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TValue projData[] = {2,4,6,8,10,12,14,16};
  TProjection proj(projData, dims, spacing, origin);
  TBeamPowerCorrector beamPowerCorrector;
  beamPowerCorrector.SetBeamDecayConstantTerm(0.5);
  beamPowerCorrector.SetBeamDecayLinearTerm(3.0);
  beamPowerCorrector.ProcessProjection(9, proj);
  for (int i=0; i<proj.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj[i], 2*(i+1) - 3*9 - 0.5);
    }
  }
