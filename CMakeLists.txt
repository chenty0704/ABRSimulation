cmake_minimum_required(VERSION 3.22)
project(ABRSimulator)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 23)

include_directories(Headers)

add_subdirectory(Sources)
add_subdirectory(Tests)
