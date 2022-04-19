add_library(picomidi INTERFACE)

target_sources(picomidi INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/midi.c
)

target_include_directories(picomidi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(picomidi INTERFACE pico_stdlib)
