# This file is a copy of amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/amazon-freertos-common/component.mk
# That file adds "../../" as a COMPONENT_SRCDIRS which causes main.c in that directory to be built. We only want our own main file to be build.

AMAZON_FREERTOS_LIB_DIR := ../amazon-freertos/lib
AMAZON_FREERTOS_DEMOS_DIR := ../amazon-freertos/demos

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/mqtt \
        $(AMAZON_FREERTOS_LIB_DIR)/shadow \
        $(AMAZON_FREERTOS_LIB_DIR)/greengrass \
        $(AMAZON_FREERTOS_LIB_DIR)/utils \
        $(AMAZON_FREERTOS_LIB_DIR)/bufferpool \
        $(AMAZON_FREERTOS_LIB_DIR)/tls \
        $(AMAZON_FREERTOS_LIB_DIR)/crypto \
        $(AMAZON_FREERTOS_LIB_DIR)/wifi/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/secure_sockets/portable/freertos_plus_tcp \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/mbedtls \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/logging \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/devmode_key_provisioning \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32 \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/include $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn

COMPONENT_OBJEXCLUDE := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.o

ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
COMPONENT_SRCDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/common/demo_runner \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/shadow \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/mqtt \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/greengrass_connectivity \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/tcp \

COMPONENT_ADD_INCLUDEDIRS += include
COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/common/include
endif

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/third_party/pkcs11

amazon-freertos/lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
amazon-freertos/demos/common/logging/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized
amazon-freertos/demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format