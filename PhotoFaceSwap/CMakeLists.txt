include(GNUInstallDirs)
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

set(BUILD_SHARED_LIBS OFF)
set(USE_AVX_INSTRUCTIONS ON)
set(DLIB_NO_GUI_SUPPORT ON)
set(DLIB_LINK_WITH_SQLITE3_STR OFF)
# fetch content dlib libraries
include(FetchContent)
FetchContent_Declare(
  dlib
  GIT_REPOSITORY https://github.com/davisking/dlib.git
  GIT_TAG v19.24
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(dlib)
FetchContent_Declare(mahi-gui
                     GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git)
FetchContent_MakeAvailable(mahi-gui)

# Link
set(TARGET_NAME PhotoFaceSwap)

file(GLOB_RECURSE TARGET_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADER_GUI ${CMAKE_CURRENT_LIST_DIR}/gui/*.hpp)
file(GLOB SRC_GUI ${CMAKE_CURRENT_LIST_DIR}/gui/*.cpp)

find_package(OpenCV CONFIG REQUIRED COMPONENTS opencv_core opencv_imgcodecs
                                        opencv_imgproc opencv_photo opencv_videoio)
message(${OpenCV_LIBS})

set(USE_AVX_INSTRUCTIONS ON)
set(DLIB_NO_GUI_SUPPORT ON)
set(DLIB_LINK_WITH_SQLITE3_STR OFF)
set(WITH_GTK OFF)
set(WITH_OPENGL ON)
set(WITH_WIN32UI OFF)
if(OpenCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS} "/usr/include/opencv4" ${dlib_DIR}
                      ${CMAKE_CURRENT_LIST_DIR}/include)

  add_executable(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/CliMain.cpp)
  target_sources(${TARGET_NAME} PRIVATE ${TARGET_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/shape_predictor.obj)
  target_link_libraries(${TARGET_NAME} PUBLIC dlib::dlib ${OpenCV_LIBS})

  target_enable_warning(${TARGET_NAME})
  if(PFS_ENABLE_SANITIZER)
    target_enable_sanitizer(${TARGET_NAME})
  endif()
  message(STATUS "enabling precompiled header build library GuiWrap")
  configure_file(${PROJECT_SOURCE_DIR}/cmake/pch.h.in
                 ${PROJECT_BINARY_DIR}/pch.h @ONLY)
  target_precompile_headers(${TARGET_NAME} PRIVATE ${PROJECT_BINARY_DIR}/pch.h)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "ENABLING OPTIMIZATION on RELEASE BUILD")
    if(MSVC)
      target_compile_options(${TARGET_NAME} PRIVATE /arch:AVX2 /fp:fast) 
    else()
      target_link_options(${TARGET_NAME} PUBLIC -v -s)
      target_compile_options(${TARGET_NAME} PRIVATE -mavx2 -Ofast)
    endif()
  endif()

  add_executable(${TARGET_NAME}-Gui ${CMAKE_CURRENT_LIST_DIR}/GuiMain.cpp)
  target_sources(${TARGET_NAME}-Gui PRIVATE ${TARGET_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/shape_predictor.obj ${HEADER_GUI} ${SRC_GUI})
  target_include_directories(${TARGET_NAME}-Gui
                             PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gui)
  target_link_libraries(${TARGET_NAME}-Gui PUBLIC dlib::dlib mahi::gui
                                                  ${OpenCV_LIBS})
  target_compile_definitions(${TARGET_NAME}-Gui PUBLIC PFS_GUI)

  target_enable_warning(${TARGET_NAME}-Gui)
  if(PFS_ENABLE_SANITIZER)
    target_enable_sanitizer(${TARGET_NAME}-Gui)
  endif()
  message(STATUS "enabling precompiled header build library GuiWrap")
  configure_file(${PROJECT_SOURCE_DIR}/cmake/pch.h.in
                 ${PROJECT_BINARY_DIR}/pch.h @ONLY)
  target_precompile_headers(${TARGET_NAME}-Gui PRIVATE
                            ${PROJECT_BINARY_DIR}/pch.h)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "ENABLING OPTIMIZATION on RELEASE BUILD")
    if(MSVC)
      target_compile_definitions(${TARGET_NAME}-Gui PUBLIC _WINDOWS WINDOW)
      target_link_options(${TARGET_NAME}-Gui PUBLIC  /SUBSYSTEM:WINDOWS)
      target_compile_options(${TARGET_NAME}-Gui PRIVATE /arch:AVX2 /fp:fast /EH) 
    else()
      target_link_options(${TARGET_NAME}-Gui PUBLIC -v -s)
      target_compile_options(${TARGET_NAME}-Gui PRIVATE -mavx2 -Ofast)
    endif()
  endif()

  # embed_resource("model/shapepredictor.dat", "include/shapepredictor.hpp",
  # "shapepredictor") add_resource_embed(${TARGET_NAME}
  # "model/shapepredictor.dat")

else()
  message(FATAL_ERROR "OpenCV not found, so we won't build the example.")
endif()
