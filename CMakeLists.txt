cmake_minimum_required(VERSION 3.22)
project(ABRSimulation)

include(CMakePrintHelpers)

# Installs to the build directory by default.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}"
            CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif ()

set(CMAKE_CXX_STANDARD 23)

include_directories(Headers)

add_subdirectory(Sources)
add_subdirectory(Tests)
add_subdirectory(Wolfram)
