cmake_minimum_required(VERSION 3.10)

add_executable(DIJKSTRA dijkstra.cpp)

set_property(TARGET DIJKSTRA PROPERTY CXX_STANDARD 11)

install(TARGETS DIJKSTRA CONFIGURATIONS Debug RUNTIME DESTINATION bin)