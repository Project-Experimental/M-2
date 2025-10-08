# mostly null target configuration for pc-x86
LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

PLATFORM := pc

MODULE_SRCS += \
	$(LOCAL_DIR)/config.cc

include make/module.mk

