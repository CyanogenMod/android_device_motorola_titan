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

#include "BstSensorExt.h"

#include "TargetPlatform.h"

/*****************************************************************************/
const struct sensor_t BstSensorExt::g_bstext_sensor_list[] =
{
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	{ "BOSCH Significant Motion Sensor",
	  "Bosch Sensortec, GmbH",
	  1, SENSORS_SIGNIFICANT_MOTION_HANDLE,
	  SENSOR_TYPE_SIGNIFICANT_MOTION, 1.0f, 1.0f, 0.0f, -1,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
         0,0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
         SENSOR_STRING_TYPE_SIGNIFICANT_MOTION,
         "",
         -1,
         0,
#endif
	  {}},
#endif

#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	{ "BOSCH Double Tap Sensor",
	  "Bosch Sensortec ,GmbH",
	  1, SENSORS_DOUBLE_TAP_HANDLE,
	  BSTEXT_SENSOR_TYPE_DOUBLE_TAP, 2.0f, 1.0f, 0.0f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	 0,0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
         "bosch.double.tap.sensor",
         "",
         -1,
         0,
#endif
	  {}},
#endif
};


BstSensorExt::BstSensorExt()
    : SensorBase(NULL, BST_SENSOR_EXT_INPUT_DEV),
      mInputReader(4)
{
	mPendingEvent.version = sizeof(sensors_event_t);
	memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));
	memset(input_sysfs_path, 0, sizeof(input_sysfs_path));
	input_sysfs_path_len = 0;
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	mDtapTimes = 0;
#endif

}

BstSensorExt::~BstSensorExt()
{
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	enable(ID_SMT, 0);
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	enable(ID_DTAP, 0);
#endif
}

int BstSensorExt::enableSensor(int en, char * sensor_path, char * enable_mode)
{
	int ret = 0;
	int fd;
	char buf[2];
	strncpy(input_sysfs_path, sensor_path, strlen(sensor_path));
	input_sysfs_path_len = strlen(input_sysfs_path);
	strncpy(&input_sysfs_path[input_sysfs_path_len], enable_mode, strlen(enable_mode));
	fd = open(input_sysfs_path, O_RDWR);
	if (fd >= 0)
	{
		buf[1] = '\0';
		if (en)
			buf[0] = '1';
		else
			buf[0] = '0';
		write(fd, buf, sizeof(buf));
		close(fd);
		ret = 0;
	}
	else
	{
		ret = -EIO;
	}
	return ret;
}

int BstSensorExt::enable(int32_t id, int en)
{
	int ret = 0;

	PDEBUG("BstSensorExt: id=%d, en=%d", id, en);
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	if (ID_SMT == id)
	{
		ret = BstSensorExt::enableSensor(en, (char *)INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR,
								(char *)INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR_ENABLE_NODE);
	}
	else
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	if (ID_DTAP == id)
	{
		ret = BstSensorExt::enableSensor(en, (char *)INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR,
								(char *)INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR_ENABLE_NODE);
	}
	else
#endif
	{
		PERR("sensor id %d not found", id);
	}

	return ret;
}


int BstSensorExt::readEvents(sensors_event_t* data, int count)
{
	if (count < 1)
		return -EINVAL;

	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;

	int numEventReceived = 0;
	input_event const* event;

	while (count && mInputReader.readEvent(&event))
	{
		PDEBUG("%s,event.type is %d,event value is %d,event code is %d",__FUNCTION__,event->type,event->value,event->code);
		if(event->type == EV_REL)
		{
			switch(event->code)
			{	case SINGLE_TAP_INTERRUPT:
					break;
				case LOW_G_INTERRUPT:
					break;
				case HIGH_G_INTERRUPT:
					break;

			#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
				case SIGNIFICANT_MOTION_INTERRUPT:
					// significant motion sensor is one-shot, and disabled in driver.
					mPendingEvent.sensor = ID_SMT;
					mPendingEvent.type = SENSOR_TYPE_SIGNIFICANT_MOTION;
					mPendingEvent.timestamp = timevalToNano(event->time);
					break;
			#else
				case SLOP_INTERRUPT:
					break;
			#endif

			#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
				case DOUBLE_TAP_INTERRUPT:
					mPendingEvent.sensor = ID_DTAP;
					mPendingEvent.type = BSTEXT_SENSOR_TYPE_DOUBLE_TAP;
					mPendingEvent.timestamp = timevalToNano(event->time);
					mPendingEvent.data[0] = mDtapTimes;
					mDtapTimes = (mDtapTimes >= 99) ? 0: mDtapTimes+1;
					break;
			#endif

				default:
					break;

			}
			*data++ = mPendingEvent;
			count--;
			numEventReceived++;
		}

		mInputReader.next();
	}

	return numEventReceived;
}

int BstSensorExt::checkSensorAvailable(int sensor_id)
{
	int fd;
	int ret = 0;
	char sysfs_path[PATH_MAX];
	int sysfs_path_len;
	char *p_path;
	char *p_enable_node;

	switch (sensor_id)
	{
	#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
		case BSTEXT_SID_SMT:
			p_path = (char*)INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR;
			p_enable_node = (char*)INPUT_SYSFS_PATH_SIGNIFICANT_MOTION_SENSOR_ENABLE_NODE;
			break;
	#endif

	#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
		case BSTEXT_SID_DTAP:
			p_path = (char*)INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR;
			p_enable_node = (char*)INPUT_SYSFS_PATH_DOUBLE_TAP_SENSOR_ENABLE_NODE;
			break;
	#endif

		default:
			return 0;
	}

	if (sensor_id < NUM_BSTEXT_SENSORS)
	{
		strcpy(sysfs_path, p_path);
		sysfs_path_len = strlen(sysfs_path);
		strcpy(&sysfs_path[sysfs_path_len], p_enable_node);

		fd = open(sysfs_path, O_RDONLY);
		if (fd >= 0)
		{
			close(fd);
			ret = 0;
		}
		else
			ret = -EIO;
	}

	PINFO("BstSensorExt::checkSensorAvailable, id, ret= %d, %d", sensor_id, ret);
	return ret;
}


int BstSensorExt::getSensorList(struct sensor_t *list, int len)
{
	int             num = 0;
	int j;

	UNUSED_PARAM(len);
	for (j = 0; j < NUM_BSTEXT_SENSORS; j++)
	{
		if ( BstSensorExt::checkSensorAvailable(j) == 0)
		{
			memcpy(list++,
				&BstSensorExt::g_bstext_sensor_list[j],
				sizeof(struct sensor_t));
			num++;
		}
	}

	return num;
}

const struct sensor_t * BstSensorExt::getSensor(int handle)
{
	const struct sensor_t *s = NULL;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(g_bstext_sensor_list); i++) {
		if (g_bstext_sensor_list[i].handle == handle) {
			break;
		}
	}

	if (i < ARRAY_SIZE(g_bstext_sensor_list)) {
		s = (const struct sensor_t *) g_bstext_sensor_list + i;
	}

	return s;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
int BstSensorExt::flush(int id)
{
	PDEBUG("flush id %d", id);

	UNUSED_PARAM(id);
	return -EINVAL;
}

int BstSensorExt::batch(int id, int flags, int64_t period_ns, int64_t timeout)
{
	int ret = 0;

	PDEBUG("batch id %d, flags %d, period_ns %lld, timeout %lld", id, flags, period_ns, timeout);

	UNUSED_PARAM(id);
	UNUSED_PARAM(flags);
	UNUSED_PARAM(period_ns);
	UNUSED_PARAM(timeout);
	//ret = BstSensorExt::setDelay(id, period_ns);
	return ret;
}
#endif

