# ==========================================
#  This makefile originated from:
#   Unity Project - A Test Framework for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

ifndef AMAZON_FREERTOS_PATH
export AMAZON_FREERTOS_PATH := $(CURDIR)/../amazon-freertos
endif

CLEANUP = rm -f
MKDIR = mkdir -p
TARGET_EXTENSION=.out
TEST_SRC = test_src

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif
UNITY_ROOT=${AMAZON_FREERTOS_PATH}/libraries/3rdparty/unity
CFLAGS=-std=c99
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition

ROOMMATE_SRC_ROOT = ../src
ROOMMATE_INCLUDES = ${ROOMMATE_SRC_ROOT}/include
ROOMMATE_SRC_FILES=\
	${ROOMMATE_SRC_ROOT}/led_sequence.c \
	${ROOMMATE_SRC_ROOT}/cal_util.c

TARGET_BASE1=all_tests
TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
TEST_SRC_FILES=\
  $(UNITY_ROOT)/src/unity.c \
  $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c \
  ${TEST_SRC}/cal_util_tests.c \
  ${TEST_SRC}/test_runners/cal_util_runner.c \
  ${TEST_SRC}/led_logic_tests.c \
  ${TEST_SRC}/test_runners/led_logic_runner.c \
  ${TEST_SRC}/led_sequence_tests.c \
  ${TEST_SRC}/test_runners/led_sequence_runner.c \
  ${TEST_SRC}/test_runners/all_tests.c
INC_DIRS=-I${TEST_SRC}/../fake_headers -I${ROOMMATE_INCLUDES} -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src
SYMBOLS=

all: clean default

default:
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(TEST_SRC_FILES) $(ROOMMATE_SRC_FILES) -o $(TARGET1)
	- ./$(TARGET1) -v

clean:
	$(CLEANUP) $(TARGET1)

ci: CFLAGS += -Werror
ci: default