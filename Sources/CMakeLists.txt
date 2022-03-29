find_package(Boost REQUIRED COMPONENTS json)

add_library(JSON
        Core/JSON.cpp)
target_link_libraries(JSON PUBLIC
        Boost::json)

add_library(VideoModel
        VideoModel/VideoModel.cpp)
target_link_libraries(VideoModel PRIVATE
        JSON)

add_library(NetworkModel
        NetworkModel/NetworkModel.cpp)
target_link_libraries(NetworkModel PRIVATE
        JSON)

add_library(ThroughputEstimators
        ThroughputEstimators/ThroughputEstimator.cpp
        ThroughputEstimators/ExponentialMovingAverageEstimator.cpp)
target_link_libraries(ThroughputEstimators PUBLIC
        NetworkModel)

add_library(ABRControllers
        ABRControllers/ThroughputBasedController.cpp)
target_link_libraries(ABRControllers PUBLIC
        VideoModel
        ThroughputEstimators)

add_library(ABRSimulation
        ABRSimulation/ABRSimulation.cpp)
target_link_libraries(ABRSimulation PUBLIC
        VideoModel
        NetworkModel
        ThroughputEstimators
        ABRControllers)
