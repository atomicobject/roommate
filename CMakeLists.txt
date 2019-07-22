cmake_minimum_required(VERSION 3.13)
 
project(my_app)

file(GLOB SOURCES "hello/main.c" "amazon-freertos/demos/demo_runner/aws_demo.c")

include_directories(my_app PRIVATE "amazon-freertos/demos/include")
include_directories(my_app PRIVATE "amazon-freertos/libraries/abstractions/platform/freertos/include")

add_executable(my_app ${SOURCES} ${HEADERS})

set(IDF_PROJECT_EXECUTABLE my_app)

set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")

add_subdirectory(amazon-freertos)
