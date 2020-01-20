
NINJAFILE = build/build.ninja
OUTPUT_BINARY = build/roommate.bin

all: $(OUTPUT_BINARY)
cmake: $(NINJAFILE)

clean:
	rm -rf build

AMAZON_FREERTOS_PATH = amazon-freertos
IDF_PATH = $(AMAZON_FREERTOS_PATH)/vendors/espressif/esp-idf

roommate-help:
	@echo "Roommate build system. Example targets:"
	@echo ""
	@echo "make setup-aws                       - run SetupAWS.py (fill out configure.json first!)"
	@echo "make save-new-board BOARD=john-home  - after calling SetupAWS.py, save its results as 'john-home'"
	@echo "make use-board BOARD=foo             - switch to board 'foo'"
	@echo "make print-BOARD                     - print current board name"
	@echo "make print-TARGET_BOARD              - print current board config directory"
	@echo "make monitor-without-reboot          - connect a serial monitor without rebooting the board (quit with CTRL+A, :quit)"


$(NINJAFILE):
	cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$(AMAZON_FREERTOS_PATH)/tools/cmake/toolchains/xtensa-esp32.cmake -GNinja

$(OUTPUT_BINARY): $(NINJAFILE)
	cmake --build build

BAUDRATE = 1843200

flash: $(NINJAFILE)
	ESPBAUD=$(BAUDRATE) ESPPORT=/dev/cu.SLAB_USBtoUART cmake --build build --target flash

monitor:
	ESPPORT=/dev/cu.SLAB_USBtoUART $(IDF_PATH)/tools/idf.py monitor

# ======== INITIAL SETUP ========

QUICK_START_FROM_FREERTOS = tools/aws_config_quick_start
QUICK_START = $(AMAZON_FREERTOS_PATH)/$(QUICK_START_FROM_FREERTOS)

config-aws:
	@echo "Enter your network credentials in: ../amazon-freertos/$(QUICK_START_FROM_FREERTOS)/configure.json"
	@echo "Valid network types are:"
	@echo "eWiFiSecurityOpen"
	@echo "eWiFiSecurityWEP"
	@echo "eWiFiSecurityWPA"
	@echo "eWiFiSecurityWPA2"

setup-aws:
	cd $(QUICK_START) && python SetupAWS.py setup

# # ======== ROOMMATE BOARD STUFF ========

# out-of-repo directory to contain all the board configs.
ROOMMATE_BOARDS_FROM_ROOT = ../roommate-boards
ROOMMATE_BOARDS := $(CURDIR)/$(ROOMMATE_BOARDS_FROM_ROOT)

# The idea is that you can keep several directories in ../roommate-boards, e.g.:
# ../roommate-boards/john-home
# ../roommate-boards/john-work
# and then whichever one is currently active will be symlinked to ../roommate-boards/current-board-dir .
# Note that there is a hardcoded reference to it in CMakeLists.txt .

CURRENT_BOARD_DIR = $(ROOMMATE_BOARDS)/current-board-dir

CLIENTCRED_H_SRC = $(AMAZON_FREERTOS_PATH)/demos/include/aws_clientcredential.h
CLIENTCRED_KEYS_H_SRC = $(AMAZON_FREERTOS_PATH)/demos/include/aws_clientcredential_keys.h 
CONFIGURE_JSON_SRC = $(QUICK_START)/configure.json

# # pull THING_NAME out of configure.json:
THING_NAME = $(shell cat $(CONFIGURE_JSON_SRC)  |grep thing_name |sed 's/ *"thing_name": *"\(.*\)",/\1/')

# # this allows you to print a variable with e.g. make print-FOO
# # https://stackoverflow.com/a/25817631/202907
print-%  : ; @echo $* = $($*)

CERT_ID_SRC = $(QUICK_START)/$(THING_NAME)_cert_id_file
CERT_PEM_SRC = $(QUICK_START)/$(THING_NAME)_cert_pem_file
KEY_PEM_SRC = $(QUICK_START)/$(THING_NAME)_private_key_pem_file


# invoke e.g. make save-new-board BOARD=george-work
save-new-board:
	echo saving board $(BOARD)
	mkdir -p $(ROOMMATE_BOARDS)/$(BOARD)
	cp $(CLIENTCRED_H_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	cp $(CLIENTCRED_KEYS_H_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	cp $(CONFIGURE_JSON_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	cp $(CERT_ID_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	cp $(CERT_PEM_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	cp $(KEY_PEM_SRC) $(ROOMMATE_BOARDS)/$(BOARD)/
	echo "symlinking $(BOARD) to current"
	ln -sf $(ROOMMATE_BOARDS)/$(BOARD) $(CURRENT_BOARD_DIR)

use-board:
	echo "symlinking $(BOARD) to current"
	ln -sf $(ROOMMATE_BOARDS)/$(BOARD) $(CURRENT_BOARD_DIR)

# https://esp32.com/viewtopic.php?t=1936
monitor-without-reboot:
	screen /dev/cu.SLAB_USBtoUART 115200

# ======== END BOARD STUFF ========
