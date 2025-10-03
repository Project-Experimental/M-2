LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	app \
	arch \
	dev \
	kernel \
	platform \
	target

MODULE_SRCS := \
	$(LOCAL_DIR)/debug.cc \
	$(LOCAL_DIR)/init.cc \
	$(LOCAL_DIR)/main.cc \

MODULE_OPTIONS := extra_warnings

include make/module.mk
