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

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Airwindows.lv2)


function(add_plugin PLUGIN_NAME)
  add_library(${PLUGIN_NAME} SHARED src/${PLUGIN_NAME}/${PLUGIN_NAME}.c)
  target_include_directories(${PLUGIN_NAME} PRIVATE ${LV2_INCLUDE_DIRS})
  set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")

  if(MSVC)
    target_compile_options(${PLUGIN_NAME} PRIVATE /Wall)
  else()
    target_compile_options(${PLUGIN_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
  endif()

  configure_file(src/${PLUGIN_NAME}/${PLUGIN_NAME}.ttl ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${PLUGIN_NAME}.ttl)
endfunction()

add_plugin(Acceleration)
add_plugin(Acceleration2)
add_plugin(Baxandall)
add_plugin(BitShiftGain)
add_plugin(Capacitor)
add_plugin(Channel8)
add_plugin(ClipOnly)
add_plugin(ClipOnly2)
add_plugin(DCVoltage)
add_plugin(DeBess)
add_plugin(Galactic)
add_plugin(Infrasonic)
add_plugin(Interstage)
add_plugin(LeftoMono)
add_plugin(MV)
add_plugin(PurestConsoleBuss)
add_plugin(PurestConsoleChannel)
add_plugin(PurestGain)
add_plugin(RightoMono)
add_plugin(Sidepass)
add_plugin(Ultrasonic)
add_plugin(UltrasonicLite)
add_plugin(UltrasonicMed)

configure_file(src/manifest.ttl.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/manifest.ttl)

if(APPLE OR UNIX)
  install(DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
