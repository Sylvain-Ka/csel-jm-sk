cmake_minimum_required(VERSION 3.28)
project(main-app)

include(../../nanopi.cmake)
add_executable(main main.c ${DRIVER_SOURCES})

target_include_directories(main PRIVATE ${DRIVER_INCLUDE_DIRS})