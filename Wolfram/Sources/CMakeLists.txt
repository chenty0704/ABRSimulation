add_library(ABRSimulationInterface SHARED
        Interface/CoreInterface.cpp
        Interface/ABRSimulationInterface.cpp)
target_link_libraries(ABRSimulationInterface
        PRIVATE LLU::LLU ABRSimulation)
set_property(TARGET ABRSimulationInterface PROPERTY PREFIX "")
