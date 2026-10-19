cmake_minimum_required(VERSION 3.24)

project(IntroToAlgo
  VERSION 1.0.0
  DESCRIPTION "Classic algorithms and data structures from an algorithms course, in modern C++"
  LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Examples, tests and the benchmark are only built by default when this is the top-level project,
# so that add_subdirectory()/FetchContent users get just the library.
option(INTROALGO_BUILD_EXAMPLES "Build the example programs (the original demos)" ${PROJECT_IS_TOP_LEVEL})
option(INTROALGO_BUILD_TESTS "Build the GoogleTest suite" ${PROJECT_IS_TOP_LEVEL})
option(INTROALGO_BUILD_BENCHMARKS "Build the complexity-scaling benchmark" ${PROJECT_IS_TOP_LEVEL})
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
# Library: one static library, one header/source pair per algorithm
# ----------------------------------------------------------------------------
set(INTROALGO_SOURCES
  src/backtracking/constrained_permutations.cpp
  src/data_structures/bst.cpp
  src/data_structures/chain_hash.cpp
  src/data_structures/max_heap.cpp
  src/data_structures/rb_tree.cpp
  src/data_structures/van_emde_boas.cpp
  src/divide_and_conquer/out_of_ranking.cpp
  src/dynamic_programming/bitonic_tour.cpp
  src/dynamic_programming/edit_distance.cpp
  src/dynamic_programming/knapsack_01.cpp
  src/dynamic_programming/lcs.cpp
  src/dynamic_programming/lmis.cpp
  src/dynamic_programming/max_quad_exp.cpp
  src/dynamic_programming/obst.cpp
  src/dynamic_programming/rod_cutting.cpp
  src/geometry/rotating_cube.cpp
  src/graph/flow_graph.cpp
  src/graph/graph.cpp
  src/greedy/solar_panels.cpp
  src/linear_programming/simplex.cpp
  src/math/optimal_stopping.cpp
  src/matrix/lup.cpp
  src/sorting/sort.cpp
  src/strings/kmp.cpp
  src/strings/rolling_hash.cpp
  src/strings/trie.cpp
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(introalgo STATIC ${INTROALGO_SOURCES})
add_library(introalgo::introalgo ALIAS introalgo)
target_compile_features(introalgo PUBLIC cxx_std_17)
target_include_directories(introalgo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(introalgo PRIVATE "$<BUILD_INTERFACE:introalgo_warnings>")

install(TARGETS introalgo EXPORT introalgoTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT introalgoTargets NAMESPACE introalgo:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/introalgo)
configure_package_config_file(cmake/introalgoConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/introalgoConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/introalgo)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/introalgoConfigVersion.cmake
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/introalgoConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/introalgoConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/introalgo)

enable_testing()

# ----------------------------------------------------------------------------
# Examples: the original programs' main() functions, one executable each.
# Each is also registered as a test that must run to completion.
# ----------------------------------------------------------------------------
if(INTROALGO_BUILD_EXAMPLES)
  function(introalgo_add_example source)
    get_filename_component(name ${source} NAME_WE)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE introalgo introalgo_warnings)
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples)
    add_test(NAME example.${name} COMMAND ${name})
    set_tests_properties(example.${name} PROPERTIES LABELS example TIMEOUT 60)
  endfunction()

  introalgo_add_example(examples/assignment6_spam_and_dna.cpp)
  introalgo_add_example(examples/backtracking/constrained_permutations_demo.cpp)
  introalgo_add_example(examples/data_structures/bst_demo.cpp)
  introalgo_add_example(examples/data_structures/chain_hash_demo.cpp)
  introalgo_add_example(examples/data_structures/max_heap_demo.cpp)
  introalgo_add_example(examples/data_structures/van_emde_boas_demo.cpp)
  introalgo_add_example(examples/divide_and_conquer/out_of_ranking_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/bitonic_tour_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/edit_distance_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/knapsack_01_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/lcs_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/lmis_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/max_quad_exp_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/obst_demo.cpp)
  introalgo_add_example(examples/dynamic_programming/rod_cutting_demo.cpp)
  introalgo_add_example(examples/geometry/rotating_cube_demo.cpp)
  introalgo_add_example(examples/graph/flow_graph_demo.cpp)
  introalgo_add_example(examples/graph/graph_demo.cpp)
  introalgo_add_example(examples/greedy/solar_panels_demo.cpp)
  introalgo_add_example(examples/linear_programming/simplex_demo.cpp)
  introalgo_add_example(examples/math/optimal_stopping_demo.cpp)
  introalgo_add_example(examples/matrix/lup_demo.cpp)
  introalgo_add_example(examples/sorting/sort_demo.cpp)
  introalgo_add_example(examples/strings/kmp_demo.cpp)
  introalgo_add_example(examples/strings/rolling_hash_demo.cpp)
  introalgo_add_example(examples/strings/trie_demo.cpp)
endif()

# ----------------------------------------------------------------------------
# Benchmark: empirical complexity fit (a measurement tool; ctest only runs a --quick smoke pass)
# ----------------------------------------------------------------------------
if(INTROALGO_BUILD_BENCHMARKS)
  add_executable(scaling benchmarks/scaling.cpp)
  target_link_libraries(scaling PRIVATE introalgo introalgo_warnings)
  set_target_properties(scaling PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks)
  add_test(NAME benchmark.scaling_quick COMMAND scaling --quick)
  set_tests_properties(benchmark.scaling_quick PROPERTIES LABELS benchmark TIMEOUT 60)
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
      DOWNLOAD_EXTRACT_TIMESTAMP FALSE)
    FetchContent_MakeAvailable(googletest)
  endif()

  add_executable(introalgo_tests
    tests/test_bitonic_tour.cpp
    tests/test_bst.cpp
    tests/test_chain_hash.cpp
    tests/test_checked_arithmetic.cpp
    tests/test_constrained_permutations.cpp
    tests/test_edit_distance.cpp
    tests/test_flow_graph.cpp
    tests/test_graph.cpp
    tests/test_kmp.cpp
    tests/test_knapsack_01.cpp
    tests/test_lcs.cpp
    tests/test_lmis.cpp
    tests/test_lup.cpp
    tests/test_max_heap.cpp
    tests/test_max_quad_exp.cpp
    tests/test_obst.cpp
    tests/test_optimal_stopping.cpp
    tests/test_out_of_ranking.cpp
    tests/test_rb_tree.cpp
    tests/test_rod_cutting.cpp
    tests/test_rolling_hash.cpp
    tests/test_rotating_cube.cpp
    tests/test_simplex.cpp
    tests/test_solar_panels.cpp
    tests/test_sort.cpp
    tests/test_trie.cpp
    tests/test_van_emde_boas.cpp
  )
  target_include_directories(introalgo_tests PRIVATE tests)
  target_link_libraries(introalgo_tests PRIVATE introalgo introalgo_warnings GTest::gtest_main)
  gtest_discover_tests(introalgo_tests DISCOVERY_MODE PRE_TEST PROPERTIES LABELS unit TIMEOUT 120)
endif()
