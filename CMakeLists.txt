# Adapted from https://github.com/opencv/opencv/blob/4.x/samples/cpp/example_cmake/CMakeLists.txt

# CMake needs this line
cmake_minimum_required(VERSION 3.7)

# Define project name
project(quark_decomposition)

# Subprojects
add_subdirectory(metric)
add_subdirectory(network_analysis)
# add_subdirectory(static_visualizer)

## "External" subprojects
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

ExternalProject_Add(qd
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/quark_decomposition"
  DOWNLOAD_COMMAND URL "${CMAKE_CURRENT_SOURCE_DIR}/quark_decomposition/qd"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make -C <SOURCE_DIR>
  INSTALL_COMMAND cp <SOURCE_DIR>/quark <INSTALL_DIR>
)

ExternalProject_Add(lqd
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/quark_decomposition"
  DOWNLOAD_COMMAND URL "${CMAKE_CURRENT_SOURCE_DIR}/quark_decomposition/lqd"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make -C <SOURCE_DIR>
  INSTALL_COMMAND cp <SOURCE_DIR>/labeled_quark <INSTALL_DIR>
)

ExternalProject_Add(uqd
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/quark_decomposition"
  DOWNLOAD_COMMAND URL "${CMAKE_CURRENT_SOURCE_DIR}/quark_decomposition/uqd"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make -C <SOURCE_DIR>
  INSTALL_COMMAND cp <SOURCE_DIR>/undirected_quark <INSTALL_DIR>
)
