#!/bin/bash

# IDF_SDK_DEFAULTS needs an absolute path for `ninja menuconfig`
# https://github.com/aws/amazon-freertos/issues/832#issuecomment-516236030
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake -DIDF_SDKCONFIG_DEFAULTS=`pwd`/sdkconfig.my.defaults -G Ninja
# cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=amazon-freertos/tools/cmake/toolchains/xtensa-esp32.cmake -G Ninja

cd build 
ninja my_app
