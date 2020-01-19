## V2 Branch

Starting over with new structure after https://github.com/aws/amazon-freertos/issues/832


new approach:
- start with https://github.com/tgsong/amazon-freertos-examples
- new tooling/documentation for reusable board configs outside the git repo
- add custom component to house roommate logic (start with hello world)
- add custom sdkconfigs
- add roommate application code

Currently targeting a specific commit of AFR because https://github.com/aws/amazon-freertos/issues/1652 :
https://github.com/aws/amazon-freertos/pull/1655/commits/681310d5e21be2db9b101961a495d3e23053a79c

# Building

- https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html#config-demos
- ...

# Amazon FreeRTOS Examples

This repo shows how to set up a CMake project for ESP32 and use amazon freertos as an external library.

See the official document for software prerequisite, https://docs.aws.amazon.com/freertos/latest/userguide/getting_started_espressif.html

Build instruction,

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake -GNinja
```

To flash,

```sh
cmake --build build --target flash
```

To monitor the output,

```sh
export ESPPORT <replace-with-serial-port-of-ESP32>
amazon-freertos/vendors/espressif/esp-idf/tools/idf.py monitor
```
