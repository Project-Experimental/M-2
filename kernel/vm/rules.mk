LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/bootalloc.cc \
	$(LOCAL_DIR)/pmm.cc \
	$(LOCAL_DIR)/vm.cc \
	$(LOCAL_DIR)/vmm.cc \

MODULE_OPTIONS := extra_warnings

include make/module.mk
