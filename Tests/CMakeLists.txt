find_package(GTest REQUIRED)

add_executable(NetworkSimulationTest
        NetworkModelTest.cpp
        ThroughputEstimatorsTest.cpp)
target_link_libraries(NetworkSimulationTest PRIVATE NetworkSimulation GTest::gtest_main)
