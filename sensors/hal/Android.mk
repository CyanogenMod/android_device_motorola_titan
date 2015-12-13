# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

REAL_LOCAL_PATH := $(LOCAL_PATH)

ifneq ($(TARGET_SIMULATOR),true)

# HAL module implemenation, not prelinked, and stored in
# hw/<SENSORS_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

ifeq ($(origin TARGET_BOARD_PLATFORM), undefined)
LOCAL_MODULE := sensors.default
else
LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)
endif

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_RELATIVE_PATH := hw
else
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := \
    -D LOG_TAG=\"bsthal\" \
    -Wall

LOCAL_SRC_FILES := \
    ../version/version.c \
    BstSensor.cpp \
    sensors.cpp \
    InputEventReader.cpp \
    SensorBase.cpp \
    LightSensor.cpp \
    ProximitySensor.cpp \
    BstSensorInfo.cpp

LOCAL_C_INCLUDES = $(LOCAL_PATH)/../version

#LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_PRELINK_MODULE := false

include $(LOCAL_PATH)/../tools/moto_config.mk

include $(LOCAL_PATH)/../tools/options.mk

ifeq (true, $(bmp_support))
LOCAL_SRC_FILES += BstSensorPressure.cpp
endif

ifeq (true, $(bst_extend_sensor_support))
LOCAL_SRC_FILES += BstSensorExt.cpp
endif

ifeq (true, $(hybrid_hal))
LOCAL_SRC_FILES += BstSensorAccel.cpp
endif

include $(BUILD_SHARED_LIBRARY)

endif # !TARGET_SIMULATOR
