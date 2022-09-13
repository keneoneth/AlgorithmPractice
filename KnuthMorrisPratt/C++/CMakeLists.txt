cmake_minimum_required(VERSION 3.10)

project(KMP_PROJ VERSION 1.0)

add_executable(KMP KMP.cpp)

set_property(TARGET KMP PROPERTY CXX_STANDARD 11)

install(TARGETS KMP CONFIGURATIONS Debug RUNTIME DESTINATION bin)