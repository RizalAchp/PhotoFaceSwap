cmake_minimum_required(VERSION 3.11)

project(PhotoFaceSwap LANGUAGES CXX C)
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "0.1.0")

set(CMAKE_COLOR_MAKEFILE ON)
set(BUILD_SHARED_LIBS OFF)
option(PFS_ENABLE_SANITIZER "enabling compiler sanitizer" OFF)

# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "Choose Release or Debug" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/PhotoFaceSwap)
