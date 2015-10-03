cmake_minimum_required (VERSION 2.8)

project (Athabasca_Recon)

set (PB_SOURCES
    bonelab/print_multithread.cpp
    bonelab/MessageQueue.cpp
    bonelab/ThreadRunner.cpp
    bonelab/SynchronizeMessage.cpp
    source/ReconConfiguration.cpp
    source/util/LinearFit.cpp
    source/filtering/RampFilterRealSpace.cpp
    source/filtering/GaussianFilter.cpp
    source/filtering/TaperedCosineWindowFilter.cpp
    source/filtering/ProjectionFilterer_Convolution.cpp
    )

# Require Boost
set (Boost_USE_STATIC_LIBS ON)
find_package (Boost 1.42.0 COMPONENTS thread system filesystem program_options date_time REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

if (WIN32)
  # Get rid of some useless compiler warnings in Windows.
  add_definitions("/wd4244 /wd4018 /wd4305")
endif (WIN32)

# Disable auto-linking for boost on Windows, as it causes endless grief
if (WIN32)
  add_definitions (-DBOOST_ALL_NO_LIB)
endif ()

option (ENABLE_TESTING "Build unit and other tests" ON)
if (ENABLE_TESTING)
  ENABLE_TESTING()
  # Require Google Test
  find_package(GTest REQUIRED)
endif (ENABLE_TESTING)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/source)
# This is actually for bonelab
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (bonelab)
# Need to put these definitions here, because they affect files in bonelab
# that are included with #include directives.
if (TRACE_THREADING)
  add_definitions (-DTRACE_THREADING)
endif (TRACE_THREADING)
if (RANGE_CHECKING)
  add_definitions (-DRANGE_CHECKING)
endif (RANGE_CHECKING)

if (APPLE)
   option (USE_VDSP "Enable Apple's VDSP library for calculating FFTs" ON)
   option (USE_FFTW "Enable FFTW library for calculating FFTs" OFF)
else (APPLE)
   option (USE_FFTW "Enable FFTW library for calculating FFTs" ON)
endif (APPLE)

if (USE_VDSP)
  add_definitions (-DUSE_VDSP)
  set (PB_SOURCES ${PB_SOURCES}
       source/filtering/ProjectionFilterer_vDSP.cpp)
endif (USE_VDSP)

if (USE_FFTW)
  add_definitions (-DUSE_FFTW)
  set (PB_SOURCES ${PB_SOURCES}
      bonelab/fftw_mt.cpp
      source/filtering/ProjectionFilterer_fftw.cpp)
  FIND_PATH (FFTW_INCLUDE_DIR fftw3.h)
  FIND_LIBRARY (FFTW_LIBRARY fftw3)
  FIND_LIBRARY (FFTW_FLOAT_LIBRARY fftw3f)
  set (FFTW_LIBRARIES ${FFTW_LIBRARY} ${FFTW_FLOAT_LIBRARY})
  mark_as_advanced (FFTW_INCLUDE_DIR)
  mark_as_advanced (FFTW_LIBRARY)
  mark_as_advanced (FFTW_FLOAT_LIBRARY)
  include_directories (${FFTW_INCLUDE_DIR})
endif (USE_FFTW)

# This is only needed for the tests, which are defined in CMakeLists.txt
# files within the source code tree.
add_subdirectory (source)

add_executable (athabasca_recon ${PB_SOURCES} source/athabasca_recon.cpp)

target_link_libraries (athabasca_recon
    ${Boost_THREAD_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    )

if (USE_VDSP)
  set_target_properties(athabasca_recon
      PROPERTIES LINK_FLAGS "-framework Accelerate")
endif (USE_VDSP)

if (USE_FFTW)
  target_link_libraries (athabasca_recon ${FFTW_LIBRARIES})
endif (USE_FFTW)

if (UNIX AND NOT APPLE)
  target_link_libraries(athabasca_recon pthread)
endif (UNIX AND NOT APPLE)
