include(GNUInstallDirs)
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

# fetch content dlib libraries
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  dlib
  GIT_REPOSITORY https://github.com/davisking/dlib.git
  GIT_TAG efe46ce76abfb265fc05c0e9a78741abf1bedfec # v19.24
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
option(USE_AVX_INSTRUCTIONS "Compile your program with AVX instructions" ON)
FetchContent_MakeAvailable(dlib)

set(TARGET_NAME PhotoFaceSwap)

file(GLOB_RECURSE TARGET_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

if(NOT DLIB_NO_GUI_SUPPORT)
  find_package(OpenCV REQUIRED)
  if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS} "/usr/include/opencv4"
                        ${dlib_DIR} ${CMAKE_CURRENT_LIST_DIR}/include)

    add_executable(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/main.cpp)
    target_sources(${TARGET_NAME} PRIVATE ${TARGET_SOURCES})
    target_link_libraries(${TARGET_NAME} PRIVATE dlib::dlib ${OpenCV_LIBS})
    target_enable_warning(${TARGET_NAME})
    if(PFS_ENABLE_SANITIZER)
      target_enable_sanitizer(${TARGET_NAME})
    endif()
    message(STATUS "enabling precompiled header build library GuiWrap")
    configure_file(${PROJECT_SOURCE_DIR}/cmake/pch.h.in ${PROJECT_BINARY_DIR}/pch.h @ONLY)
    target_precompile_headers(${TARGET_NAME} PRIVATE ${PROJECT_BINARY_DIR}/pch.h)

  else()
    message(FATAL_ERROR "OpenCV not found, so we won't build the example.")
  endif()
else()
  message(FATAL_ERROR "No GUI support, so we won't build the the program.")
endif()
