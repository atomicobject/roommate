
AMAZON_FREERTOS_LIB_DIR := ../amazon-freertos/lib
ORIGINAL_FREERTOS_DIR := ../amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/freertos

COMPONENT_ADD_INCLUDEDIRS := $(ORIGINAL_FREERTOS_DIR)/include ../config_files $(AMAZON_FREERTOS_LIB_DIR)/include $(AMAZON_FREERTOS_LIB_DIR)/include/private $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32/include
COMPONENT_SRCDIRS := $(ORIGINAL_FREERTOS_DIR) $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS/portable/ThirdParty/GCC/Xtensa_ESP32 $(AMAZON_FREERTOS_LIB_DIR)/FreeRTOS
COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority