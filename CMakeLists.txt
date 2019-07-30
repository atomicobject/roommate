cmake_minimum_required(VERSION 3.13)
 
project(my_app)

# set(CMAKE_TOOLCHAIN_FILE "amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake")

file(GLOB SOURCES
    "src/*.c"
    "amazon-freertos/demos/demo_runner/aws_demo.c"
    "amazon-freertos/demos/demo_runner/aws_demo_network_addr.c"
)
# file(GLOB SOURCES "hello/main.c" "amazon-freertos/demos/demo_runner/aws_demo.c")
include_directories("src/include")


include_directories(my_app PRIVATE "amazon-freertos/demos/include")

# ../src/main.c:46:28: fatal error: aws_test_utils.h: No such file or directory
include_directories(my_app PRIVATE "amazon-freertos/tests/include")

# iot_network_manager_private.h
# include_directories(my_app PRIVATE "amazon-freertos/demos/network_manager")



# ../amazon-freertos/demos/demo_runner/aws_demo_network_addr.c:33:26: fatal error: FreeRTOS_IP.h: No such file or directory
include_directories(my_app PRIVATE "amazon-freertos/libraries/freertos_plus/standard/freertos_plus_tcp/include")

include_directories(my_app PRIVATE "amazon-freertos/libraries/abstractions/platform/freertos/include")

add_executable(my_app ${SOURCES} ${HEADERS})

# target_compile_definitions(my_app PRIVATE BLE_ENABLED=0)

# add_definitions(-DBLE_ENABLED=0)

set(IDF_PROJECT_EXECUTABLE my_app)

# set(CONFIG_BT_ENABLED "n")
# set(CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS "y")

# +CONFIG_BT_ENABLED=n
# +CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS=y
# add_definitions(-DIDF_SDKCONFIG_DEFAULTS=\"sdkconfig.my.defaults\")


set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")

add_subdirectory(amazon-freertos)

target_link_libraries(my_app PRIVATE AFR::mqtt)
target_link_libraries(my_app PRIVATE 3rdparty::jsmn)
target_link_libraries(my_app PRIVATE AFR::wifi)
target_link_libraries(my_app PRIVATE AFR::dev_mode_key_provisioning)
# ../src/main.c:42:29: fatal error: iot_system_init.h: No such file or directory
target_link_libraries(my_app PRIVATE AFR::utils)
target_link_libraries(my_app PRIVATE AFR::secure_sockets)
