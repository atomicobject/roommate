#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := roommate

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../amazon-freertos
endif

ifndef IDF_PATH
export IDF_PATH := $(AMAZON_FREERTOS_PATH)/lib/third_party/mcu_vendor/espressif/esp-idf
endif

EXTRA_COMPONENT_DIRS := $(AMAZON_FREERTOS_PATH)/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/ethernet
EXTRA_COMPONENT_DIRS += $(CURDIR)/../freertos-common-component
EXTRA_COMPONENT_DIRS += $(AMAZON_FREERTOS_PATH)/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/freertos
EXTRA_COMPONENT_DIRS += $(AMAZON_FREERTOS_PATH)/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/mbedtls
EXTRA_COMPONENT_DIRS += $(AMAZON_FREERTOS_PATH)/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/tcpip_adapter
EXTRA_COMPONENT_DIRS += $(CURDIR)/../src

# ======== ROOMMATE BOARD STUFF ========

# out-of-repo directory do contain all the board configs.
# can be overridden with env var:
ROOMMATE_BOARDS ?= $(CURDIR)/../../roommate-boards

roommate_help:
	@echo "Roommate build system. Example targets:"
	@echo ""
	@echo "make save-new-board BOARD=john-home - after calling SetupAWS.py, save its results as 'john-home'"
	@echo "make print-BOARD - print current board name"
	@echo "make use-board BOARD=foo - switch to board 'foo'"
	@echo "make print-TARGET_BOARD - print current board config directory"

CURRENT_BOARD_FILE = $(ROOMMATE_BOARDS)/current-board

# this variable holds the name of the board. It can be specified on the
# command line, but most of the time we want to slurp the value out of
# $(CURRENT_BOARD_FILE)
BOARD = $(shell cat $(CURRENT_BOARD_FILE))

TARGET_BOARD = $(ROOMMATE_BOARDS)/$(BOARD)

CLIENTCRED_H_SRC = $(AMAZON_FREERTOS_PATH)/demos/common/include/aws_clientcredential.h
CLIENTCRED_KEYS_H_SRC = $(AMAZON_FREERTOS_PATH)/demos/common/include/aws_clientcredential_keys.h 

QUICK_START = $(AMAZON_FREERTOS_PATH)/demos/common/tools/aws_config_quick_start
CONFIGURE_JSON_SRC = $(QUICK_START)/configure.json

# pull THING_NAME out of configure.json:
THING_NAME = $(shell cat $(CONFIGURE_JSON_SRC)  |grep thing_name |sed 's/ *"thing_name":"\(.*\)",/\1/')

# this allows you to print a variable with e.g. make print-FOO
# https://stackoverflow.com/a/25817631/202907
print-%  : ; @echo $* = $($*)

# THING_NAME = roommate-john-4
CERT_ID_SRC = $(QUICK_START)/$(THING_NAME)_cert_id_file
CERT_PEM_SRC = $(QUICK_START)/$(THING_NAME)_cert_pem_file
KEY_PEM_SRC = $(QUICK_START)/$(THING_NAME)_private_key_pem_file

CLIENTCRED_H_DST = $(TARGET_BOARD)/aws_clientcredential.h
CLIENTCRED_KEYS_H_DST = $(TARGET_BOARD)/aws_clientcredential_keys.h 

# invoke e.g. make save-new-board BOARD=george-work
save-new-board:
	echo saving board $(BOARD)
	mkdir -p $(TARGET_BOARD)
	cp $(CLIENTCRED_H_SRC) $(TARGET_BOARD)/
	cp $(CLIENTCRED_KEYS_H_SRC) $(TARGET_BOARD)/
	cp $(CONFIGURE_JSON_SRC) $(TARGET_BOARD)/
	cp $(CERT_ID_SRC) $(TARGET_BOARD)/
	cp $(CERT_PEM_SRC) $(TARGET_BOARD)/
	cp $(KEY_PEM_SRC) $(TARGET_BOARD)/
	echo $(BOARD) > $(CURRENT_BOARD_FILE)

use-board:
	echo $(BOARD) > $(CURRENT_BOARD_FILE)

# EXTRA_COMPONENT_DIRS += $(TARGET_BOARD)
# COMPONENT_ADD_INCLUDEDIRS += $(TARGET_BOARD)

# this puts an include path for $(TARGET_BOARD) at a higher precedence
# than demos/common/include/, so that we get our headers instead of
# the versioned ones:
CFLAGS += -I $(TARGET_BOARD)

# ======== END BOARD STUFF ========

include $(IDF_PATH)/make/project.mk
