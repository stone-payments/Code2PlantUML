cmake_minimum_required(VERSION 3.5)


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -Wall -std=c++14")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++14")

project (genGraph)


add_executable(${PROJECT_NAME} main.cpp)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif (CMAKE_BUILD_TYPE STREQUAL "Release")
