add_executable(core_tests
    CoreTests.cpp
    ${CMAKE_SOURCE_DIR}/src/cad/StepIgesIO.cpp
    ${CMAKE_SOURCE_DIR}/src/cad/GltfExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/cad/FeatureOps.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logging.cpp
)

target_link_libraries(core_tests PRIVATE
    Qt6::Test
    Qt6::Widgets
    AegisCADLib
    ${OpenCASCADE_LIBRARIES}
)

target_include_directories(core_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${OpenCASCADE_INCLUDE_DIR}
)

add_test(NAME core_tests COMMAND core_tests)
