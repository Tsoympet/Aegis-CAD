cmake_minimum_required(VERSION 3.16)
project(AegisCAD VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(OpenCASCADE REQUIRED)

file(GLOB_RECURSE AEGISCAD_SOURCES
    src/app/*.cpp
    src/ui/*.cpp
    src/ui/modern/*.cpp
    src/cad/*.cpp
    src/assembly/*.cpp
    src/cam/*.cpp
    src/drafting/*.cpp
    src/analysis/*.cpp
    src/ai/*.cpp
    src/utils/*.cpp
    src/scripting/*.cpp
    src/perf/*.cpp
)

add_executable(AegisCAD
    ${AEGISCAD_SOURCES}
    resources/aegiscad.qrc
)

target_include_directories(AegisCAD PRIVATE
    src/app
    src/ui
    src/ui/modern
    src/cad
    src/assembly
    src/cam
    src/drafting
    src/analysis
    src/ai
    src/utils
    src/scripting
    src/perf
)

target_link_libraries(AegisCAD
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
)

if (WIN32)
    set_target_properties(AegisCAD PROPERTIES WIN32_EXECUTABLE ON)
endif()

install(TARGETS AegisCAD
    RUNTIME DESTINATION bin
)
