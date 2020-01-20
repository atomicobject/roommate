cmake_minimum_required(VERSION 3.13)

project(freertos_examples)

add_executable(roommate src/main.c)

# Tell IDF build to link against this target.
set(IDF_PROJECT_EXECUTABLE roommate)

# Add some extra components. IDF_EXTRA_COMPONENT_DIRS is an variable used by ESP-IDF
# to collect extra components.
get_filename_component(
    EXTRA_COMPONENT_DIRS
    "components/roommate_main" ABSOLUTE
)
list(APPEND IDF_EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS})

# As of now there's no offical way to redefine config files outside of Amazon FreeRTOS source tree.
# This is a temporary approach to inject an include path so that this takes precedence over the
# config file directory inside Amazon FreeRTOS.
include_directories(BEFORE amazon-freertos-configs)
include_directories(BEFORE ../roommate-boards/current-board-dir)

# https://github.com/aws/amazon-freertos/issues/832#issuecomment-514677910
set(IDF_SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults")

# Add amazon freertos as an subdirectory. AFR_BOARD tells which board to target.
set(AFR_BOARD espressif.esp32_devkitc CACHE INTERNAL "")
add_subdirectory(amazon-freertos)

# Link against the mqtt demo so that we can use it. Dependencies of this demo are transitively
# linked.
target_link_libraries(roommate PRIVATE AFR::demo_mqtt)
