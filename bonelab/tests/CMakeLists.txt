# Actually the ENABLE_TESTING command has to be in the root
# source directory CMakeLists.txt file.
enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(bonelabTests
    FFT_util_Tests.cpp
    TupleTests.cpp
	property_tree_util_Tests.cpp
	conf_parser_Tests.cpp
	typesTests.cpp
	endianTests.cpp
	ArrayTests.cpp
	ImageTests.cpp
	textTests.cpp
	DoubleBufferTests.cpp
	)

target_link_libraries(bonelabTests ${GTEST_BOTH_LIBRARIES})

if (UNIX AND NOT APPLE)
    target_link_libraries(bonelabTests pthread)
endif (UNIX AND NOT APPLE)

add_test(bonelabTests bonelabTests)
