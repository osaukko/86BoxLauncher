cmake_minimum_required(VERSION 3.12)

project(
  86BoxLauncher
  VERSION 0.1.0
  DESCRIPTION "Alternative cross-platform 86Box launcher"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(DetectQt)
detect_qt_version()
message(STATUS "Using Qt version ${QT_VERSION_STR}")

add_subdirectory(src)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
