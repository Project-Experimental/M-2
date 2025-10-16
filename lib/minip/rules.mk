LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	lib/cbuf \
	lib/iovec \
	lib/libcpp \
	lib/pool

MODULE_SRCS += \
	$(LOCAL_DIR)/arp.cc \
	$(LOCAL_DIR)/chksum.cc \
	$(LOCAL_DIR)/dhcp.cc \
	$(LOCAL_DIR)/lk_console.cc \
	$(LOCAL_DIR)/minip.cc \
	$(LOCAL_DIR)/net_timer.cc \
	$(LOCAL_DIR)/pktbuf.cc \
	$(LOCAL_DIR)/tcp.cc \
	$(LOCAL_DIR)/udp.cc

include make/module.mk
