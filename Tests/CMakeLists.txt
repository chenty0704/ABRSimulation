find_package(GTest REQUIRED)

add_executable(NetworkModelTest
        NetworkModel/NetworkModelTest.cpp
        NetworkModel/ThroughputEstimatorsTest.cpp)
target_link_libraries(NetworkModelTest PRIVATE NetworkModel GTest::gtest_main)
