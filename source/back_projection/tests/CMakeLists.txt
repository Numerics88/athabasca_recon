find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(BackProjectionTests
    PixelInterpolatorTests.cpp
    RayTracerTests.cpp
	ParallelBackProjectorTests.cpp
	)

target_link_libraries(BackProjectionTests ${GTEST_BOTH_LIBRARIES})

add_test(BackProjectionTests BackProjectionTests)
