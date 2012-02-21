project(cxxspec)

include_directories(".")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

add_executable(cxxspec testSpecificationRegistry.cpp testAssertions.cpp testLinker2.cpp testLinker1.cpp testExecutor.cpp testSpecification.cpp main.cpp)
