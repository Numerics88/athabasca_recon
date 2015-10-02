#include "projection_correction/ConstantAttenuationBeamPowerCorrector.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef ConstantAttenuationBeamPowerCorrector<TProjection> TBeamPowerCorrector;

// Create a test fixture class.
class ConstantAttenuationBeamPowerCorrectorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (ConstantAttenuationBeamPowerCorrectorTests, BasicTest)
  {
  bonelab::Tuple<2,TIndex> dims(2,4);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.

  // Create two test projections
  TValue proj0Data[] = {2,4,6,8,10,12,14,16};  // sums to 72
  TProjection proj0(proj0Data, dims, spacing, origin);
  TValue proj1Data[] = {2.5,4,6.5,8,10,12,14.5,16.5};  // sums to 76
  TProjection proj1(proj1Data, dims, spacing, origin);
  TValue expected[8];
  for (int i=0; i<8; ++i)
    {
    expected[i] = proj1[i] - 0.25;
    }
  TBeamPowerCorrector beamPowerCorrector;

  // The first one is the set as reference, and should be unmodified.
  beamPowerCorrector.SetReferenceProjection(proj0);
  beamPowerCorrector.ProcessProjection(9, proj0);
  for (int i=0; i<proj0.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj0[i], 2*(i+1));
    }

  // The second one should be shifted to the same total attenuation.
  beamPowerCorrector.ProcessProjection(11, proj1);
  for (int i=0; i<proj1.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj1[i], expected[i]);
    }
  }

TEST_F (ConstantAttenuationBeamPowerCorrectorTests, WithConstantTerm)
  {
  bonelab::Tuple<2,TIndex> dims(2,4);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TValue proj0Data[] = {2,4,6,8,10,12,14,16};  // sums to 72
  TProjection proj0(proj0Data, dims, spacing, origin);
  TValue proj1Data[] = {2.5,4,6.5,8,10,12,14.5,16.5};  // sums to 76
  TProjection proj1(proj1Data, dims, spacing, origin);
  TValue expected[8];
  TValue constantTerm = 0.5;
  for (int i=0; i<8; ++i)
    {
    expected[i] = proj1[i] - 0.25 - constantTerm;
    }
  TBeamPowerCorrector beamPowerCorrector;
  beamPowerCorrector.SetReferenceProjection(proj0);
  beamPowerCorrector.SetBeamDecayConstantTerm(constantTerm);
  beamPowerCorrector.ProcessProjection(9, proj0);
  for (int i=0; i<proj0.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj0[i], 2*(i+1) - constantTerm);
    }
  beamPowerCorrector.ProcessProjection(11, proj1);
  for (int i=0; i<proj1.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj1[i], expected[i]);
    }
  }
