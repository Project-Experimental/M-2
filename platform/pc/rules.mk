LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

# two implementations, modern and legacy
# legacy implies older hardware, pre pentium, pre pci
CPU ?= modern

MODULE_DEPS += lib/bio
MODULE_DEPS += lib/cbuf
MODULE_DEPS += lib/fixed_point

ifneq ($(CPU),legacy)
MODULE_DEPS += dev/bus/pci/drivers
MODULE_DEPS += lib/acpi_lite
endif

MODULE_SRCS += \
    $(LOCAL_DIR)/cmos.cc \
    $(LOCAL_DIR)/console.cc \
    $(LOCAL_DIR)/debug.cc \
    $(LOCAL_DIR)/ide.cc \
    $(LOCAL_DIR)/interrupts.cc \
    $(LOCAL_DIR)/keyboard.cc \
    $(LOCAL_DIR)/mp.cc \
    $(LOCAL_DIR)/mp-boot.S \
    $(LOCAL_DIR)/pic.cc \
    $(LOCAL_DIR)/pit.cc \
    $(LOCAL_DIR)/platform.cc \
    $(LOCAL_DIR)/timer.cc \
    $(LOCAL_DIR)/uart.cc \

LK_HEAP_IMPLEMENTATION ?= dlmalloc

GLOBAL_DEFINES += \
	PLATFORM_HAS_DYNAMIC_TIMER=1

include make/module.mk

