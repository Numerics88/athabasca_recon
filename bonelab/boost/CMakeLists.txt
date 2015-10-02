PROJECT (BONELAB_BOOST)

# Create cmake config file with include file location.
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/bonelab_boost-config.cmake.in"
  "${PROJECT_BINARY_DIR}/bonelab_boost-config.cmake")
