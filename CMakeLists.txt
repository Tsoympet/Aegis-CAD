cmake_minimum_required(VERSION 3.16)
project(AegisCAD VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------
# Dependencies
# ---------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)

# ---------------------------
# Sources
# ---------------------------
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

# ---------------------------
# Qt Resource file (icons, qml, etc.)
# ---------------------------
set(AEGISCAD_RESOURCES
    resources/aegiscad.qrc
)

# ---------------------------
# Executable
# ---------------------------
add_executable(AegisCAD
    ${AEGISCAD_SOURCES}
    ${AEGISCAD_RESOURCES}
)

# ---------------------------
# Include directories
# ---------------------------
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
    ${OpenCASCADE_INCLUDE_DIR}
)

# ---------------------------
# Link libraries
# ---------------------------
target_link_libraries(AegisCAD
    Qt6::Widgets
    ${OpenCASCADE_LIBRARIES}
)

# ---------------------------
# Platform-specific setup
# ---------------------------
if (WIN32)
    set_target_properties(AegisCAD PROPERTIES WIN32_EXECUTABLE ON)
    # Embed icon in executable
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/app_icon.ico")
    if (EXISTS ${APP_ICON_RESOURCE_WINDOWS})
        target_sources(AegisCAD PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
    endif()
endif()

# ---------------------------
# Install
# ---------------------------
install(TARGETS AegisCAD
    RUNTIME DESTINATION bin
)

message(STATUS "✅ AegisCAD configured with Qt6 + OpenCascade + full icon embedding.")

