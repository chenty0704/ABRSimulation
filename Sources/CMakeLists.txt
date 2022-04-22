add_library(Core INTERFACE)
target_link_libraries(Core
        INTERFACE fmt::fmt)

add_library(NetworkModel
        NetworkModel/NetworkModel.cpp)

add_library(ThroughputEstimators
        ThroughputEstimators/IThroughputEstimator.cpp
        ThroughputEstimators/ExponentialMovingAverageEstimator.cpp)
target_link_libraries(ThroughputEstimators
        PUBLIC NetworkModel)

add_library(ABRControllers
        ABRControllers/ThroughputBasedController.cpp
        ABRControllers/ModelPredictiveController.cpp)
target_link_libraries(ABRControllers
        PUBLIC LLU::LLU ThroughputEstimators
        PRIVATE Core)

add_library(ABRSimulation
        ABRSimulation/ABRSimulation.cpp)
target_link_libraries(ABRSimulation
        PUBLIC NetworkModel ThroughputEstimators ABRControllers)
