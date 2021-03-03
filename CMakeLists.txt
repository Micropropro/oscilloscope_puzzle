cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(p2_project)
pico_sdk_init()
add_executable(p2
  p2.c
)
pico_enable_stdio_usb(p2 1)

pico_enable_stdio_uart(p2 1)
pico_add_extra_outputs(p2)
target_link_libraries(p2 pico_stdlib hardware_adc)

