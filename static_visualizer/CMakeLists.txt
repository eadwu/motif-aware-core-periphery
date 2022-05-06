# Adapted from https://github.com/opencv/opencv/blob/4.x/samples/cpp/example_cmake/CMakeLists.txt

# CMake needs this line
cmake_minimum_required(VERSION 3.1)

# Pkg-config for C headers
find_package(PkgConfig REQUIRED)

# Graphviz support
pkg_check_modules(GRAPHVIZ REQUIRED libgvc)

# Define project name
project(static_visualizer)

# Declare the executable target built from your sources
add_executable(visualize main.cpp)
target_link_libraries(visualize ${GRAPHVIZ_LIBRARIES})
target_include_directories(visualize PUBLIC ${GRAPHVIZ_INCLUDE_DIRS})
target_compile_options(visualize PUBLIC ${GRAPHVIZ_CFLAGS_OTHER})
