# Actually the ENABLE_TESTING command has to be in the root
# source directory CMakeLists.txt file.
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(utilTests
    LinearFitTests.cpp ../LinearFit.cpp
    SubvolumerTests.cpp
	)

target_link_libraries(utilTests ${GTEST_BOTH_LIBRARIES})

if (UNIX AND NOT APPLE)
    target_link_libraries(utilTests pthread)
endif (UNIX AND NOT APPLE)

add_test(utilTests utilTests)
