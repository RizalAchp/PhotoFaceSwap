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
      >
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

function(add_resource_embed target input)
  string(MAKE_C_IDENTIFIER ${input} input_identifier)
  set(output "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${input_identifier}.o")

  target_link_libraries(${target} PRIVATE ${output})
  add_custom_command(
    OUTPUT ${output}
    COMMAND ${CMAKE_LINKER} --relocatable --format binary --output ${output}
            ${input}
    DEPENDS ${input})

  set(RC_DEPENDS
      ${RC_DEPENDS} ${output}
      PARENT_SCOPE)
endfunction()

function(embed_resource resource_file_name source_file_name variable_name)

  if(EXISTS "${source_file_name}")
    if("${source_file_name}" IS_NEWER_THAN "${resource_file_name}")
      return()
    endif()
  endif()

  file(READ "${resource_file_name}" hex_content HEX)

  string(REPEAT "[0-9a-f]" 32 pattern)
  string(REGEX REPLACE "(${pattern})" "\\1\n" content "${hex_content}")

  string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

  string(REGEX REPLACE ", $" "" content "${content}")

  set(array_definition
      "static const unsigned char ${variable_name}[] =\n{\n${content}\n};")

  set(source "// Auto generated file.\n${array_definition}\n")

  file(WRITE "${source_file_name}" "${source}")

  string(LENGTH "${hex_content}" hex_content_length)
  set(size_definition
      "\nstatic const size_t ${variable_name}_length = ${hex_content_length}\n;"
  )
  file(APPEND "${source_file_name}" "${size_definition}")

endfunction()
