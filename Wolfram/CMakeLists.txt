if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
            CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
endif ()

include_directories(Headers)

add_subdirectory(Sources)

install_paclet_files(TARGET ABRSimulationInterface
        LLU_LOCATION ${LLU_ROOT}
        PACLET_NAME ABRSimulation)
