cmake_minimum_required(VERSION 3.20)
project(AegisCAD VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

file(GLOB_RECURSE AEGISCAD_SOURCES
    src/app/*.cpp
    src/ui/*.cpp
    src/cad/*.cpp
    src/analysis/*.cpp
    src/ai/*.cpp
    src/utils/*.cpp
    src/scripting/*.cpp
)

set(AEGISCAD_RESOURCES
    resources/aegiscad.qrc
)

add_executable(AegisCAD
    ${AEGISCAD_SOURCES}
    ${AEGISCAD_RESOURCES}
)

target_include_directories(AegisCAD PRIVATE
    src/app
    src/ui
    src/cad
    src/analysis
    src/ai
    src/utils
    src/scripting
    ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(AegisCAD PRIVATE
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
    pybind11::embed
)

target_compile_definitions(AegisCAD PRIVATE QT_NO_KEYWORDS)

target_compile_options(AegisCAD PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

install(TARGETS AegisCAD
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

message(STATUS "AegisCAD configured with Qt6, OpenCascade, and pybind11")
