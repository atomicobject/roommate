cmake_minimum_required(VERSION 3.13)
 
project(my_app)
 
# AFR_BOARD tells which board we need to target.
set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")
add_subdirectory(amazon-freertos)
 
# todo: add_subdirectory(src)
include_directories(src/include)
include_directories(amazon-freertos/demos/dev_mode_key_provisioning/include)
include_directories(amazon-freertos/libraries/abstractions/pkcs11/include)
include_directories(amazon-freertos/libraries/3rdparty/pkcs11)
include_directories(amazon-freertos/libraries/freertos_plus/standard/utils/include)
include_directories(amazon-freertos/libraries/abstractions/wifi/include)
include_directories(amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/include)

add_executable(my_app src/main.c)

target_link_libraries(my_app PRIVATE AFR::mqtt)