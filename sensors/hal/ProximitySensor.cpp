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

#include "ProximitySensor.h"
#include "sensors.h"

#define EVENT_TYPE_PROXIMITY            ABS_DISTANCE

#define PROXIMITY_THRESHOLD                     5.0f
/*****************************************************************************/

enum input_device_name {
    GENERIC_PSENSOR = 0,
    LEGACY_PSENSOR,
    CM36283_PS,
    STK3x1x_PS,
    LTR559_PS,
    SUPPORTED_PSENSOR_COUNT,
};

static const char *data_device_name[] = {
    [GENERIC_PSENSOR] = "proximity",
    [LEGACY_PSENSOR] = "proximity",
    [CM36283_PS] = "cm36283-ps",
    [STK3x1x_PS] = "stk3x1x-ps",
    [LTR559_PS] = "ltr559-ps",
};

static const char *input_sysfs_path_list[] = {
    /* This is not used by generic HAL. Just for back compatibility */
    [GENERIC_PSENSOR] = "/sys/class/input/%s/device/",
    [LEGACY_PSENSOR] = "/sys/class/input/%s/device/",
    [CM36283_PS] = "/sys/class/input/%s/device/",
    [STK3x1x_PS] = "/sys/class/input/%s/device/",
    [LTR559_PS] = "/sys/class/input/%s/device/",
};

static const char *input_sysfs_enable_list[] = {
    [GENERIC_PSENSOR] = "driver/enable",
    [LEGACY_PSENSOR] = "enable",
    [CM36283_PS] = "enable",
    [STK3x1x_PS] = "enable",
    [LTR559_PS] = "enable",
};


ProximitySensor::ProximitySensor()
: SensorBase(NULL, NULL),
mEnabled(0),
mInputReader(4),
mHasPendingEvent(false),
sensor_index(-1) {
    int i;
    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    mPendingFlushFinishEvent = 0;
#endif

    for (i = 0; i < SUPPORTED_PSENSOR_COUNT; i++) {
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

    ALOGI("The proximity sensor path is %s", input_sysfs_path);
}

ProximitySensor::ProximitySensor(char *name)
: SensorBase(NULL, NULL),
mEnabled(0),
mInputReader(4),
mHasPendingEvent(false),
sensor_index(-1) {
    int i;

    mPendingEvent.version = sizeof(sensors_event_t);
    mPendingEvent.sensor = SENSORS_PROXIMITY_HANDLE;
    mPendingEvent.type = SENSOR_TYPE_PROXIMITY;
    memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    mPendingFlushFinishEvent = 0;
#endif

    for (i = 0; i < SUPPORTED_PSENSOR_COUNT; i++) {
        data_name = data_device_name[i];

        // data_fd is not initialized if data_name passed
        // to SensorBase is NULL.
        data_fd = openInput(data_name);
        if (data_fd > 0) {
            sensor_index = i;
            break;
        }
    }

    if (data_fd) {
        strlcpy(input_sysfs_path, SYSFS_CLASS, sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, name, sizeof(input_sysfs_path));
        strlcat(input_sysfs_path, "/", sizeof(input_sysfs_path));
        input_sysfs_path_len = strlen(input_sysfs_path);
        ALOGI("The proximity sensor path is %s", input_sysfs_path);
        enable(0, 1);
    }
}

ProximitySensor::~ProximitySensor() {
    if (mEnabled) {
        enable(0, 0);
    }
}

int ProximitySensor::setInitialState() {
    struct input_absinfo absinfo;
    if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_PROXIMITY), &absinfo)) {
        // make sure to report an event immediately
        mHasPendingEvent = true;
        mPendingEvent.distance = indexToValue(absinfo.value);
    }
    return 0;
}

int ProximitySensor::enable(int32_t, int en) {
    int flags = en ? 1 : 0;
    if (flags != mEnabled) {
        int fd;
        if (sensor_index >= 0) {
            strlcpy(&input_sysfs_path[input_sysfs_path_len], input_sysfs_enable_list[sensor_index],
                    sizeof(input_sysfs_path) - input_sysfs_path_len);
        } else {
            ALOGE("invalid sensor index:%d\n", sensor_index);
            return -1;
        }
        fd = open(input_sysfs_path, O_RDWR);
        if (fd >= 0) {
            char buf[2];
            buf[1] = 0;
            if (flags) {
                buf[0] = '1';
            } else {
                buf[0] = '0';
            }
            write(fd, buf, sizeof(buf));
            close(fd);
            mEnabled = flags;
            if (mEnabled)
                setInitialState();
            return 0;
        } else {
            ALOGE("open %s failed.(%s)\n", input_sysfs_path, strerror(errno));
            return -1;
        }
    }
    return 0;
}

bool ProximitySensor::hasPendingEvents() const {
    return mHasPendingEvent || mPendingFlushFinishEvent;
}

int ProximitySensor::readEvents(sensors_event_t *data, int count) {
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
        flush_finish_event.meta_data.sensor = SENSORS_PROXIMITY_HANDLE;
        int pEvents = mPendingFlushFinishEvent;
        while (mPendingFlushFinishEvent) {
            PINFO("<BST> " "report flush finish event for sensor id: %d", flush_finish_event.meta_data.sensor);
            *data++ = flush_finish_event;
            count--;
            numEventReceived++;
            mPendingFlushFinishEvent--;
        }
        if (pEvents) return pEvents;
    }
#endif
    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;


    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            if (event->code == EVENT_TYPE_PROXIMITY) {
                if (event->value != -1) {
                    // FIXME: not sure why we're getting -1 sometimes
                    mPendingEvent.distance = indexToValue(event->value);
                }
            }
        } else if (type == EV_SYN) {
            mPendingEvent.timestamp = timevalToNano(event->time);
            if (mEnabled) {
                *data++ = mPendingEvent;
                count--;
                numEventReceived++;
            }
        } else {
            ALOGE("ProximitySensor: unknown event (type=%d, code=%d)",
                  type, event->code);
        }
        mInputReader.next();
    }

    return numEventReceived;
}

float ProximitySensor::indexToValue(size_t index) const {
    return index * PROXIMITY_THRESHOLD;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__

int ProximitySensor::flush(int id) {
    int ret = 0;
    mPendingFlushFinishEvent++;
    PINFO("<BST> flush sensor id: %d for %d times", id, mPendingFlushFinishEvent);
    return ret;
}

int ProximitySensor::batch(int id, int flags, int64_t period_ns, int64_t timeout) {
    int ret = 0;

    PDEBUG("batch id %d, flags %d, period_ns %lld, timeout %lld", id, flags, period_ns, timeout);

    UNUSED_PARAM(id);
    UNUSED_PARAM(flags);
    UNUSED_PARAM(period_ns);
    UNUSED_PARAM(timeout);
    //ret = ProximitySensor::setDelay(id, period_ns);  // add it will cause proximity fail
    return ret;
}

#endif
