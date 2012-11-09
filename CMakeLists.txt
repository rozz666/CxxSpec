project(cxxspec)

cmake_minimum_required(VERSION 2.8)

include_directories(".")
include_directories("gmock-1.6.0/fused-src")

add_library(
    gmock STATIC
    gmock-1.6.0/fused-src/gmock-gtest-all.cc
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

add_executable(
    cxxspec
    testConsoleSpecificationObserver.cpp
    testSpecificationRegistry.cpp
    testAssertions.cpp
    testLinker2.cpp
    testLinker1.cpp
    testExecutor.cpp
    testSpecification.cpp
    example.cpp
    main.cpp
)

target_link_libraries(cxxspec gmock pthread)
