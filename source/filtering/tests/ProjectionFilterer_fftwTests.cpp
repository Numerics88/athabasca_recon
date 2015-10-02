#include "filtering/ProjectionFilterer_fftw.hpp"

typedef float TValue;
typedef int TIndex;
typedef float TSpace;
typedef bonelab::Image<2,TValue,TIndex,TSpace> TProjection;
typedef athabasca_recon::ProjectionFilterer_fftw<TProjection> TProjectionFilterer;

#define PROJECTION_FILTERER_TESTS ProjectionFilterer_fftwTests

#include "ProjectionFiltererTestsTemplate.txx"
