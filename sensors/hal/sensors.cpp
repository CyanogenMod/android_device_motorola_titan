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

//#define LOG_TAG "Sensors"

#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>

#include <unistd.h>

#include <linux/input.h>


#include "sensors.h"

#include "BstSensor.h"
#include "LightSensor.h"
#include "ProximitySensor.h"
#ifdef __PRESSURE_SENSOR_SUPPORT__
#include "BstSensorPressure.h"
#endif
#ifdef __BST_EXTEND_SENSOR_SUPPORT__
#include "BstSensorExt.h"
#endif

#ifdef __HYBRID_HAL__
#include "BstSensorAccel.h"
#endif

#include "TargetPlatform.h"

/*****************************************************************************/
enum {
	light,
	proximity,
#ifdef __PRESSURE_SENSOR_SUPPORT__
	pressure,
#endif
#ifdef __BST_EXTEND_SENSOR_SUPPORT__
	bst_ext,
#endif
#ifdef __HYBRID_HAL__
	bst_acc,
#endif
	bst,
	numSensorDrivers,
	numFds,
};

#define DELAY_OUT_TIME 0x7FFFFFFF

/*****************************************************************************/

/* The SENSORS Module */
#ifdef __BST_EXTEND_SENSOR_SUPPORT__
#ifdef __HYBRID_HAL__
#define LOCAL_SENSORS (numSensorDrivers- 3)
#else
#define LOCAL_SENSORS (numSensorDrivers- 2)
#endif
#define SENSOR_NUMBERS (LOCAL_SENSORS + BstSensorExt::NUM_BSTEXT_SENSORS + BstSensor::NUM_SENSORS)
#else
#ifdef __HYBRID_HAL__
#define LOCAL_SENSORS (numSensorDrivers- 2)
#else
#define LOCAL_SENSORS (numSensorDrivers- 1)
#endif
#define SENSOR_NUMBERS (LOCAL_SENSORS + BstSensor::NUM_SENSORS)
#endif

#if defined(L8720_COMMON) || defined(L8150_COMMON)

static  struct sensor_t sSensorList[SENSOR_NUMBERS] = {
	{
		"liteon-light",
		"liteon",
		1, SENSORS_LIGHT_HANDLE,
		SENSOR_TYPE_LIGHT,  60000, 0.0125f, 0.2f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		0, 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
		SENSOR_STRING_TYPE_LIGHT,"",-1,0,
#endif
		{}
	},

	{
		"liteon-proximity",
		"liteon",
		1, SENSORS_PROXIMITY_HANDLE,
		SENSOR_TYPE_PROXIMITY, 5.0f, 5.0f, 3.0f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		0, 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                SENSOR_STRING_TYPE_PROXIMITY,"",-1,SENSOR_FLAG_WAKE_UP,
#endif
		{}
	},


};

#else
static  struct sensor_t sSensorList[SENSOR_NUMBERS] = {
	{
		"STK Light sensor",
		"Sensortek",
		1, SENSORS_LIGHT_HANDLE,
		SENSOR_TYPE_LIGHT,  powf(10, (280.0f / 47.0f)) * 4, 1.0f, 0.75f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		0, 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
		SENSOR_STRING_TYPE_LIGHT,"",-1,0,
#endif
		{}
	},

	{
		"STK Proximity sensor",
		"Sensortek",
		1, SENSORS_PROXIMITY_HANDLE,
		SENSOR_TYPE_PROXIMITY, 5.0f, 5.0f, 0.75f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		0, 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                SENSOR_STRING_TYPE_PROXIMITY,"",-1,SENSOR_FLAG_WAKE_UP,
#endif
		{}
	},

#ifdef __PRESSURE_SENSOR_SUPPORT__
	{
		"BOSCH Pressure Sensor",
		"Bosch Sensortec,GmbH",
		1, SENSORS_PRESSURE_HANDLE,
		SENSOR_TYPE_PRESSURE, 1100.0f, 0.18f, 4.0f, 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		0, 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                SENSOR_STRING_TYPE_PRESSURE,"",-1,0,
#endif
		{}
	},
#endif
};
#endif
static int numSensors = LOCAL_SENSORS;

static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device);


static int sensors__get_sensors_list(struct sensors_module_t* module,
                                     struct sensor_t const** list)
{
        UNUSED_PARAM(module);
        *list = sSensorList;
        return numSensors;
}

static struct hw_module_methods_t sensors_module_methods = {
        open: open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
        common: {
                tag: HARDWARE_MODULE_TAG,
                version_major: 1,
                version_minor: 0,
                id: SENSORS_HARDWARE_MODULE_ID,
                name: "Bosch Sensor module",
                author: "Bosch Sensortec,GmbH",
                methods: &sensors_module_methods,
                dso: NULL,
                reserved: {0}
        },
        get_sensors_list: sensors__get_sensors_list,
};

struct sensors_poll_context_t {
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
	sensors_poll_device_1_t device; // must be first
#else
	struct sensors_poll_device_t device; // must be first
#endif
	sensors_poll_context_t();
	~sensors_poll_context_t();
	int activate(int handle, int enabled);
	int setDelay(int handle, int64_t ns);
	int pollEvents(sensors_event_t* data, int count);
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	int batch(int handle, int flags, int64_t period_ns, int64_t timeout);
	int flush(int handle);
#endif

private:

    static const size_t wake = numFds - 1;
    static const char WAKE_MESSAGE = 'W';
    struct pollfd mPollFds[numFds];
    int mWritePipeFd;
    SensorBase* mSensors[numSensorDrivers];

    int handleToDriver(int handle) const {
        switch (handle) {
            case ID_A:
#ifdef __HYBRID_HAL__
                return bst_acc;
#endif
            case ID_RV:
            case ID_LA:
            case ID_GR:
            case ID_GY:
            case ID_M:
            case ID_O:
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
            case ID_GRV:
            case ID_GYU:
            case ID_MU:
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
			case ID_GEOM_RV:
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
			case ID_STC:
			case ID_STD:
#endif

#ifndef __PRESSURE_SENSOR_SUPPORT__
            case ID_PR:
#endif
            case ID_GEST_FLIP:
            case ID_SGM:
                return bst;
            case ID_P:
                return proximity;
            case ID_L:
                return light;
#ifdef __PRESSURE_SENSOR_SUPPORT__
            case ID_PR:
                return pressure;
#endif
#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
            case ID_SMT:
                return bst_ext;
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
            case ID_DTAP:
                return bst_ext;
#endif

        }
        return -EINVAL;
    }
};

/*****************************************************************************/

sensors_poll_context_t::sensors_poll_context_t()
{
	unsigned char bst_sensor_num = 0;

    mSensors[light] = new LightSensor();
    mPollFds[light].fd = mSensors[light]->getFd();
    mPollFds[light].events = POLLIN;
    mPollFds[light].revents = 0;

    mSensors[proximity] = new ProximitySensor();
    mPollFds[proximity].fd = mSensors[proximity]->getFd();
    mPollFds[proximity].events = POLLIN;
    mPollFds[proximity].revents = 0;

#ifdef __PRESSURE_SENSOR_SUPPORT__
    mSensors[pressure] = new PressureSensor("bmp");
    mPollFds[pressure].fd = mSensors[pressure]->getFd();
    mPollFds[pressure].events = POLLIN;
    mPollFds[pressure].revents = 0;
#endif

#ifdef __BST_EXTEND_SENSOR_SUPPORT__
	mSensors[bst_ext] = new BstSensorExt();
	mPollFds[bst_ext].fd = mSensors[bst_ext]->getFd();
	mPollFds[bst_ext].events = POLLIN;
	mPollFds[bst_ext].revents = 0;
#endif

    mSensors[bst] = new BstSensor();
    mPollFds[bst].fd = mSensors[bst]->getFd();
    mPollFds[bst].events = POLLIN;
    mPollFds[bst].revents = 0;

#ifdef __HYBRID_HAL__
	mSensors[bst_acc] = new BstSensorAccel(DEV_NAME_A,NULL);
	mPollFds[bst_acc].fd = mSensors[bst_acc]->getFd();
	mPollFds[bst_acc].events = POLLIN;
	mPollFds[bst_acc].revents = 0;
#endif


#ifdef __BST_EXTEND_SENSOR_SUPPORT__
	bst_sensor_num = ((BstSensorExt *)mSensors[bst_ext] )->getSensorList(sSensorList + LOCAL_SENSORS,
				sizeof(sSensorList[0]) * BstSensorExt::NUM_BSTEXT_SENSORS);
#endif

	numSensors =
			LOCAL_SENSORS + bst_sensor_num +
			((BstSensor *)mSensors[bst] )->getSensorList(sSensorList + LOCAL_SENSORS + bst_sensor_num,
				sizeof(sSensorList[0]) * BstSensor::NUM_SENSORS);

    int wakeFds[2];
    int result = pipe(wakeFds);
    PERR_IF(result<0, "error creating wake pipe (%s)", strerror(errno));
    fcntl(wakeFds[0], F_SETFL, O_NONBLOCK);
    fcntl(wakeFds[1], F_SETFL, O_NONBLOCK);
    mWritePipeFd = wakeFds[1];

    mPollFds[wake].fd = wakeFds[0];
    mPollFds[wake].events = POLLIN;
    mPollFds[wake].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t() {
    for (int i=0 ; i<numSensorDrivers ; i++) {
        delete mSensors[i];
    }
    close(mPollFds[wake].fd);
    close(mWritePipeFd);
}

int sensors_poll_context_t::activate(int handle, int enabled) {
    
    int index = handleToDriver(handle);
	PDEBUG("activate, handle: %d, enabled: %d",
			handle, enabled);

    if (index < 0) return index;
    int err =  mSensors[index]->enable(handle, enabled);
    if (enabled && !err) {
        const char wakeMessage(WAKE_MESSAGE);
        int result = write(mWritePipeFd, &wakeMessage, 1);
        PERR_IF(result<0, "error sending wake message (%s)", strerror(errno));
    }
    return err;
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns) {

    int index = handleToDriver(handle);

	PDEBUG("setDelay, handle: %d delay: %jd",
			handle, ns);

    if (index < 0) return index;
    return mSensors[index]->setDelay(handle, ns);
}

int sensors_poll_context_t::pollEvents(sensors_event_t* data, int count)
{
    int nbEvents = 0;
    int n = 0;

    do {
        // see if we have some leftover from the last poll()
        for (int i=0 ; count && i<numSensorDrivers ; i++) {
            SensorBase* const sensor(mSensors[i]);
            if ((mPollFds[i].revents & POLLIN) || (sensor->hasPendingEvents())) {
                int nb = sensor->readEvents(data, count);
                if (nb < count) {
                    // no more data for this sensor
                    mPollFds[i].revents = 0;
                }
                count -= nb;
                nbEvents += nb;
                data += nb;
            }
        }

        if (count) {
            // we still have some room, so try to see if we can get
            // some events immediately or just wait if we don't have
            // anything to return
            do {
                n = poll(mPollFds, numFds, nbEvents ? 0 : -1);
            } while (n < 0 && errno == EINTR);
            if (n<0) {
                PERR("poll() failed (%s)", strerror(errno));
                return -errno;
            }
            if (mPollFds[wake].revents & POLLIN) {
                char msg;
                int result = read(mPollFds[wake].fd, &msg, 1);
                PERR_IF(result<0, "error reading from wake pipe (%s)", strerror(errno));
                PERR_IF(msg != WAKE_MESSAGE, "unknown message on wake queue (0x%02x)", int(msg));
                mPollFds[wake].revents = 0;
            }
        }
        // if we have events and space, go read them
    } while (n && count);

    return nbEvents;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
int sensors_poll_context_t::batch(int handle, int flags, int64_t period_ns, int64_t timeout)
{
	int index = handleToDriver(handle);
	PDEBUG("batch, handle: %d, flags: %d, period_ns: %lld, timeout: %lld, index: %d",
		handle, flags, period_ns, timeout, index);

	if (index < 0) return index;

	int err =  mSensors[index]->batch(handle, flags, period_ns, timeout);

	return err;
}


int sensors_poll_context_t::flush(int handle)
{
	int index = handleToDriver(handle);

	PDEBUG("flush, handle: %d, index: %d",
		handle, index);
	if (index < 0)
		return index;
	return mSensors[index]->flush(handle);
}
#endif

/*****************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
        int handle, int enabled) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
        int handle, int64_t ns) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->setDelay(handle, ns);
}

static int poll__poll(struct sensors_poll_device_t *dev,
        sensors_event_t* data, int count) {
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->pollEvents(data, count);
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
static int poll__batch(struct sensors_poll_device_1* dev,
            int handle, int flags, int64_t period_ns, int64_t timeout)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->batch(handle, flags, period_ns, timeout);
}

static int poll__flush(struct sensors_poll_device_1* dev, int handle)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    return ctx->flush(handle);
}
#endif
/*****************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char* id,
			struct hw_device_t** device)
{
        int status = -EINVAL;
        UNUSED_PARAM(id);
        sensors_poll_context_t *dev = new sensors_poll_context_t();

	#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
		memset(&dev->device, 0, sizeof(sensors_poll_device_1_t));
	#else
		memset(&dev->device, 0, sizeof(sensors_poll_device_t));
	#endif

		dev->device.common.tag = HARDWARE_DEVICE_TAG;
	#if __HAL_VER__ == __SENSORS_DEVICE_API_VERSION_1_3__
		dev->device.common.version	= SENSORS_DEVICE_API_VERSION_1_3;
	#elif __HAL_VER__ == __SENSORS_DEVICE_API_VERSION_1_1__
		dev->device.common.version	= SENSORS_DEVICE_API_VERSION_1_1;
	#elif __HAL_VER__ == __SENSORS_DEVICE_API_VERSION_1_0__
		dev->device.common.version	= SENSORS_DEVICE_API_VERSION_1_0;
	#elif __HAL_VER__ == __SENSORS_DEVICE_API_VERSION_0__
		dev->device.common.version	= SENSORS_DEVICE_API_VERSION_0_1;
	#else
		#warning no specify the sensor device api version, will set 0 as default
		dev->device.common.version = 0;
	#endif
	dev->device.common.module = const_cast<hw_module_t*>(module);
	dev->device.common.close = poll__close;
	dev->device.activate = poll__activate;
	dev->device.setDelay = poll__setDelay;
	dev->device.poll = poll__poll;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	dev->device.batch = poll__batch;
	dev->device.flush = poll__flush;
#endif
	*device = &dev->device.common;
	status = 0;

        return status;
}

