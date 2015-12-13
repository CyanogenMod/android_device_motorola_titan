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

#ifndef ANDROID_PROXIMITY_SENSOR_H
#define ANDROID_PROXIMITY_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"

/*****************************************************************************/

struct input_event;

class ProximitySensor : public SensorBase {
int mEnabled;
InputEventCircularReader mInputReader;
sensors_event_t mPendingEvent;
bool mHasPendingEvent;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
uint32_t mPendingFlushFinishEvent;
#endif
char input_sysfs_path[PATH_MAX];
int input_sysfs_path_len;
int sensor_index;

int setInitialState();
float indexToValue(size_t index) const;

public:
ProximitySensor();
ProximitySensor(char *name);
virtual ~ProximitySensor();
virtual int readEvents(sensors_event_t* data, int count);
virtual bool hasPendingEvents() const;
virtual int enable(int32_t handle, int enabled);

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
virtual int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
virtual int flush(int handle);
#endif
};

/*****************************************************************************/

#endif  // ANDROID_PROXIMITY_SENSOR_H