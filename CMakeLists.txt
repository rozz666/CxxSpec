project(cxxspec)

include_directories(".")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(cxxspec testSpecification.cpp main.cpp)
