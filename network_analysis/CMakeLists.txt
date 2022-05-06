# Metrics
add_subdirectory(third_party)

# Declare the executable target built from your sources
add_executable(gml-to-el gml-to-el.cpp)
target_link_libraries(gml-to-el PRIVATE readgml)

add_executable(postprocess-edgelist postprocess-edgelist.cpp)
