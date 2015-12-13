/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "sensors.h"
#include "LightSensor.h"

#define EVENT_TYPE_LIGHT                ABS_MISC
/*****************************************************************************/

enum input_device_name {
    GENERIC_LS = 0,
    LIGHTSENSOR_LEVEL,
    CM36283_LS,
    STK3x1x_LS,
    LTR559_LS,
    SUPPORTED_LSENSOR_COUNT,
};

enum {
    TYPE_ADC = 0,
    TYPE_LUX,
};

static const char *data_device_name[] = {
    [GENERIC_LS] = "light",
    [LIGHTSENSOR_LEVEL] = "lightsensor-level",
    [CM36283_LS] = "cm36283-ls",
    [STK3x1x_LS] = "stk3x1x-ls",
    [LTR559_LS] = "ltr559-ls",
};

static const char *input_sysfs_path_list[] = {
    /* This one is for back compatibility, we don't need it for generic HAL.*/
    [GENERIC_LS] = "/sys/class/input/%s/device/",
    [LIGHTSENSOR_LEVEL] = "/sys/class/input/%s/device/",
    [CM36283_LS] = "/sys/class/input/%s/device/",
    [STK3x1x_LS] = "/sys/class/input/%s/device/",
    [LTR559_LS] = "/sys/class/input/%s/device/",
};

static const char *input_sysfs_enable_list[] = {
    [GENERIC_LS] = "enable",
    [LIGHTSENSOR_LEVEL] = "driver/enable",
    [CM36283_LS] = "enable",
    [STK3x1x_LS] = "enable",
    [LTR559_LS] = "enable_als_sensor",
};

static const int input_report_type[] = {
    [GENERIC_LS] = TYPE_LUX,
    [LIGHTSENSOR_LEVEL] = TYPE_LUX,
    [CM36283_LS] = TYPE_LUX,
    [STK3x1x_LS] = TYPE_LUX,
    [LTR559_LS] = TYPE_LUX,
};

LightSensor::LightSensor()
: SensorBase(NULL, NULL),
mEnabled(0),
mInputReader(4),
mHasPendingEvent(false),
sensor_index(-1) {
    int i;
    char *tok = NULL;

    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_LIGHT_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_LIGHT;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    mPendingFlushFinishEvent = 0;
#endif

    for (i = 0; i < SUPPORTED_LSENSOR_COUNT; i++) {
        data_name = data_device_name[i];

        // data_fd is not initialized if data_name passed
        // to SensorBase is NULL.
        data_fd = openInput(data_name);
        if (data_fd > 0) {
            sensor_index = i;
            break;
        }
    }

    if (data_fd > 0) {
        snprintf(input_sysfs_path, sizeof(input_sysfs_path),
                 input_sysfs_path_list[i], input_name);
        input_sysfs_path_len = strlen(input_sysfs_path);
        enable(0, 1);
    }
    ALOGI("The light sensor path is %s", input_sysfs_path);
}

LightSensor::LightSensor(char *name)
: SensorBase(NULL, NULL),
mEnabled(0),
mInputReader(4),
mHasPendingEvent(false),
sensor_index(-1) {
    int i;

    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_LIGHT_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_LIGHT;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    mPendingFlushFinishEvent = 0;
#endif

    for (i = 0; i < SUPPORTED_LSENSOR_COUNT; i++) {
        data_name = data_device_name[i];

        // data_fd is not initialized if data_name passed
        // to SensorBase is NULL.
        data_fd = openInput(data_name);
        if (data_fd > 0) {
            sensor_index = i;
            break;
        }
    }

    if (data_fd > 0) {
        strlcpy(input_sysfs_path, SYSFS_CLASS, sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, name, sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
        input_sysfs_path_len = strlen(input_sysfs_path);
        ALOGI("The light sensor path is %s", input_sysfs_path);
        enable(0, 1);
    }
}

LightSensor::~LightSensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int LightSensor::setDelay(int32_t handle, int64_t ns) {
    int fd;
    int delay_ms = ns / 1000000;
    strlcpy(&input_sysfs_path[input_sysfs_path_len],
            SYSFS_POLL_DELAY, SYSFS_MAXLEN);
    fd = open(input_sysfs_path, O_RDWR);
    if (fd >= 0) {
        char buf[80];
        snprintf(buf, sizeof(buf), "%d", delay_ms);
        write(fd, buf, strlen(buf) + 1);
        close(fd);
        return 0;
    }
    return -1;
}

int LightSensor::enable(int32_t handle, int en) {
    int flags = en ? 1 : 0;
    if (flags != mEnabled) {
        int fd;
        if (sensor_index >= 0) {
            strlcpy(&input_sysfs_path[input_sysfs_path_len],
                    input_sysfs_enable_list[sensor_index], sizeof(input_sysfs_path) - input_sysfs_path_len);
        }
        else {
            ALOGE("invalid sensor index:%d\n", sensor_index);
            return -1;
        }
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
            char buf[2];
            int err;
            buf[1] = 0;
            if (flags) {
                buf[0] = '1';
            } else {
                buf[0] = '0';
            }
            err = write(fd, buf, sizeof(buf));
            close(fd);
            mEnabled = flags;
            return 0;
        } else {
            ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
            return -1;
        }
    }
    return 0;
}

bool LightSensor::hasPendingEvents() const {
    return mHasPendingEvent;
}

int LightSensor::readEvents(sensors_event_t *data, int count) {
    int numEventReceived = 0;
    input_event const *event;
    if (count < 1)
        return -EINVAL;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    sensors_meta_data_event_t flush_finish_event;
#endif

    if (mHasPendingEvent) {
        mHasPendingEvent = false;
        mPendingEvent.timestamp = getTimestamp();
        *data = mPendingEvent;
        return mEnabled ? 1 : 0;
    }

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    if (mEnabled) {
        PINFO("<BST> " "mPendingFlushFinishEvent = %d ", mPendingFlushFinishEvent);
        flush_finish_event.version = META_DATA_VERSION;
        flush_finish_event.type = SENSOR_TYPE_META_DATA;
        flush_finish_event.meta_data.what = META_DATA_FLUSH_COMPLETE;
        flush_finish_event.meta_data.sensor = SENSORS_LIGHT_HANDLE;
        while (mPendingFlushFinishEvent) {
            PINFO("<BST> " "report flush finish event for sensor id: %d", flush_finish_event.meta_data.sensor);
            *data++ = flush_finish_event;
            count--;
            numEventReceived++;
            mPendingFlushFinishEvent--;
        }
    }
#endif
    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;


    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_LIGHT) {
                mPendingEvent.light = convertEvent(event->value);
            }
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                count--;
                numEventReceived++;
            }
        } else {
            ALOGE("LightSensor: unknown event (type=%d, code=%d)",
                  type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

float LightSensor::convertEvent(int value) {
    float lux = 0;

    if (sensor_index >= 0) {
        if (input_report_type[sensor_index] == TYPE_ADC) {
            // Convert adc value to lux assuming:
            // I = 10 * log(Ev) uA
            // R = 47kOhm
            // Max adc value 4095 = 3.3V
            // 1/4 of light reaches sensor
            lux = powf(10, value * (330.0f / 4095.0f / 47.0f)) * 4;
        } else if (input_report_type[sensor_index] == TYPE_LUX) {
            lux = value;
        } else {
            ALOGE("LightSensor: unknown report type\n");
            lux = 0;
        }
    }

    return lux;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__

int LightSensor::flush(int id) {
    int ret = 0;
    mPendingFlushFinishEvent++;
    return ret;
}

int LightSensor::batch(int id, int flags, int64_t period_ns, int64_t timeout) {
    int ret = 0;

    PDEBUG("batch id %d, flags %d, period_ns %lld, timeout %lld", id, flags, period_ns, timeout);

    UNUSED_PARAM(id);
    UNUSED_PARAM(flags);
    UNUSED_PARAM(period_ns);
    UNUSED_PARAM(timeout);
    //ret = LightSensor::setDelay(id, period_ns); // it will cause the ligt sensor fail
    return ret;
}

#endif
