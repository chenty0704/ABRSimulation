find_package(LLU REQUIRED CONFIG PATH_SUFFIXES LLU)

add_library(NetworkSimulation
        NetworkModel.cpp
        ThroughputEstimators.cpp)
