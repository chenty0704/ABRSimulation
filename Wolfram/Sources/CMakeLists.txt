add_library(ABRSimulationLink SHARED
        Links/CoreLink.cpp
        Links/ABRSimulationLink.cpp)
target_link_libraries(ABRSimulationLink
        PRIVATE LLU::LLU ABRSimulation)
set_property(TARGET ABRSimulationLink PROPERTY PREFIX "")
