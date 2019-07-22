cmake_minimum_required(VERSION 3.13)
 
project(my_app)


# file(GLOB HEADERS "amazon-freertos/demos/include/*.h")
file(GLOB SOURCES "hello/main.c" "amazon-freertos/demos/demo_runner/aws_demo.c")

add_executable(my_app ${SOURCES} ${HEADERS})

set(IDF_PROJECT_EXECUTABLE my_app)

get_filename_component(ESPRESSIF_CODE_COMPONENT "${CMAKE_CURRENT_LIST_DIR}/amazon-freertos/vendors/espressif/boards/esp32/aws_demos/application_code" ABSOLUTE)

# Add some external components to the project
set(IDF_EXTRA_COMPONENT_DIRS ${IDF_EXTRA_COMPONENT_DIRS}
  ${ESPRESSIF_CODE_COMPONENT}
)

set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")

add_subdirectory(amazon-freertos)

# FreeRTOS_Sockets.h
# include_directories("amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/include")

target_link_libraries(my_app PRIVATE AFR::secure_sockets)
target_include_directories(my_app PRIVATE "amazon-freertos/demos/include")
target_include_directories(my_app PRIVATE "amazon-freertos/libraries/abstractions/platform/freertos/include")