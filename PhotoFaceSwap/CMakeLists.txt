include(GNUInstallDirs)
include(${PROJECT_SOURCE_DIR}/cmake/utils.cmake)

# fetch content dlib libraries
include(FetchContent)
FetchContent_Declare(
  dlib
  GIT_REPOSITORY https://github.com/davisking/dlib.git
  GIT_TAG v19.24
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)
option(USE_AVX_INSTRUCTIONS "Compile your program with AVX instructions" ON)
option(DLIB_NO_GUI_SUPPORT ON)
FetchContent_MakeAvailable(dlib)

include(FetchContent)
FetchContent_Declare(mahi-gui
                     GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git)
FetchContent_MakeAvailable(mahi-gui)

set(TARGET_NAME PhotoFaceSwap)

file(GLOB_RECURSE TARGET_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB HEADER_GUI ${CMAKE_CURRENT_LIST_DIR}/gui/*.hpp)
file(GLOB SRC_GUI ${CMAKE_CURRENT_LIST_DIR}/gui/*.cpp)

find_package(OpenCV REQUIRED)
message(${OpenCV_LIBS})
if(OpenCV_FOUND)
  include_directories(${OpenCV_INCLUDE_DIRS} "/usr/include/opencv4" ${dlib_DIR}
                      ${CMAKE_CURRENT_LIST_DIR}/include)

  add_executable(${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/CliMain.cpp
                                ${CMAKE_CURRENT_LIST_DIR}/shape_predictor.o)
  target_sources(${TARGET_NAME} PRIVATE ${TARGET_SOURCES})
  target_link_libraries(${TARGET_NAME} PUBLIC dlib::dlib ${OpenCV_LIBS})
  target_link_options(${TARGET_NAME} PUBLIC -v -s)

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
    target_compile_options(${TARGET_NAME} PRIVATE -O3 -mavx -DNDEBUG)
  endif()

  add_executable(
    ${TARGET_NAME}-Gui
    ${CMAKE_CURRENT_LIST_DIR}/GuiMain.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shape_predictor.o ${HEADER_GUI} ${SRC_GUI})
  target_sources(${TARGET_NAME}-Gui PRIVATE ${TARGET_SOURCES})
  target_include_directories(${TARGET_NAME}-Gui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gui)
  target_link_libraries(${TARGET_NAME}-Gui PUBLIC dlib::dlib ${OpenCV_LIBS}
                                                  mahi::gui)
  target_link_options(${TARGET_NAME}-Gui PUBLIC -v -s)
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
    target_compile_options(${TARGET_NAME}-Gui PRIVATE -O3 -mavx -DNDEBUG)
  endif()

  # embed_resource("model/shapepredictor.dat", "include/shapepredictor.hpp",
  # "shapepredictor") add_resource_embed(${TARGET_NAME}
  # "model/shapepredictor.dat")

else()
  message(FATAL_ERROR "OpenCV not found, so we won't build the example.")
endif()
