cmake_minimum_required(VERSION 3.5)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_PARALLEL_LEVEL 128)
set(FAMILY rp2040)
set(BOARD adafruit_feather_rp2040)
set(PICO_BOARD adafruit_feather_rp2040)

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)

project(PolyMidiGater)
family_initialize_project(PolyMidiGater ${CMAKE_CURRENT_LIST_DIR})

# family_add_subdirectory will filter what to actually add based on selected FAMILY
#family_add_subdirectory(midi)
family_add_subdirectory(Display)
