LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
    $(LOCAL_DIR)/cache_tests.cc \
    $(LOCAL_DIR)/cbuf_tests.cc \
    $(LOCAL_DIR)/clock_tests.cc \
    $(LOCAL_DIR)/fibo.cc \
    $(LOCAL_DIR)/mem_tests.cc \
    $(LOCAL_DIR)/tests.cc \
    $(LOCAL_DIR)/thread_tests.cc \
    $(LOCAL_DIR)/port_tests.cc \
    $(LOCAL_DIR)/v9p_tests.cc \
    $(LOCAL_DIR)/v9fs_tests.cc \

MODULE_FLOAT_SRCS := \
    $(LOCAL_DIR)/benchmarks.cc \
    $(LOCAL_DIR)/float.cc \
    $(LOCAL_DIR)/float_instructions.S \

MODULE_DEPS += \
    lib/cbuf

MODULE_COMPILEFLAGS += -fno-builtin

include make/module.mk
