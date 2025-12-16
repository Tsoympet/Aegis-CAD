cmake_minimum_required(VERSION 3.20)
project(AegisCAD VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

set(AEGISCAD_UTILS
    src/utils/JsonHelpers.cpp
    src/utils/Logging.cpp
    src/utils/Settings.cpp
)

add_library(aegiscad_core STATIC ${AEGISCAD_UTILS})
target_include_directories(aegiscad_core PUBLIC src)

add_executable(AegisCAD src/app/main.cpp)
target_link_libraries(AegisCAD PRIVATE aegiscad_core)
target_include_directories(AegisCAD PRIVATE src)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
find_package(OpenCASCADE REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
find_package(OpenCV QUIET)
find_package(Tesseract QUIET)

file(GLOB_RECURSE AEGISCAD_SOURCES
    src/app/*.cpp
    src/ui/*.cpp
    src/cad/*.cpp
    src/cam/*.cpp
    src/assembly/*.cpp
    src/analysis/*.cpp
    src/ai/*.cpp
    src/drafting/*.cpp
    src/utils/*.cpp
    src/scripting/*.cpp
)

set(AEGISCAD_RESOURCES
    resources/aegiscad.qrc
)

add_library(AegisCADLib
    ${AEGISCAD_SOURCES}
)

target_include_directories(AegisCADLib PRIVATE
    src/app
    src/ui
    src/cad
    src/cam
    src/assembly
    src/analysis
    src/ai
    src/drafting
    src/utils
    src/scripting
    ${OpenCASCADE_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
)

target_link_libraries(AegisCADLib PRIVATE
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
    pybind11::embed
    ${OpenCV_LIBS}
    ${Tesseract_LIBRARIES}
)

if (OpenCV_FOUND)
    target_compile_definitions(AegisCADLib PRIVATE HAVE_OPENCV)
endif()

if (Tesseract_FOUND)
    target_compile_definitions(AegisCADLib PRIVATE HAVE_TESSERACT)
endif()

target_compile_definitions(AegisCADLib PRIVATE QT_NO_KEYWORDS)

target_compile_options(AegisCADLib PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

add_executable(AegisCAD
    ${AEGISCAD_RESOURCES}
)

target_link_libraries(AegisCAD PRIVATE AegisCADLib)

install(TARGETS AegisCAD
    RUNTIME DESTINATION bin
)
install(DIRECTORY resources/ DESTINATION share)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

message(STATUS "AegisCAD configured without external GUI dependencies")
