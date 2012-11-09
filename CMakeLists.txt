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
    test/testConsoleSpecificationObserver.cpp
    test/testSpecificationRegistry.cpp
    test/testAssertions.cpp
    test/testLinker2.cpp
    test/testLinker1.cpp
    test/testExecutor.cpp
    test/testSpecification.cpp
    test/main.cpp
    example/example.cpp
)

target_link_libraries(cxxspec gmock pthread)
