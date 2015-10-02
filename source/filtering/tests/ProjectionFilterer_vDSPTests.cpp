#include "filtering/ProjectionFilterer_vDSP.hpp"

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef athabasca_recon::ProjectionFilterer_vDSP<TProjection> TProjectionFilterer;

#define PROJECTION_FILTERER_TESTS ProjectionFilterer_vDSPTests

#include "ProjectionFiltererTestsTemplate.txx"
