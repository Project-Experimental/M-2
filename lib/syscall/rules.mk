LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS :=

MODULE_SRCS += $(LOCAL_DIR)/system_call.cc \
			   $(LOCAL_DIR)/syscall.s

include make/module.mk