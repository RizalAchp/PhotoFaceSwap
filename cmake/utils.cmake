macro(target_enable_warning target_name)
  message(STATUS "enabling warning build on target: ${target_name}")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND MSVC_OPTIONS "/W3")
    if(MSVC_VERSION GREATER 1900) # Allow non fatal security warnings for msvc
                                  # 2015
      list(APPEND MSVC_OPTIONS "/WX")
    endif()
  endif()

  target_compile_options(
    ${target_name}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
      -Wall
      -Wextra
      -Werror
      -Wfatal-errors>
      $<$<CXX_COMPILER_ID:MSVC>:${MSVC_OPTIONS}>)
  # -Wconversion

  target_compile_features(${target_name} PRIVATE cxx_std_20)
endmacro()

# Enable address sanitizer (gcc/clang only)
function(target_enable_sanitizer target_name)
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Sanitizer supported only for gcc/clang")
  endif()
  message(STATUS "enabling Address sanitizer on target: ${target_name}")
  target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined)
  target_compile_options(${target_name}
                         PRIVATE -fno-sanitize=signed-integer-overflow)
  target_compile_options(${target_name} PRIVATE -fno-sanitize-recover=all)
  target_compile_options(${target_name} PRIVATE -fno-omit-frame-pointer)
  target_link_libraries(${target_name} PRIVATE -fsanitize=address,undefined
                                               -fuse-ld=gold)
endfunction()
