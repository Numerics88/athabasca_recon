find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TEST_SOURCE_FILES
    RampFilterRealSpaceTests.cpp ../RampFilterRealSpace.cpp
    ProjectionFilterer_ConvolutionTests.cpp ../ProjectionFilterer_Convolution.cpp
	GaussianFilterTests.cpp ../GaussianFilter.cpp
	TaperedCosineWindowFilterTests.cpp ../TaperedCosineWindowFilter.cpp
    )

if (USE_VDSP)
  set (TEST_SOURCE_FILES ${TEST_SOURCE_FILES}
       ProjectionFilterer_vDSPTests.cpp
       ../ProjectionFilterer_vDSP.cpp )
endif (USE_VDSP)

if (USE_FFTW)
  set (TEST_SOURCE_FILES ${TEST_SOURCE_FILES}
       ../../../bonelab/fftw_mt.cpp
       ProjectionFilterer_fftwTests.cpp
       ../ProjectionFilterer_fftw.cpp )
endif (USE_FFTW)

add_executable(FilteringTests ${TEST_SOURCE_FILES})

target_link_libraries(FilteringTests ${GTEST_BOTH_LIBRARIES})

if (USE_VDSP)
  set_target_properties(FilteringTests
      PROPERTIES LINK_FLAGS "-framework Accelerate")
endif (USE_VDSP)

if (USE_FFTW)
  target_link_libraries (FilteringTests
      ${FFTW_LIBRARIES}
      ${Boost_THREAD_LIBRARY}
      ${Boost_SYSTEM_LIBRARY}
      ${Boost_DATE_TIME_LIBRARY}
      )
endif (USE_FFTW)

add_test(FilteringTests FilteringTests)
