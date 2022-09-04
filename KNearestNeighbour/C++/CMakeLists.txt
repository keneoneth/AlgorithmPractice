cmake_minimum_required(VERSION 3.10)

add_executable(KNN KNN.cpp)

set_property(TARGET KNN PROPERTY CXX_STANDARD 11)

install(TARGETS KNN CONFIGURATIONS Debug RUNTIME DESTINATION bin)