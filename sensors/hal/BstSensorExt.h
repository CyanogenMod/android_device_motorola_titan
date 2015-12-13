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

#ifndef __BST_SENSOR_EXT_H
#define __BST_SENSOR_EXT_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"
#include "BstSensorType.h"

/*****************************************************************************/

/* interruption enum, should same as defined in sensor linux driver */
#define LOW_G_INTERRUPT    REL_Z
#define HIGH_G_INTERRUPT    REL_HWHEEL
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	#define SIGNIFICANT_MOTION_INTERRUPT    REL_DIAL
#else
	#define SLOP_INTERRUPT    REL_DIAL
#endif
#define DOUBLE_TAP_INTERRUPT    REL_WHEEL
#define SINGLE_TAP_INTERRUPT    REL_MISC
#define ORIENT_INTERRUPT    ABS_PRESSURE
#define FLAT_INTERRUPT    ABS_DISTANCE
#define SLOW_NO_MOTION_INTERRUPT    REL_Y
#define HIGH_G_INTERRUPT_X_HAPPENED    1
#define HIGH_G_INTERRUPT_Y_HAPPENED    2
#define HIGH_G_INTERRUPT_Z_HAPPENED    3
#define HIGH_G_INTERRUPT_X_NEGATIVE_HAPPENED    4
#define HIGH_G_INTERRUPT_Y_NEGATIVE_HAPPENED    5
#define HIGH_G_INTERRUPT_Z_NEGATIVE_HAPPENED    6
#define SLOPE_INTERRUPT_X_HAPPENED    7
#define SLOPE_INTERRUPT_Y_HAPPENED    8
#define SLOPE_INTERRUPT_Z_HAPPENED    9
#define SLOPE_INTERRUPT_X_NEGATIVE_HAPPENED    10
#define SLOPE_INTERRUPT_Y_NEGATIVE_HAPPENED    11
#define SLOPE_INTERRUPT_Z_NEGATIVE_HAPPENED    12
#define DOUBLE_TAP_INTERRUPT_HAPPENED    13
#define SINGLE_TAP_INTERRUPT_HAPPENED    14
#define UPWARD_PORTRAIT_UP_INTERRUPT_HAPPENED    15
#define UPWARD_PORTRAIT_DOWN_INTERRUPT_HAPPENED    16
#define UPWARD_LANDSCAPE_LEFT_INTERRUPT_HAPPENED    17
#define UPWARD_LANDSCAPE_RIGHT_INTERRUPT_HAPPENED    18
#define DOWNWARD_PORTRAIT_UP_INTERRUPT_HAPPENED    19
#define DOWNWARD_PORTRAIT_DOWN_INTERRUPT_HAPPENED    20
#define DOWNWARD_LANDSCAPE_LEFT_INTERRUPT_HAPPENED    21
#define DOWNWARD_LANDSCAPE_RIGHT_INTERRUPT_HAPPENED    22
#define FLAT_INTERRUPT_TURE_HAPPENED    23
#define FLAT_INTERRUPT_FALSE_HAPPENED    24
#define LOW_G_INTERRUPT_HAPPENED    25
#define SLOW_NO_MOTION_INTERRUPT_HAPPENED    26


enum BSTEXT_SENSOR_HANDLE {
	BSTEXT_SENSOR_HANDLE_START = 0,
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	BSTEXT_SENSOR_HANDLE_SIGNIFICANT_MOTION,
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	BSTEXT_SENSOR_HANDLE_DOUBLE_TAP,
#endif

	BSTEXT_SENSOR_HANDLE_END
};
#define BSTEXT_SENSOR_NUM_MAX    (BSTEXT_SENSOR_HANDLE_END-1)

#define BST_SENSOR_EXT_INPUT_DEV "bma_interrupt"
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
#define INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR "/sys/class/sig_sensor/g_sensor/"
#define INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR_ENABLE_NODE "en_sig_motion"
#endif

#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
#define INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR "/sys/class/dtap_sensor/g_sensor/"
#define INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR_ENABLE_NODE "en_double_tap"
#endif

struct input_event;

class BstSensorExt : public SensorBase {
	InputEventCircularReader mInputReader;
	sensors_event_t mPendingEvent;

	char input_sysfs_path[PATH_MAX];
	int input_sysfs_path_len;
	static const struct sensor_t g_bstext_sensor_list[];
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	unsigned char mDtapTimes;
#endif

public:
	BstSensorExt();
	virtual ~BstSensorExt();
	virtual int readEvents(sensors_event_t* data, int count);
	virtual int enable(int32_t handle, int enabled);
	static int getSensorList(struct sensor_t *list, int len);
	static const struct sensor_t * getSensor(int handle);
	int enableSensor(int en, char* sensor, char* enable_mode);
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	virtual int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
	virtual int flush(int handle);
#endif

	enum
	{
		BSTEXT_SID_START = -1,
	#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
		BSTEXT_SID_SMT,
	#endif
	#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
		BSTEXT_SID_DTAP,
	#endif
		NUM_BSTEXT_SENSORS
	};

private:
	static int checkSensorAvailable(int sensor_id);
};


/*****************************************************************************/

#endif  // __BST_SENSOR_EXT_H
