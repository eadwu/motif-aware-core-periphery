# Adapted from https://github.com/opencv/opencv/blob/4.x/samples/cpp/example_cmake/CMakeLists.txt

# CMake needs this line
cmake_minimum_required(VERSION 3.1)

# Define project name
project(core_periphery_metric)

# C++11
set(CMAKE_CXX_STANDARD 11)

# nlohmann_json
include(FetchContent)

set(JSON_ImplicitConversions OFF)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
  GIT_TAG v3.10.5)

FetchContent_MakeAvailable(json)

# Metrics
add_subdirectory(centrality)
add_subdirectory(idealized)
add_subdirectory(modularity)
add_subdirectory(nmi)

# Declare the executable target built from your sources
add_executable(metric main.cpp node.cpp)
target_link_libraries(metric PRIVATE nlohmann_json::nlohmann_json centrality idealized modularity nmi)
