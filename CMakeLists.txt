cmake_minimum_required (VERSION 3.16)

include(pico_sdk_import.cmake)

project(main_project)

pico_sdk_init()

#This creates an executable target "blink" from a single source file blink.c
add_executable(main main.c)


#Generates different file formats such as .uf2 .elf .bin
pico_add_extra_outputs(main)

#Specifies libraries - flags to use when linking a given target
target_link_libraries(main pico_stdlib
hardware_pwm
hardware_rtc)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
