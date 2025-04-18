cmake_minimum_required(VERSION 3.28)
project(smart-led-app)

include(../../nanopi.cmake)
add_executable(smart_led_control smart_led_control.c)