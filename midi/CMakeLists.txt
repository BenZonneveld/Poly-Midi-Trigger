add_library(picomidi INTERFACE)

target_sources(picomidi INTERFACE
      ${CMAKE_CURRENT_LIST_DIR}/midi.c
      ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
)

target_include_directories(picomidi INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(picomidi INTERFACE pico_stdlib)
