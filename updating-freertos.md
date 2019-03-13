## Updating Amazon FreeRTOS

We keep FreeRTOS in a clean submodule with no local changes[^1] and minimal duplication of its files in our own repo. 

Our project is based on the espressif demo at `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/`, so it may occasionally be necessary to bring over changes, in particular:
 - Our `src/main.c` is based on `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/main.c`.
 - Several files in our `make/` directory come from `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/make`. 

[^1]: Our `make/Makefile` renames `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/main.c` to exclude it (the demo app's main) from our build, in order for us to supply our own main. Hopefully upstream [fixes this](https://github.com/aws/amazon-freertos/issues/367) someday.
