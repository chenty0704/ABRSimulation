find_package(GTest REQUIRED)

add_executable(MetaTest
        Core/MetaTest.cpp)
target_link_libraries(MetaTest
        PRIVATE GTest::gtest_main)

add_executable(NetworkModelTest
        NetworkModel/NetworkModelTest.cpp)
target_link_libraries(NetworkModelTest
        PRIVATE GTest::gtest_main NetworkModel)

add_executable(ThroughputEstimatorsTest
        ThroughputEstimators/ExponentialMovingAverageEstimatorTest.cpp)
target_link_libraries(ThroughputEstimatorsTest
        PRIVATE GTest::gtest_main ThroughputEstimators)

add_executable(ABRSimulationTest
        ABRSimulation/ABRSimulationTest.cpp)
target_link_libraries(ABRSimulationTest
        PRIVATE GTest::gtest_main ABRSimulation)

add_executable(AllTests
        $<TARGET_PROPERTY:MetaTest,SOURCES>
        $<TARGET_PROPERTY:NetworkModelTest,SOURCES>
        $<TARGET_PROPERTY:ThroughputEstimatorsTest,SOURCES>
        $<TARGET_PROPERTY:ABRSimulationTest,SOURCES>)
target_link_libraries(AllTests
        PRIVATE GTest::gtest_main NetworkModel ThroughputEstimators ABRControllers ABRSimulation)
