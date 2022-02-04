find_package(GTest REQUIRED)

add_executable(VideoModelTest
        VideoModel/VideoModelTest.cpp)
target_link_libraries(VideoModelTest PRIVATE VideoModel GTest::gtest_main)

add_executable(NetworkModelTest
        NetworkModel/NetworkModelTest.cpp
        NetworkModel/ThroughputEstimatorsTest.cpp)
target_link_libraries(NetworkModelTest PRIVATE NetworkModel GTest::gtest_main)
