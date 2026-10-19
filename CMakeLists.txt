cmake_minimum_required(VERSION 3.20)

project(IntroToAlgo
  VERSION 1.0.0
  DESCRIPTION "Classic algorithms and data structures from an algorithms course, in modern C++"
  LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(INTROALGO_BUILD_EXAMPLES "Build the example programs (the original demos)" ON)
option(INTROALGO_BUILD_TESTS "Build the GoogleTest suite" ON)
option(INTROALGO_BUILD_BENCHMARKS "Build the complexity-scaling benchmark" ON)
option(INTROALGO_SANITIZE "Build everything with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(INTROALGO_WERROR "Treat compiler warnings as errors" OFF)
option(INTROALGO_USE_SYSTEM_GTEST "Use an installed GoogleTest instead of downloading one" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(INTROALGO_SANITIZE)
  if(MSVC)
    message(FATAL_ERROR "INTROALGO_SANITIZE is only supported with GCC/Clang")
  endif()
  add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=undefined
                      -fno-omit-frame-pointer -g)
  add_link_options(-fsanitize=address,undefined)
endif()

# Warning flags are attached to our own targets only (not to GoogleTest).
add_library(introalgo_warnings INTERFACE)
if(MSVC)
  target_compile_options(introalgo_warnings INTERFACE /W4)
  if(INTROALGO_WERROR)
    target_compile_options(introalgo_warnings INTERFACE /WX)
  endif()
else()
  target_compile_options(introalgo_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)
  if(INTROALGO_WERROR)
    target_compile_options(introalgo_warnings INTERFACE -Werror)
  endif()
endif()

# ----------------------------------------------------------------------------
# Library
# ----------------------------------------------------------------------------
set(INTROALGO_SOURCES
  src/divide_and_conquer/out_of_ranking.cpp
  src/dynamic_programming/bitonic_tour.cpp
  src/dynamic_programming/edit_distance.cpp
  src/dynamic_programming/knapsack_01.cpp
  src/dynamic_programming/lcs.cpp
  src/dynamic_programming/lmis.cpp
  src/dynamic_programming/max_quad_exp.cpp
  src/dynamic_programming/obst.cpp
  src/dynamic_programming/rod_cutting.cpp
  src/data_structures/bst.cpp
  src/data_structures/chain_hash.cpp
  src/data_structures/max_heap.cpp
  src/data_structures/rb_tree.cpp
  src/data_structures/van_emde_boas.cpp
  src/graph/flow_graph.cpp
  src/graph/graph.cpp
  src/sorting/sort.cpp
  src/strings/kmp.cpp
  src/strings/rolling_hash.cpp
  src/strings/trie.cpp
)

add_library(introalgo STATIC ${INTROALGO_SOURCES})
add_library(introalgo::introalgo ALIAS introalgo)
target_include_directories(introalgo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(introalgo PRIVATE introalgo_warnings)

install(TARGETS introalgo ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

enable_testing()

# ----------------------------------------------------------------------------
# Examples: the original programs' main() functions, one executable each
# ----------------------------------------------------------------------------
if(INTROALGO_BUILD_EXAMPLES)
  function(introalgo_add_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE introalgo introalgo_warnings)
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                             ${CMAKE_BINARY_DIR}/examples)
    add_test(NAME example.${name} COMMAND ${name})
    set_tests_properties(example.${name} PROPERTIES LABELS example)
  endfunction()

  introalgo_add_example(out_of_ranking_demo examples/divide_and_conquer/out_of_ranking_demo.cpp)
  introalgo_add_example(bitonic_tour_demo examples/dynamic_programming/bitonic_tour_demo.cpp)
  introalgo_add_example(edit_distance_demo examples/dynamic_programming/edit_distance_demo.cpp)
  introalgo_add_example(knapsack_01_demo examples/dynamic_programming/knapsack_01_demo.cpp)
  introalgo_add_example(lcs_demo examples/dynamic_programming/lcs_demo.cpp)
  introalgo_add_example(lmis_demo examples/dynamic_programming/lmis_demo.cpp)
  introalgo_add_example(max_quad_exp_demo examples/dynamic_programming/max_quad_exp_demo.cpp)
  introalgo_add_example(obst_demo examples/dynamic_programming/obst_demo.cpp)
  introalgo_add_example(rod_cutting_demo examples/dynamic_programming/rod_cutting_demo.cpp)
  introalgo_add_example(bst_demo examples/data_structures/bst_demo.cpp)
  introalgo_add_example(chain_hash_demo examples/data_structures/chain_hash_demo.cpp)
  introalgo_add_example(max_heap_demo examples/data_structures/max_heap_demo.cpp)
  introalgo_add_example(van_emde_boas_demo examples/data_structures/van_emde_boas_demo.cpp)
  introalgo_add_example(sort_demo examples/sorting/sort_demo.cpp)
  introalgo_add_example(flow_graph_demo examples/graph/flow_graph_demo.cpp)
  introalgo_add_example(graph_demo examples/graph/graph_demo.cpp)
  introalgo_add_example(kmp_demo examples/strings/kmp_demo.cpp)
  introalgo_add_example(rolling_hash_demo examples/strings/rolling_hash_demo.cpp)
  introalgo_add_example(trie_demo examples/strings/trie_demo.cpp)
  introalgo_add_example(assignment6_spam_and_dna examples/assignment6_spam_and_dna.cpp)
endif()

# ----------------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------------
if(INTROALGO_BUILD_TESTS)
  include(GoogleTest)
  if(INTROALGO_USE_SYSTEM_GTEST)
    find_package(GTest REQUIRED)
  else()
    include(FetchContent)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
      URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(googletest)
  endif()

  add_executable(introalgo_tests
    tests/test_out_of_ranking.cpp
    tests/test_bitonic_tour.cpp
    tests/test_edit_distance.cpp
    tests/test_knapsack_01.cpp
    tests/test_lcs.cpp
    tests/test_lmis.cpp
    tests/test_max_quad_exp.cpp
    tests/test_obst.cpp
    tests/test_rod_cutting.cpp
    tests/test_bst.cpp
    tests/test_chain_hash.cpp
    tests/test_max_heap.cpp
    tests/test_rb_tree.cpp
    tests/test_sort.cpp
    tests/test_van_emde_boas.cpp
    tests/test_flow_graph.cpp
    tests/test_graph.cpp
    tests/test_kmp.cpp
    tests/test_rolling_hash.cpp
    tests/test_trie.cpp
  )
  target_include_directories(introalgo_tests PRIVATE tests)
  target_link_libraries(introalgo_tests PRIVATE introalgo introalgo_warnings GTest::gtest_main)
  gtest_discover_tests(introalgo_tests DISCOVERY_MODE PRE_TEST PROPERTIES LABELS unit)
endif()
