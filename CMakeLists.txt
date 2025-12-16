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

install(TARGETS AegisCAD
    RUNTIME DESTINATION bin
)
install(DIRECTORY resources/ DESTINATION share)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

message(STATUS "AegisCAD configured without external GUI dependencies")
