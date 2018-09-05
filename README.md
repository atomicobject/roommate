# Roommate - Embedded
AO Conference Room IoT Project

## CI
[![CircleCI](https://circleci.com/gh/atomicobject/roommate.svg?style=svg&circle-token=51df136e9f481a8f75c9b775bcf2f5abe44b9dc9)](https://circleci.com/gh/atomicobject/roommate)

## Clone (with submodules)
`git clone --recursive git@github.com:atomicobject/roommate.git`

## Setup
`make config-aws` to see path to configure.json
Update the json file with wifi credentials and network type.
`make setup-aws` to update some project header files with your network information and pull down certs, policies, etc.
`make save-new-board BOARD=a-great-board-name` to save a board profile
You can repeat the above process for as many boards/environments as you choose
Use `make use-board BOARD=a-great-board-name` to begin using a specific board profile

## Build and Run
To build and flash
`cd make` then
`make flash monitor`

