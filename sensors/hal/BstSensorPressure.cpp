/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2014 Bosch Sensortec GmbH All Rights Reserved
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
 *------------------------------------------------------------------------------
 * Disclaimer
 *
 * Common: Bosch Sensortec products are developed for the consumer goods
 * industry. They may only be used within the parameters of the respective valid
 * product data sheet.  Bosch Sensortec products are provided with the express
 * understanding that there is no warranty of fitness for a particular purpose.
 * They are not fit for use in life-sustaining, safety or security sensitive
 * systems or any system or device that may lead to bodily harm or property
 * damage if the system or device malfunctions. In addition, Bosch Sensortec
 * products are not fit for use in products which interact with motor vehicle
 * systems.  The resale and/or use of products are at the purchaser's own risk
 * and his own responsibility. The examination of fitness for the intended use
 * is the sole responsibility of the Purchaser.
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims,
 * including any claims for incidental, or consequential damages, arising from
 * any product use not covered by the parameters of the respective valid product
 * data sheet or not approved by Bosch Sensortec and reimburse Bosch Sensortec
 * for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased products,
 * particularly with regard to product safety and inform Bosch Sensortec without
 * delay of all security relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may vary
 * from the valid technical specifications of the product series. They are
 * therefore not intended or fit for resale to third parties or for use in end
 * products. Their sole purpose is internal client testing. The testing of an
 * engineering sample may in no way replace the testing of a product series.
 * Bosch Sensortec assumes no liability for the use of engineering samples. By
 * accepting the engineering samples, the Purchaser agrees to indemnify Bosch
 * Sensortec from all claims arising from the use of engineering samples.
 *
 * Special: This software module (hereinafter called "Software") and any
 * information on application-sheets (hereinafter called "Information") is
 * provided free of charge for the sole purpose to support your application
 * work. The Software and Information is subject to the following terms and
 * conditions:
 *
 * The Software is specifically designed for the exclusive use for Bosch
 * Sensortec products by personnel who have special experience and training. Do
 * not use this Software if you do not have the proper experience or training.
 *
 * This Software package is provided `` as is `` and without any expressed or
 * implied warranties, including without limitation, the implied warranties of
 * merchantability and fitness for a particular purpose.
 *
 * Bosch Sensortec and their representatives and agents deny any liability for
 * the functional impairment of this Software in terms of fitness, performance
 * and safety. Bosch Sensortec and their representatives and agents shall not be
 * liable for any direct or indirect damages or injury, except as otherwise
 * stipulated in mandatory applicable law.
 *
 * The Information provided is believed to be accurate and reliable. Bosch
 * Sensortec assumes no responsibility for the consequences of use of such
 * Information nor for any infringement of patents or other rights of third
 * parties which may result from its use.
 *
 * @file         BstSensorPressure.cpp
 * @date         "Fri Jul 5 13:47:33 2013 +0800"
 * @commit       "39326f1"
 *
 * @brief
 *
 * @detail
 *
 */


#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <math.h>
#include <cutils/log.h>

#include "BstSensorPressure.h"

#define DEBUG_SENSOR    0

/* android delay mode, unit:ms */
#define SENSOR_DELAY_NORMAL 200
#define SENSOR_DELAY_UI 60
#define SENSOR_DELAY_GAME 20
#define SENSOR_DELAY_FASTEST 0

/* millisecond convert to nanosecond */
#define ms2ns(ms) (ms*1000*1000LL)

/*********************[BMP280]******************/
/* workmode */
#define BMP280_ULTRALOWPOWER_MODE            0x00
#define BMP280_LOWPOWER_MODE                 0x01
#define BMP280_STANDARDRESOLUTION_MODE       0x02
#define BMP280_HIGHRESOLUTION_MODE           0x03
#define BMP280_ULTRAHIGHRESOLUTION_MODE      0x04

/* standby duration */
#define BMP280_STANDBYTIME_1_MS              1
#define BMP280_STANDBYTIME_63_MS             63
#define BMP280_STANDBYTIME_125_MS            125
#define BMP280_STANDBYTIME_250_MS            250
#define BMP280_STANDBYTIME_500_MS            500
#define BMP280_STANDBYTIME_1000_MS           1000
#define BMP280_STANDBYTIME_2000_MS           2000
#define BMP280_STANDBYTIME_4000_MS           4000

/* filter coefficient */
#define BMP280_FILTERCOEFF_OFF               0x00
#define BMP280_FILTERCOEFF_2                 0x01
#define BMP280_FILTERCOEFF_4                 0x02
#define BMP280_FILTERCOEFF_8                 0x03
#define BMP280_FILTERCOEFF_16                0x04

/* sysfs node */
#define PRESSURE_SYSFS_PATH           "/sys/class/input"

#define BMP280_SYSFS_ATTR_ENABLE     "enable"
#define BMP280_SYSFS_ATTR_DELAY      "delay"
#define BMP280_SYSFS_ATTR_WORKMODE   "workmode"
#define BMP280_SYSFS_ATTR_FILTER     "filter"
#define BMP280_SYSFS_ATTR_STANDBYDUR "standbydur"

/*********************[BMP18X]******************/
#define BMP18X_SYSFS_ATTR_ENABLE     "enable"
#define BMP18X_SYSFS_ATTR_DELAY      "delay"

static const char *sensors_list[] = {
		"bmp280",
		"bmp18x"
};

PressureSensor::PressureSensor(const char *InputName)
	:SensorBase(NULL, NULL),
	mEnabled(0),
	mPendingMask(0),
	mDelay(0),
	mInputReader(32)
{
	memset(&mPendingEvent, 0, sizeof(mPendingEvent));
	memset(mClassPath, '\0', sizeof(mClassPath));

	mPendingEvent.version = sizeof(sensors_event_t);
	mPendingEvent.sensor  = ID_PR;
	mPendingEvent.type    = SENSOR_TYPE_PRESSURE;

	if(sensor_get_class_path(mClassPath, InputName))//support bmp280 & bmp18*
	{
		PERR("Can`t find the %s sensor!", InputName);
	}

	if (mName)
	{
		data_name = mName;
		data_fd = openInput(mName);
	}
	//strcpy(mName, InputName);
	//LOGD("Expected pressure sensor %s", mName);
}

PressureSensor::~PressureSensor()
{
}

int PressureSensor::enable(int32_t handle, int en)
{
	int err = 0;
	uint32_t newState  = en;

	UNUSED_PARAM(handle);
	if (mEnabled != newState) {
		err = write_enable(newState);
		PINFO("Change pressure sensor state \"%d -> %d\"", mEnabled, newState);
		PERR_IF(err, "Could not change sensor state \"%d -> %d\" (%s)",
				mEnabled, newState, strerror(-err));
		if (!err) {
			mEnabled = newState;
			update_delay();
		}
	}
	return err;
}

int PressureSensor::setDelay(int32_t handle, int64_t ns)
{
	if (ns < 0)
		return -EINVAL;
	UNUSED_PARAM(handle);
	mDelay = ns;
	return update_delay();
}

int PressureSensor::update_delay()
{
	if (mEnabled)
		return set_delay(mDelay);
	else
		return 0;
}

int PressureSensor::set_delay(int64_t ns) {
	/*
	*update settings depend on delay time
	*/
	write_mode(ns);
	return write_delay(ns);
}

int PressureSensor::write_enable(int newState) {
	int err = 0;

	if (strcmp(mName, "bmp280") == 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", !!newState);

		err = set_sysfs_input_attr(mClassPath, BMP280_SYSFS_ATTR_ENABLE, buffer, bytes);
		return err;
	} else if (strcmp(mName, "bmp18x") == 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", !!newState);

		err = set_sysfs_input_attr(mClassPath, BMP18X_SYSFS_ATTR_ENABLE, buffer, bytes);
		return err;
	}
	return 0;
}

int PressureSensor::write_delay(int64_t ns) {
	int err = 0;

	if (0 > ns)
		ns = 0;

	if (strcmp(mName, "bmp280") == 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", (int32_t)(ns / 1000000LL));

		err = set_sysfs_input_attr(mClassPath, BMP280_SYSFS_ATTR_DELAY, buffer, bytes);
		return err;
	} else if(strcmp(mName, "bmp18x") == 0) {
		char buffer[20];
		int bytes = sprintf(buffer, "%d\n", (int32_t)(ns / 1000000LL));

		err = set_sysfs_input_attr(mClassPath, BMP18X_SYSFS_ATTR_DELAY, buffer, bytes);
		return err;
	}
	return 0;
}

int PressureSensor::write_mode(int64_t ns)
{
	if (0 > ns)
		ns = 0;

	if (strcmp(mName, "bmp280") == 0) {

		char workmode[20], standbydur[20], filtercoeff[20];
		int bytes_mode, bytes_dur, bytes_filter;

		if (mDelay >= ms2ns(SENSOR_DELAY_NORMAL)){
			PDEBUG("normal delay mode");
			bytes_mode = sprintf(workmode, "%d\n", BMP280_STANDARDRESOLUTION_MODE);
			bytes_dur = sprintf(standbydur, "%d\n", BMP280_STANDBYTIME_125_MS);
			bytes_filter = sprintf(filtercoeff, "%d\n", BMP280_FILTERCOEFF_4);
		} else if (mDelay >= ms2ns(SENSOR_DELAY_UI)) {
			PDEBUG("ui delay mode");
			bytes_mode = sprintf(workmode, "%d\n", BMP280_ULTRAHIGHRESOLUTION_MODE);
			bytes_dur = sprintf(standbydur, "%d\n", BMP280_STANDBYTIME_1_MS);
			bytes_filter = sprintf(filtercoeff, "%d\n", BMP280_FILTERCOEFF_16);
		} else if (mDelay >= ms2ns(SENSOR_DELAY_GAME)) {
			PDEBUG("game delay mode");
			bytes_mode = sprintf(workmode, "%d\n", BMP280_STANDARDRESOLUTION_MODE);
			bytes_dur = sprintf(standbydur, "%d\n", BMP280_STANDBYTIME_1_MS);
			bytes_filter = sprintf(filtercoeff, "%d\n", BMP280_FILTERCOEFF_16);
		} else {
			PDEBUG("fastest delay mode");
			bytes_mode = sprintf(workmode, "%d\n", BMP280_LOWPOWER_MODE);
			bytes_dur = sprintf(standbydur, "%d\n", BMP280_STANDBYTIME_1_MS);
			bytes_filter = sprintf(filtercoeff, "%d\n", BMP280_FILTERCOEFF_OFF);
		}

		set_sysfs_input_attr(mClassPath, BMP280_SYSFS_ATTR_WORKMODE, workmode, bytes_mode);
		set_sysfs_input_attr(mClassPath, BMP280_SYSFS_ATTR_STANDBYDUR, standbydur, bytes_dur);
		set_sysfs_input_attr(mClassPath, BMP280_SYSFS_ATTR_FILTER, filtercoeff, bytes_filter);
	}
	return 0;
}

int PressureSensor::readEvents(sensors_event_t* data, int count)
{
	if (count < 1)
		return -EINVAL;

	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;

	int numEventReceived = 0;
	input_event const* event;

	while (count && mInputReader.readEvent(&event)) {
		int type = event->type;
		if (type == EV_MSC) {
			processEvent(event->code, event->value);
			mInputReader.next();
		} else if (type == EV_SYN) {
			int64_t time = timevalToNano(event->time);
#if DEBUG_SENSOR
			PDEBUG("Pressure sensor event time: %f",
				(int64_t)((int64_t)event->time.tv_sec*1000000000
				+ (int64_t)event->time.tv_usec*1000) / 1000000000.0);
#endif
			if (mPendingMask) {
				mPendingMask = 0;
				mPendingEvent.timestamp = time;
				if (mEnabled) {
					*data++ = mPendingEvent;
					count--;
					numEventReceived++;
				}
			}
			if (!mPendingMask) {
				mInputReader.next();
			}
		} else {
			PERR("Pressure sensor: unknown event (type=%d, code=%d)",
					type, event->code);
			mInputReader.next();
		}
	}

	return numEventReceived;
}

void PressureSensor::processEvent(int code, int value)
{
	switch (code) {
	case MSC_RAW:
		mPendingMask = 1;
		mPendingEvent.pressure = value/100.0;
	break;
	default:
		/* TODO: implement if needed. */
	break;
	}

#if DEBUG_SENSOR
	PDEBUG("The input code is %d value is %d" ,code, value);
	PDEBUG("The pressure data is x = %ld",mPendingEvent.pressure);
#endif
}

int PressureSensor::sensor_get_class_path(char *class_path, const char* InputName)
{
	char dirname[256] = PRESSURE_SYSFS_PATH;
	char buf[256];
	char path[256];
	int res;
	DIR *dir;
	struct dirent de;
	struct dirent *result;
	int ret = 0;
	int fd = -1;
	int found = 0;
	unsigned int i;

	UNUSED_PARAM(InputName);
	dir = opendir(dirname);
	if (dir == NULL)
		return -1;

	while(ret == 0) {
		ret = readdir_r(dir, &de, &result);
		if ((0 != ret) || (NULL == result))
		{
			break;
		}
		if (strncmp(de.d_name, "input", strlen("input")) != 0) {
			continue;
		}

		snprintf(path, sizeof(path), "%s/%s", dirname, de.d_name);
		snprintf(buf, sizeof(buf), "%s/name", path);

		fd = open(buf, O_RDONLY);
		if (fd < 0) {
			continue;
		}
		if ((res = read(fd, buf, sizeof(buf))) < 0) {
			close(fd);
			continue;
		}
		buf[res - 1] = '\0';
#if 0
		if (strcmp(buf, InputName) == 0) {
			found = 1;
			close(fd);
			break;
		}
#endif
		for (i = 0; i < ARRAY_SIZE(sensors_list); i++){
			if (strcmp(buf, sensors_list[i]) == 0) {
				strcpy(mName, sensors_list[i]);
				PDEBUG("Expected pressure sensor %s", mName);
				found = 1;
				close(fd);
				break;
			}
		}
		if (found)
			break;
		close(fd);
		fd = -1;
	}
	closedir(dir);

	if (found) {
		strcpy(class_path, path);
		return 0;
	}else {
		*class_path = '\0';
		return -1;
	}
}

int PressureSensor::set_sysfs_input_attr(char *class_path, const char *attr,
			char *value, int len) {
	char path[256];
	int fd;

	if (class_path == NULL || *class_path == '\0'
		|| attr == NULL || value == NULL || len < 1) {
		return -EINVAL;
	}

	snprintf(path, sizeof(path), "%s/%s", class_path, attr);

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		PERR("Fail to open path :%s", path);
		return -errno;
	}

	if (write(fd, value, len) < 0) {
		PERR("Cannot write path :%s", path);
		close(fd);
		return -errno;
	}
	close(fd);

	return 0;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
int PressureSensor::flush(int id)
{
	PDEBUG("flush id %d", id);

	UNUSED_PARAM(id);
	return -EINVAL;
}

int PressureSensor::batch(int id, int flags, int64_t period_ns, int64_t timeout)
{
	int ret = 0;

	PDEBUG("batch id %d, flags %d, period_ns %lld, timeout %lld", id, flags, period_ns, timeout);

	UNUSED_PARAM(id);
	UNUSED_PARAM(flags);
	UNUSED_PARAM(period_ns);
	UNUSED_PARAM(timeout);
	ret = PressureSensor::setDelay(id, period_ns);
	return ret;
}
#endif

