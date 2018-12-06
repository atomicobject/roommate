# Roommate - Embedded

AO Conference Room IoT Project

## CI

[![CircleCI](https://circleci.com/gh/atomicobject/roommate.svg?style=svg&circle-token=51df136e9f481a8f75c9b775bcf2f5abe44b9dc9)](https://circleci.com/gh/atomicobject/roommate)

## Clone (with submodules)

`git clone --recursive git@github.com:atomicobject/roommate.git`

## Initial Setup

We're using [Amazon FreeRTOS](https://github.com/aws/amazon-freertos) as a (pristine) submodule, which makes this a little tricky.

1. edit _amazon-freertos/tools/aws_config_quick_start/configure.json_ (documented in amazon-freertos/tools/aws_config_quick_start/README.md)

2. From the _make_ directory, `make setup-aws` to perform freertos' initial setup (consumes configure.json, generates headers, pulls down certs, etc.)

3. `make save-new-board BOARD=a-great-board-name` to save a board profile and select it as default

You can repeat the above process for as many boards/environments as you choose
Use `make use-board BOARD=a-great-board-name` to begin using a specific board profile

## Build and Run

To build and flash
`cd make` then
`make flash monitor`
