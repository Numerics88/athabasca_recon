#include "filtering/ProjectionFilterer_Convolution.hpp"

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef athabasca_recon::ProjectionFilterer_Convolution<TProjection> TProjectionFilterer;

#define PROJECTION_FILTERER_TESTS ProjectionFiltererConvolutionTests

#include "ProjectionFiltererTestsTemplate.txx"
