cmake_minimum_required(VERSION 3.11)
project(PhotoFaceSwap VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_COLOR_MAKEFILE ON)
set(BUILD_SHARED_LIBS OFF)
option(PFS_ENABLE_SANITIZER "enabling compiler sanitizer" OFF)

set(CMAKE_CXX_STANDARD 20)
# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Resource file list
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib_archive)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/PhotoFaceSwap)
