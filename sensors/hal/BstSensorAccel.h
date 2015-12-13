/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2015 Bosch Sensortec GmbH All Rights Reserved
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
 *
 
 *
 * @file         BstSensorAccel.h
 * @date         "Mon Jan 19 13:43:34 2015 +0800"
 * @commit       "e253b7d"
 *
 * @brief
 *
 * @detail
 *
 */


#ifndef __BST_SENSOR_ACCEL_H
#define __BST_SENSOR_ACCEL_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"
#include "BstSensorPriv.h"

struct input_event;

struct sensor_accel_bw {
	int odr;
	int reg_val;
};

class BstSensorAccel: public SensorBase {
public:
	BstSensorAccel(const char* name, SensorBase *delegate = NULL);
	virtual ~BstSensorAccel();
	virtual int readEvents(sensors_event_t* data, int count);
	virtual int setDelay(int32_t handle, int64_t ns);
	virtual int enable(int32_t handle, int enabled);

	//virtual void getSensorSpec(struct bst_sensor_spec *spec) = 0;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	virtual int batch(int handle,int flags,int64_t period_ns,int64_t timeout);
	virtual int flush(int handle);
#endif

	enum {
		HW_A_BW_INVALID = -1,
		/* 0 */
		HW_A_BW_8 = 0,
		/* 1 */
		HW_A_BW_16,
		/* 2 */
		HW_A_BW_31,
		/* 3 */
		HW_A_BW_63,
		/* 4 */
		HW_A_BW_125,
		/* 5 */
		HW_A_BW_250,
		/* 6 */
		HW_A_BW_500,
		/* 7 */
		HW_A_BW_1000,
		/* 8 */
		HW_A_BW_2000,
		/* 9 */
		HW_A_BW_MAX
	};

	enum {
		HW_A_RANGE_INVALID = -1,
		/* 0 */
		HW_A_RANGE_2G,
		/* 1 */
		HW_A_RANGE_4G,
		/* 2 */
		HW_A_RANGE_8G,
		/* 3 */
		HW_A_RANGE_16G,
		/* 4 */
		HW_A_RANGE_32G,
		/* 5 */
		HW_A_RANGE_MAX
	};

	enum {
		BMA_CHIPID_BMA222E = 0XF8,
		BMA_CHIPID_BMA250E = 0XF9,
		BMA_CHIPID_BMA255  = 0XFA,
		BMA_CHIPID_BMA280  = 0XFB,
		BMA_CHIPID_BMA355  = 0XEA,
	};
protected:
	int mEnabled;
	InputEventCircularReader mInputReader;
	sensors_event_t mPendingEvent;
	char mInputSysfsPath[PATH_MAX];
	int mInputSysfsPathLen;

protected:
	SensorBase *mDelegate;
	int mPlace;
	int mBW;
	int mRange;
	int mRef;
	int mScale;
	pthread_mutex_t mLock;

	/* set the bandwidth */
	int setBW(int bw);
	/* set the range */
	int setRange(int range);

	int processEvent(const input_event *event);

	/* TODO: those should be moved out to some generic base class */
	static void remapSensorData(sensors_event_t *event,
			     const struct bst_axis_remap *remap);
	static void remapSensorData(sensors_event_t *event, int place);
	static int getBWFromDelay(uint64_t delay_us);

	int getInputSysfsNodeInt(const char *name, int *val);
	int setInputSysfsNodeInt(const char *name, int val);
	void lock();
	void unlock();

	static const int64_t MIN_DELAY;
	static const int32_t BASE_DELAY_WAKEUP;
	static const struct sensor_accel_bw sTabHWBW[HW_A_BW_MAX];

	static const struct bst_axis_remap
		sTabAxisRemapDft[BST_DFT_AXIS_REMAP_TAB_SZ];
	static const float SCALE_GRAVITY = GRAVITY_EARTH / 4096;
};
#endif
