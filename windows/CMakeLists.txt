cmake_minimum_required(VERSION 3.14)
set(PROJECT_NAME "secugen_windows")
project(${PROJECT_NAME} LANGUAGES CXX)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "${PROJECT_NAME}_plugin")

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "secugen_windows_plugin.cpp"
  "include/secugen_windows/secugen_windows_plugin.h"
)

apply_standard_settings(${PLUGIN_NAME})
set_target_properties(${PLUGIN_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

# Detect architecture and choose appropriate SDK files
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(SECUGEN_SDK_ARCH "x64")
else()
  set(SECUGEN_SDK_ARCH "i386")
endif()

# Include SecuGen SDK
target_include_directories(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sdk/inc")
target_link_libraries(${PLUGIN_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/sdk/lib/${SECUGEN_SDK_ARCH}/sgfplib.lib")

# Copy SecuGen DLLs to output directory
set(SECUGEN_DLLS
  "sgfplib.dll"
  "sgfpamx.dll"
  "sgwsqlib.dll"
)

if(SECUGEN_SDK_ARCH STREQUAL "i386")
  list(APPEND SECUGEN_DLLS "sgfdusda.dll")
else()
  list(APPEND SECUGEN_DLLS "sgfdusdax64.dll")
endif()

foreach(DLL ${SECUGEN_DLLS})
  add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/sdk/bin/${SECUGEN_SDK_ARCH}/${DLL}"
    $<TARGET_FILE_DIR:${PLUGIN_NAME}>
  )
endforeach()

# List of absolute paths to libraries that should be bundled with the plugin
set(secugen_windows_bundled_libraries
  ""
  PARENT_SCOPE
)