cmake_minimum_required(VERSION 3.5)

include(../pico_sdk_import.cmake)

include(${PICO_SDK_PATH}/lib/tinyusb/hw/bsp/family_support.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#set(FAMILY rp2040)
#set(BOARD adafruit_feather_rp2040)
# Default path to the SDK
#set(PICO_BOARD adafruit_feather_rp2040)

# Pull in Pico SDK (must be before project)

family_get_project_name(PROJECT ${CMAKE_CURRENT_LIST_DIR})
project(${PROJECT} C CXX ASM)


family_initialize_project(${PROJECT} ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(external/hagl)
add_subdirectory(external/hagl_hal)
add_subdirectory(external/SSD1306)

add_executable(${PROJECT})

target_include_directories(${PROJECT} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hagl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/hagl_hal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/midi
        ${CMAKE_CURRENT_SOURCE_DIR}/screen
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${FREERTOS_KERNEL_PATH}/include
        ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/include
)        

target_sources(${PROJECT} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/usb_descriptors.c
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/led_task.c
  ${CMAKE_CURRENT_SOURCE_DIR}/screen/disp_hagl.c
  ${CMAKE_CURRENT_SOURCE_DIR}/cdc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/rtos_hooks.c
  ${CMAKE_CURRENT_SOURCE_DIR}/device_callbacks.c
  ${CMAKE_CURRENT_SOURCE_DIR}/midi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/midi/gate.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/midi/LinkedList.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/midi/handlers.c
  ${CMAKE_CURRENT_SOURCE_DIR}/midi/Arp.c
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/MovingAvg.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/MultiClick.c
  ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/port.c
)

pico_set_program_name(${PROJECT} "Pico MIdi Gate")
pico_set_program_version(${PROJECT} "0.1")

# Enable usb output, disable uart output
#pico_enable_stdio_usb(${PROJECT} 1)
#pico_enable_stdio_uart(${PROJECT} 0)

target_link_libraries(${PROJECT} PUBLIC
        pico_stdlib
        pico_bootsel_via_double_reset
        hardware_spi
        hardware_dma
        hardware_rtc
        hardware_uart
        hardware_adc
        hagl
        hagl_hal
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
)

family_configure_device_example(${PROJECT})        
pico_add_extra_outputs(${PROJECT})

target_compile_definitions(${PROJECT} PRIVATE
#  HAGL_HAL_USE_SINGLE_BUFFER
  #HAGL_HAL_USE_DOUBLE_BUFFER
  HAGL_HAL_USE_TRIPLE_BUFFER
  HAGL_HAL_USE_DMA
#  HAGL_HAL_DEBUG
  MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=40000000
  MIPI_DISPLAY_PIN_CS=10
  MIPI_DISPLAY_PIN_DC=8 # A0
  MIPI_DISPLAY_PIN_RST=9
  MIPI_DISPLAY_PIN_BL=-1
  MIPI_DISPLAY_PIN_CLK=2
  MIPI_DISPLAY_PIN_MOSI=3 # SDA
  MIPI_DISPLAY_PIN_MISO=-1
  MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT
  MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_BGR|MIPI_DCS_ADDRESS_MODE_MIRROR_Y|MIPI_DCS_ADDRESS_MODE_MIRROR_X
  MIPI_DISPLAY_WIDTH=128
  MIPI_DISPLAY_HEIGHT=160
  MIPI_DISPLAY_OFFSET_X=0
  MIPI_DISPLAY_OFFSET_Y=0
  MIPI_DISPLAY_INVERT=0
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND	picotool load -x ${PROJECT}.uf2
    VERBATIM
)
