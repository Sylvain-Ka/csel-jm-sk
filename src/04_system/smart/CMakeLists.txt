cmake_minimum_required(VERSION 3.28)
project(smart-led-app)

add_subdirectory(drivers)

include(../../nanopi.cmake)
add_executable(smart_led_control smart_led_control.c ${DRIVER_SOURCES})

target_include_directories(smart_led_control PRIVATE ${DRIVER_INCLUDE_DIRS})