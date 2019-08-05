cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake -G Ninja

and then `ninja` from build/

erase with:
./amazon-freertos/vendors/espressif/esp-idf/tools/idf.py erase_flash -p /dev/cu.SLAB_USBtoUART

flash with (from build dir):
ESPBAUD=1843200 ESPPORT=/dev/cu.SLAB_USBtoUART ninja flash

monitor with:
./amazon-freertos/vendors/espressif/esp-idf/tools/idf.py monitor -p /dev/cu.SLAB_USBtoUART -B build
