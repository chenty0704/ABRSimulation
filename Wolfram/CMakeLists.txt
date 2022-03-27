set(LLU_ROOT "$ENV{ProgramFiles}/Wolfram Research/LibraryLinkUtilities/${CMAKE_BUILD_TYPE}"
        CACHE PATH "Path to LLU installation.")
find_package(LLU REQUIRED CONFIG PATH_SUFFIXES LLU)

add_library(ABRSimulationInterface
        Sources/ABRSimulationInterface.cpp)
target_link_libraries(ABRSimulationInterface PRIVATE LLU::LLU)
set_property(TARGET ABRSimulationInterface PROPERTY PREFIX "")
set_windows_static_runtime(ABRSimulationInterface)

install_paclet_files(
        TARGET ABRSimulationInterface
        LLU_LOCATION ${LLU_ROOT}
        PACLET_NAME ${PROJECT_NAME}
)
