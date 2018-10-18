#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
# This file is a copy of amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/amazon-freertos-common/component.mk

# We do a few things here to try to simplify the process of integrating changes fro upstream:
# - Our changes are purely additive.
#   - If you need to edit an existing line from upstream, "comment" it out, and put your version above or below it.
# - To make diffing various versions easier, we keep the upstream version of this file in a separate branch: "upstream-version-of-copied-files".

ifeq ("commented","out")
#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====
AMAZON_FREERTOS_LIB_DIR := ../../../../../../../lib
AMAZON_FREERTOS_DEMOS_DIR := ../../../../../../../demos
#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
endif

AMAZON_FREERTOS_LIB_DIR := ../amazon-freertos/lib
AMAZON_FREERTOS_DEMOS_DIR := ../amazon-freertos/demos
#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====

COMPONENT_SRCDIRS := $(AMAZON_FREERTOS_LIB_DIR)/mqtt \
        $(AMAZON_FREERTOS_LIB_DIR)/shadow \
        $(AMAZON_FREERTOS_LIB_DIR)/greengrass \
        $(AMAZON_FREERTOS_LIB_DIR)/utils \
        $(AMAZON_FREERTOS_LIB_DIR)/bufferpool \
        $(AMAZON_FREERTOS_LIB_DIR)/tls \
        $(AMAZON_FREERTOS_LIB_DIR)/ota \
        $(AMAZON_FREERTOS_LIB_DIR)/crypto \
        $(AMAZON_FREERTOS_LIB_DIR)/wifi/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/ota/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/secure_sockets/portable/freertos_plus_tcp \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/portable/espressif/esp32_devkitc_esp_wrover_kit \
        $(AMAZON_FREERTOS_LIB_DIR)/pkcs11/mbedtls \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/logging \
        $(AMAZON_FREERTOS_DEMOS_DIR)/common/devmode_key_provisioning \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/NetworkInterface/esp32 \
        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn \
        $(AMAZON_FREERTOS_LIB_DIR)/third_party/tinycbor

COMPONENT_ADD_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/include \
                             $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/Compiler/GCC \
                             $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/include \
                             $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/include/portable/espressif/esp32_devkitc_esp_wrover_kit \
                             $(AMAZON_FREERTOS_LIB_DIR)/third_party/jsmn \
                             $(AMAZON_FREERTOS_LIB_DIR)/third_party/tinycbor

COMPONENT_OBJEXCLUDE := $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-TCP/source/portable/BufferManagement/BufferAllocation_1.o \
                        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.o \
                        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.o \
                        $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.o

ifndef AMAZON_FREERTOS_ENABLE_UNIT_TEST
#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
ifeq ("commented","out")
#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====
COMPONENT_SRCDIRS += ../.. \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/demo_runner \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/shadow \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/mqtt \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/greengrass_connectivity \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/tcp \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/ota \

#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
endif

# Upstream component.mk adds "../../" as a COMPONENT_SRCDIRS which causes main.c in that directory to be built. We only want our own main file to be built.
COMPONENT_SRCDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/common/demo_runner \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/shadow \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/mqtt \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/greengrass_connectivity \
    $(AMAZON_FREERTOS_DEMOS_DIR)/common/tcp \


# ROOMMATE_BOARD include must go before demos/common/include in order to get our aws_clientcredential headers:
ROOMMATE_BOARD_INCLUDE = ../$(TARGET_BOARD_FROM_ROOT)
COMPONENT_ADD_INCLUDEDIRS += $(ROOMMATE_BOARD_INCLUDE)

#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====
COMPONENT_ADD_INCLUDEDIRS += $(AMAZON_FREERTOS_DEMOS_DIR)/common/include
endif

COMPONENT_PRIV_INCLUDEDIRS := $(AMAZON_FREERTOS_LIB_DIR)/third_party/pkcs11 $(AMAZON_FREERTOS_LIB_DIR)/ota/portable/espressif/esp32_devkitc_esp_wrover_kit

#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
ifeq ("commented","out")
#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====
lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
demos/common/logging/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized
demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format
#### ==== vvvv BEGIN CUSTOMIZATION vvvv ====
endif

amazon-freertos/lib/greengrass/aws_greengrass_discovery.o: CFLAGS+=-Wno-format
amazon-freertos/demos/common/logging/aws_logging_task_dynamic_buffers.o: CFLAGS+=-Wno-format -Wno-uninitialized
amazon-freertos/demos/common/tcp/aws_tcp_echo_client_single_task.o: CFLAGS+=-Wno-format
#### ==== ^^^^ END CUSTOMIZATION ^^^^ ====