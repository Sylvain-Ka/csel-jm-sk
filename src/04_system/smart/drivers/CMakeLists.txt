# drivers/CMakeLists.txt
file(GLOB_RECURSE IO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/user-io/*.c)
file(GLOB TIMER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/timer/*.c)

set(DRIVER_SOURCES
    ${IO_SOURCES}
    ${TIMER_SOURCES}
    PARENT_SCOPE
)
set(DRIVER_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/user-io/led
    ${CMAKE_CURRENT_SOURCE_DIR}/timer
    PARENT_SCOPE
)