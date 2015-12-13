ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH := $(call my-dir)
REAL_LOCAL_PATH:=$(LOCAL_PATH)
include $(CLEAR_VARS)

LOCAL_INCLUDES += $(LOCAL_PATH)

LOCAL_SRC_FILES := \
    ../version/version.c \
    src/main.c \
    src/misc.c \
    src/trace.c \
    src/sensor_cfg.c \
    src/event_handler.c \
    src/lib/util_misc.c \
    src/lib/util_time.c \
    src/lib/util_sysfs.c \
    src/lib/util_input_dev.c \
    src/channel_cntl.c \
    src/channel_a.c \
    src/channel_g.c \
    src/channel_m.c \
    src/channel_o.c \
    src/channel_vg.c \
    src/channel_vla.c \
    src/channel_vrv.c \
    src/channel_grv.c \
    src/channel_gu.c \
    src/channel_mu.c \
    src/channel_geom_rv.c \
    src/channel_stc.c \
    src/channel_std.c \
    src/channel_sgm.c \
    src/channels.c \
    src/sensor_provider.c \
    src/sensor_fusion.c \
    src/hw/hw_cntl.c

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    $(LOCAL_PATH)/inc \
                    $(LOCAL_PATH)/../version \
                    $(LOCAL_PATH)/algo/inc \
                    $(LOCAL_PATH)/src/algo \
                    $(LOCAL_PATH)/src/hw \

ifeq (arm,$(TARGET_ARCH))
LOCAL_LDFLAGS += $(LOCAL_PATH)/algo/lib/bosch_lib32/libalgobsx.a
else
LOCAL_LDFLAGS += $(LOCAL_PATH)/algo/lib/bosch_lib64/libalgobsx.a
endif

LOCAL_LDLIBS := -llog
LOCAL_SHARED_LIBRARIES += liblog libcutils libc libm
#LOCAL_STATIC_LIBRARIES += libalgobsx

include $(addprefix $(LOCAL_PATH)/,$(addsuffix /Android.mk, \
    src/algo \
    src/hw/a \
    src/hw/m \
    src/hw/g \
       ))

-include $(LOCAL_PATH)/src/hw/Android.mk

LOCAL_MODULE_TAGS := optional
ifneq (,$(sensord_suffix))
LOCAL_MODULE = sensord.$(sensord_suffix)
else
LOCAL_MODULE = sensord
endif

include $(LOCAL_PATH)/../tools/moto_config.mk

include $(LOCAL_PATH)/../tools/options.mk

ifeq (true, $(flip_gesture_support))
LOCAL_SRC_FILES += src/channel_gest_flip.c
LOCAL_CFLAGS += -D__FLIP_GESTURE__
endif

LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
include $(BUILD_EXECUTABLE)

endif  # TARGET_SIMULATOR != true
