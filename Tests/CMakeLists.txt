find_package(GTest REQUIRED)

add_executable(NetworkModelTest
        NetworkModel/NetworkModelTest.cpp)
target_link_libraries(NetworkModelTest PRIVATE NetworkModel GTest::gtest_main)

add_executable(ThroughputEstimatorsTest
        ThroughputEstimators/ExponentialMovingAverageEstimatorTest.cpp)
target_link_libraries(ThroughputEstimatorsTest PRIVATE ThroughputEstimators GTest::gtest_main)

add_executable(ABRSimulationTest
        ABRSimulation/ABRSimulationTest.cpp)
target_link_libraries(ABRSimulationTest PRIVATE ABRSimulation GTest::gtest_main)
