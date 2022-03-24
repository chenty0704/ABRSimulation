find_package(LLU REQUIRED CONFIG PATH_SUFFIXES LLU)
find_package(Boost REQUIRED COMPONENTS json)

add_library(JSON
        Core/JSON.cpp)
target_link_libraries(JSON PUBLIC Boost::json)

add_library(VideoModel
        VideoModel/VideoModel.cpp)
target_link_libraries(VideoModel PRIVATE JSON)

add_library(NetworkModel
        NetworkModel/NetworkModel.cpp
        NetworkModel/ThroughputEstimators.cpp)
target_link_libraries(NetworkModel PRIVATE JSON)

add_library(ABRControllers
        ABRControllers/ThroughputBasedController.cpp)
