# Roommate - Embedded

AO Conference Room IoT Project

| Embedded Device | Cloud Backend |
| --------------- | ------------- |
| [atomicobject/roommate](https://github.com/atomicobject/roommate) | [atomicobject/roommate-lambda](https://github.com/atomicobject/roommate-lambda) |
| [![CircleCI](https://circleci.com/gh/atomicobject/roommate.svg?style=svg)](https://circleci.com/gh/atomicobject/roommate) | [![CircleCI](https://circleci.com/gh/atomicobject/roommate-lambda.svg?style=svg)](https://circleci.com/gh/atomicobject/roommate-lambda) |


## Clone (with submodules)

`git clone --recursive git@github.com:atomicobject/roommate.git`

## Initial Setup

We're using [Amazon FreeRTOS](https://github.com/aws/amazon-freertos) as a (pristine) submodule, which makes this a little tricky.

1. edit _amazon-freertos/tools/aws_config_quick_start/configure.json_ (documented in amazon-freertos/tools/aws_config_quick_start/README.md)

2. From the _make_ directory, `make setup-aws` to perform freertos' initial setup (consumes configure.json, generates headers, pulls down certs, etc.)

3. `make save-new-board BOARD=a-great-board-name` to save a board profile and select it as default

You can repeat the above process for as many boards/environments as you choose
Use `make use-board BOARD=a-great-board-name` to begin using a specific board profile

Usage information is available via `make roommate-help`.

## Build and Run

To build and flash
`cd make` then
`make flash monitor`
