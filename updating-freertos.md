## Updating Amazon FreeRTOS

We keep FreeRTOS in a clean submodule with no local changes. We've managed to construct a build environment entirely outside of it, but it required copying a few files, which causes a bit of pain when it's time to integrate changes from upstream.

Notes:
- Our `config_files` directory is a (modified) copy of `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/config_files`
  - To update it, delete our directory, re-copy upstream's, and then use git to carefully restore our changes. (e.g., discard the deletion of our changes)
- Our `freertos-common-component/component.mk` is a more-heavily modified copy of `amazon-freertos/demos/espressif/esp32_devkitc_esp_wrover_kit/common/application_code/espressif_code/amazon-freertos-common/component.mk`.
  - There are some maintenance notes in our copy.
