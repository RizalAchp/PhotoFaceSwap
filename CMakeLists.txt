cmake_minimum_required(VERSION 3.11)
project(
  PhotoFaceSwap
  VERSION 0.1.0
  LANGUAGES CXX C)

option(PFS_ENABLE_SANITIZER "enabling compiler sanitizer" OFF)

if(MSVC)
set(CMAKE_TOOLCHAIN_FILE "D:/VCPKG/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif(MSVC)
set(LLVM_ENABLE_RTTI ON)
set(LLVM_ENABLE_EH ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
endif()
set(BUILD_SHARED_LIBS ON)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD 20)
# Set default build to release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/PhotoFaceSwap)
