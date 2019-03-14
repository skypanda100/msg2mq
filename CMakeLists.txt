cmake_minimum_required(VERSION 2.8)
project(msg2mq)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set (CMAKE_C_STANDARD 99)
endif ()

set(SOURCE_FILES main.c mq.c mq.h)
add_executable(msg2mq ${SOURCE_FILES})
target_link_libraries(msg2mq rabbitmq)
