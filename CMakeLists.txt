cmake_minimum_required(VERSION 3.12)

project(
  86BoxLauncher
  VERSION 0.1.0
  DESCRIPTION "Alternative cross-platform 86Box launcher"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" OFF)
option(ENABLE_ASAN "Build with AddressSanitizer" OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(DetectQt)
detect_qt_version()
message(STATUS "Using Qt version ${QT_VERSION_STR}")

if(ENABLE_ASAN)
  message(STATUS "AddressSanitizer enabled")
  add_compile_options(-fsanitize=address -O1 -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address -O1 -fno-omit-frame-pointer)
endif()

add_subdirectory(src)

if(BUILD_TESTS)
  message(STATUS "Building with unit tests")
  add_subdirectory(tests)
endif()
