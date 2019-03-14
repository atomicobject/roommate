# Roommate - Embedded

_Roommate_ is a project to create a conference room gadget that displays availability, takes impromptu reservations, etc.

This repo contains code for the embedded device, an [Espressif ESP32](https://aws.amazon.com/blogs/apn/connect-microcontroller-based-devices-to-the-cloud-with-amazon-freertos-and-espressif-esp32/) running [Amazon FreeRTOS](https://github.com/aws/amazon-freertos).

| Embedded Device                                                                                                           | Cloud Backend                                                                                                                           |
| ------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------- |
| [atomicobject/roommate](https://github.com/atomicobject/roommate)                                                         | [atomicobject/roommate-lambda](https://github.com/atomicobject/roommate-lambda)                                                         |
| [![CircleCI](https://circleci.com/gh/atomicobject/roommate.svg?style=svg)](https://circleci.com/gh/atomicobject/roommate) | [![CircleCI](https://circleci.com/gh/atomicobject/roommate-lambda.svg?style=svg)](https://circleci.com/gh/atomicobject/roommate-lambda) |

## Clone (with submodules)

`git clone --recursive git@github.com:atomicobject/roommate.git`

## Initial Setup

We're using [Amazon FreeRTOS](https://github.com/aws/amazon-freertos) as a (pristine) submodule, which makes this a little tricky.

1. Edit _amazon-freertos/tools/aws_config_quick_start/configure.json_ (documented in the adjacent [README.md](amazon-freertos/tools/aws_config_quick_start/README.md))

2. From the _make_ directory, `make setup-aws` to perform freertos' initial setup (consumes configure.json, generates headers, pulls down certs, etc.)

3. `make save-new-board BOARD=a-great-board-name` to save a board profile and select it as default

You may have noticed that the setup-aws step _modifies versioned files_ in the submodule. After saving the board, you can discard changes to the submodule.

You can repeat the above process for as many boards/environments as you choose, e.g. in order to have different board configurations for different wifi networks.

Use `make use-board BOARD=a-great-board-name` to switch to another (already-saved) board profile.

Usage information is available via `make roommate-help`.

## Build and Run

To build and flash
`cd make` then
`make flash monitor`
