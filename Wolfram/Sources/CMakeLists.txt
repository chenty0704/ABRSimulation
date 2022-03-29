add_library(ABRSimulationInterface SHARED
        Interface/ABRSimulationInterface.cpp)
target_link_libraries(ABRSimulationInterface PRIVATE
        LLU::LLU
        VideoModel
        NetworkModel
        ThroughputEstimators
        ABRControllers
        ABRSimulation)
set_property(TARGET ABRSimulationInterface PROPERTY PREFIX "")
