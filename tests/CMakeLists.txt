add_executable(core_tests
    CoreTests.cpp
)

target_link_libraries(core_tests PRIVATE aegiscad_core)
target_include_directories(core_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME core_tests COMMAND core_tests)
