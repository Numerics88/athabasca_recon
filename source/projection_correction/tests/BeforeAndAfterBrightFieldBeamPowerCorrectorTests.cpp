#include "projection_correction/BeforeAndAfterBrightFieldBeamPowerCorrector.hpp"
#include "CTCalibration.hpp"
#include <gtest/gtest.h>

using namespace athabasca_recon;

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef CTCalibration<TProjection,TProjection> TCalibration;
typedef BeforeAndAfterBrightFieldBeamPowerCorrector<TProjection> TBeamPowerCorrector;

// Create a test fixture class.
class BeforeAndAfterBrightFieldBeamPowerCorrectorTests : public ::testing::Test
  {
  };

// --------------------------------------------------------------------
// test implementations

TEST_F (BeforeAndAfterBrightFieldBeamPowerCorrectorTests, BasicTest)
  {
  bonelab::Tuple<2,TIndex> dims(2,4);
  bonelab::Tuple<2,TSpace> spacing(1,1);  // not used.
  bonelab::Tuple<2,TSpace> origin(0,0);  // not used.
  TCalibration ctCalibration;
  TValue flatFieldData[] = {100,99,100,100,100,100,100,100};
  TProjection& flatField = ctCalibration.GetFlatFieldNonConst();
  flatField.construct_reference(flatFieldData, dims, spacing, origin);
  TProjection& postScanFlatField = ctCalibration.GetPostScanFlatFieldNonConst();
  postScanFlatField.construct(dims, spacing, origin);
  TValue factor = 0.9;
  for (int i=0; i<flatField.size(); ++i)
    {
    postScanFlatField[i] = factor*flatField[i];
    }
  TValue advanceInterval = 10;
  ctCalibration.SetBrightFieldAdvanceInterval(advanceInterval);
  TValue postScanFlatFieldInterval = 4.5;
  ctCalibration.SetPostScanBrightFieldInterval(postScanFlatFieldInterval);
  TIndex numberOfProjections = 20;
  
  TValue projData[] = {2,4,6,8,10,12,14,16};
  TProjection proj(projData, dims, spacing, origin);
  TBeamPowerCorrector beamPowerCorrector;
  beamPowerCorrector.Initialize(&ctCalibration, numberOfProjections, 1, NULL);

  // This is the real test - are the linear and constant terms generated
  // properly?
  TValue linearTerm = log(1.0/factor)/
      (numberOfProjections - 1 + advanceInterval + postScanFlatFieldInterval);
  TValue constantTerm = linearTerm * advanceInterval;
  ASSERT_FLOAT_EQ(beamPowerCorrector.GetBeamDecayConstantTerm(), constantTerm);
  ASSERT_FLOAT_EQ(beamPowerCorrector.GetBeamDecayLinearTerm(), linearTerm);
  
  // Might as well also verify that linear and constant terms are being
  // applied properly, although this should be OK if the ManualBeamPowerCorrector
  // passes its tests.
  TValue expected[8];
  TIndex projectionIndex = 2;
  for (int i=0; i<8; ++i)
    {
    expected[i] = proj[i] - constantTerm - projectionIndex*linearTerm;
    }

  beamPowerCorrector.ProcessProjection(projectionIndex, proj);
  for (int i=0; i<proj.size(); ++i)
    {
    ASSERT_FLOAT_EQ(proj[i], expected[i]);
    }
  }
