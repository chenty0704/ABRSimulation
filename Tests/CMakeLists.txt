find_package(GTest REQUIRED)

add_executable(NetworkModelTest NetworkModelTest.cpp)
target_link_libraries(NetworkModelTest PRIVATE NetworkModel GTest::gtest_main)
