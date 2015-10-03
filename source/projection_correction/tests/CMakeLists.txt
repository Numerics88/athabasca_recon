find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set (TEST_SOURCE_FILES
    ManualBeamPowerCorrectorTests.cpp
    ConstantAttenuationBeamPowerCorrectorTests.cpp
    NullProjectionEdgeBeamPowerCorrectorTests.cpp
    BeforeAndAfterBrightFieldBeamPowerCorrectorTests.cpp
    NeighborhoodWalkerTests.cpp
    BadPixelCorrectorTests.cpp
    )

add_executable(ProjectionCorrectionTests ${TEST_SOURCE_FILES})

target_link_libraries(ProjectionCorrectionTests ${GTEST_BOTH_LIBRARIES})

if (UNIX AND NOT APPLE)
    target_link_libraries(ProjectionCorrectionTests pthread)
endif (UNIX AND NOT APPLE)

add_test(ProjectionCorrectionTests ProjectionCorrectionTests)
