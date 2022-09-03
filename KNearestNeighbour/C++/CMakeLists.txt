cmake_minimum_required(VERSION 3.10)

project(KNN_PROJ VERSION 1.0)

add_executable(KNN KNN.cpp)

set_property(TARGET KNN PROPERTY CXX_STANDARD 11)