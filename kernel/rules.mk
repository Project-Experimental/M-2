LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	lib/libc \
	lib/heap \
	lib/libcpp

MODULE_SRCS := \
	$(LOCAL_DIR)/debug.cc \
	$(LOCAL_DIR)/event.cc \
	$(LOCAL_DIR)/init.cc \
	$(LOCAL_DIR)/mutex.cc \
	$(LOCAL_DIR)/thread.cc \
	$(LOCAL_DIR)/timer.cc \
	$(LOCAL_DIR)/semaphore.cc \
	$(LOCAL_DIR)/mp.cc \
	$(LOCAL_DIR)/port.cc

ifeq ($(WITH_KERNEL_VM),1)
MODULE_DEPS += kernel/vm
else
MODULE_DEPS += kernel/novm
endif

MODULE_OPTIONS := extra_warnings

include make/module.mk
