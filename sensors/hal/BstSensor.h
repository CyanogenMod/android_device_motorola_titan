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
 * @file         BstSensor.h
 * @date         "Wed May 22 14:07:19 2013 +0800"
 * @commit       "49559c7"
 *
 * @brief
 *
 * @detail
 *
 */


#ifndef ANDROID_BST_SENSOR_H
#define ANDROID_BST_SENSOR_H

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "version.h"
#include "sensors.h"
#include "SensorBase.h"
#include "BstSensorPriv.h"
#include "BstSensorInfo.h"
#include "BstSensorType.h"

#define BST_DATA_POLL_TIMEOUT 500000

enum BST_SENSOR_HANDLE {
	BST_SENSOR_HANDLE_START = 0,
	BST_SENSOR_HANDLE_ACCELERATION,         /* 1 */
	BST_SENSOR_HANDLE_MAGNETIC_FIELD,       /* 2 */
	BST_SENSOR_HANDLE_ORIENTATION,          /* 3 */
	BST_SENSOR_HANDLE_GYROSCOPE,            /* 4 */
	BST_SENSOR_HANDLE_LIGHT,                /* 5 */
	BST_SENSOR_HANDLE_PRESSURE,             /* 6 */
	BST_SENSOR_HANDLE_TEMPERATURE,          /* 7 */
	BST_SENSOR_HANDLE_PROXIMITY,            /* 8 */
	BST_SENSOR_HANDLE_GRAVITY,              /* 9 */
	BST_SENSOR_HANDLE_LINEAR_ACCELERATION,  /* 10 */
	BST_SENSOR_HANDLE_ROTATION_VECTOR,      /* 11 */
	BST_SENSOR_HANDLE_ORIENTATION_RAW,      /* 12 */

	BST_SENSOR_HANDLE_GEST_FLIP,            /* 13 */
	BST_SENSOR_HANDLE_SW_SIGNIFICANT_MOTION,
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
	BST_SENSOR_HANDLE_GAME_ROTATION_VECTOR,
	BST_SENSOR_HANDLE_GYROSCOPE_UNCALIBRATED,
	BST_SENSOR_HANDLE_MAGNETIC_FIELD_UNCALIBRATED,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	BST_SENSOR_HANDLE_GEOMAGNETIC_ROTATION_VECTOR,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
	BST_SENSOR_HANDLE_STEP_COUNTER,
	BST_SENSOR_HANDLE_STEP_DETECTOR,
#endif

	BST_SENSOR_HANDLE_END
};

#define BST_SENSOR_NUM_MAX		 (BST_SENSOR_HANDLE_END-1)


class BstSensor : public SensorBase {
	public:
		BstSensor();
		virtual ~BstSensor();

		enum
		{
			BST_SID_START = -1,
			BST_SID_GYRO,
			BST_SID_ACC,
			BST_SID_MAG,
			BST_SID_ORI,
			BST_SID_ROV,
			BST_SID_LA,
			BST_SID_GRV,
			BST_SID_PRS,
			BST_SID_GEST_FLIP,
			BST_SID_SGM,
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
			BST_SID_GROV,
			BST_SID_GRU,
			BST_SID_MU,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
			BST_SID_GEOM_RV,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
			BST_SID_STC,
			BST_SID_STD,
#endif
			NUM_SENSORS
		};

		virtual int setDelay(int32_t id, int64_t ns);
		virtual int enable(int32_t id, int enable);
		virtual int readEvents(sensors_event_t *pdata, int count);
		static int getSensorList(struct sensor_t *list, int len);
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		virtual int batch(int id, int flags, int64_t period_ns, int64_t timeout);
		virtual int flush(int id);
#endif

	protected:
		uint32_t mEnabled;
		uint64_t mDelays[BST_SENSOR_NUM_MAX];
		int mCmdFd;

		void processEvent(int code, int value);

		const static int s_tab_id2handle[BST_SENSOR_NUM_MAX];
		const static int s_tab_handle2id[BST_SENSOR_NUM_MAX];
		static int id2handle(int id);
		static int handle2id(int handle);
	private:
		int initIPC();
		static int initStorage();
};

#endif  // ANDROID_BST_SENSOR_H
