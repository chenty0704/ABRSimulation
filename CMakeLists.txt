cmake_minimum_required(VERSION 3.22)
project(ABRSimulation)

include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 23)

set(LLU_ROOT "$ENV{ProgramFiles}/Wolfram Research/LibraryLinkUtilities/${CMAKE_BUILD_TYPE}"
        CACHE PATH "Path to LLU installation.")

find_package(LLU REQUIRED CONFIG PATH_SUFFIXES LLU)
find_package(Boost REQUIRED COMPONENTS json)
find_package(fmt REQUIRED CONFIG)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(Headers)
include_directories(Wolfram/Headers)

add_subdirectory(Sources)
add_subdirectory(Tests)
add_subdirectory(Wolfram)
