cmake_minimum_required(VERSION 3.2)

project(airwindows-lv2 LANGUAGES C)

# Default to release builds
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(APPLE)
  set(CMAKE_INSTALL_PREFIX $ENV{HOME}/Library/Audio/Plug-Ins/LV2)
elseif(UNIX)
  set(CMAKE_INSTALL_PREFIX /usr/local/lib/lv2)
endif()

include(FindPkgConfig)
pkg_check_modules(LV2 lv2 REQUIRED)
if(MSYS OR MINGW)
  # Ugly but it should work in most cases
  string(REGEX REPLACE "C:" "/c/" LV2_INCLUDE_DIRS ${LV2_INCLUDE_DIRS})
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/airwindows.lv2)


function(add_plugin PLUGIN_NAME)
  add_library(${PLUGIN_NAME} SHARED src/${PLUGIN_NAME}/${PLUGIN_NAME}.c)
  target_include_directories(${PLUGIN_NAME} PRIVATE ${LV2_INCLUDE_DIRS})
  set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

  if(MSVC)
    target_compile_options(${PLUGIN_NAME} PRIVATE /Wall)
  else()
    target_compile_options(${PLUGIN_NAME} PRIVATE -Wall -Wextra -Wpedantic)
  endif()

  configure_file(src/${PLUGIN_NAME}/${PLUGIN_NAME}.ttl ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PLUGIN_NAME}.ttl)
endfunction()

add_plugin(LeftoMono)
add_plugin(RightoMono)
add_plugin(Ultrasonic)

configure_file(src/manifest.ttl.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/manifest.ttl)

if(APPLE OR UNIX)
  install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
