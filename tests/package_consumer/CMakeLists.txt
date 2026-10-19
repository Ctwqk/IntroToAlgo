# Minimal project that uses an *installed* introalgo; CI builds it against `cmake --install`.
cmake_minimum_required(VERSION 3.21)
project(introalgo_consumer CXX)

find_package(introalgo REQUIRED)

add_executable(use_introalgo main.cpp)
target_link_libraries(use_introalgo PRIVATE introalgo::introalgo)
