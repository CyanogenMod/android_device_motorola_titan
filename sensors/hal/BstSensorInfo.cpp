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
 * @file         BstSensorInfo.cpp
 * @date         "Tue May 28 15:47:53 2013 +0800"
 * @commit       "5293218"
 *
 * @brief
 *
 * @detail
 *
 */

#include "sensors.h"

#include "BstSensor.h"
#include "BstSensorInfo.h"

#define VENDOR_NAME_BST_SHORT "BOSCH "
#define VENDOR_NAME_BST "Bosch Sensortec GmbH"
#define SENSOR_STR " Sensor"

#define SENSOR_VER_BST 3060102


const struct sensor_t BstSensorInfo::g_bst_sensor_list[] = {
	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_A SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_ACCELERATION_HANDLE,
		type: SENSOR_TYPE_ACCELEROMETER,
		maxRange: 16.0f * 9.805,
		resolution: 4.0f * 9.805 / (1 << HW_INFO_BITWIDTH_A),
		power: 0.13f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
		fifoReservedEventCount: 0,
		fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
		stringType: SENSOR_STRING_TYPE_ACCELEROMETER,
		requiredPermission: "",
		maxDelay: 200000,
		flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_M SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_MAGNETIC_FIELD_HANDLE,
		type: SENSOR_TYPE_MAGNETIC_FIELD,
		maxRange: 1600.0f,
		resolution: 0.3f,
		power: 0.5f,
		minDelay: 20000,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_MAGNETIC_FIELD,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_G SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_GYROSCOPE_HANDLE,
		type: SENSOR_TYPE_GYROSCOPE,
		maxRange: 2500.0f,
		resolution: 250.0f / (1 << 16),
		power: 5.0f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_GYROSCOPE,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_O SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_ORIENTATION_HANDLE,
		type: SENSOR_TYPE_ORIENTATION,
		maxRange: 360.0f,
		resolution: 1.0f,
		power: 0.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_ORIENTATION,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_VG SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_GRAVITY_HANDLE,
		type: SENSOR_TYPE_GRAVITY,
		maxRange: 9.805f * 2,
		resolution: 4.0f * 9.805 / (1 << HW_INFO_BITWIDTH_A),
		power: 5.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_GRAVITY,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_VLA SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_LINEAR_ACCEL_HANDLE,
		type: SENSOR_TYPE_LINEAR_ACCELERATION,
		maxRange: 9.805f * 2,
		resolution: 4.0f * 9.805 / (1 << HW_INFO_BITWIDTH_A),
		power: 5.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_LINEAR_ACCELERATION,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_VRV SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_ROTATION_VECTOR_HANDLE,
		type: SENSOR_TYPE_ROTATION_VECTOR,
		maxRange: 1.0f,
		resolution: 1.0f / (1 << 24),
		power: 5.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_ROTATION_VECTOR,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},


//	{
//		name: VENDOR_NAME_BST_SHORT
//			      BST_SENSOR_NAME_GEST_FLIP SENSOR_STR,
//		vendor: VENDOR_NAME_BST,
//		version: SENSOR_VER_BST,
//		handle: SENSORS_GEST_FLIP_HANDLE,
//		type: BST_SENSOR_TYPE_GEST_FLIP,
//		maxRange: 1.0f,
//		resolution: 1.0f,
//		power: 0.13f,
//		minDelay: 0,
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
//                fifoReservedEventCount: 0,
//                fifoMaxEventCount: 0,
//#endif
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
//                stringType: "bosch.gest.flip.sensor",
//                requiredPermission: "",
//                maxDelay: 200000,
//                flags: 0,
//#endif
//		reserved: {}
//	},
//
//	{
//		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_SGM SENSOR_STR,
//		vendor: VENDOR_NAME_BST,
//		version: SENSOR_VER_BST,
//		handle: SENSORS_SW_SIGNIFICANT_MOTION_HANDLE,
//		type: BSTEXT_SENSOR_TYPE_SW_SGM,
//		maxRange: 1.0f,
//		resolution: 1.0f,
//		power: 0.13f,
//#ifdef __FASTEST_MODE_100HZ__
//		minDelay: 10000,
//#else
//		minDelay: 5000,
//#endif
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
//                fifoReservedEventCount: 0,
//                fifoMaxEventCount: 0,
//#endif
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
//                stringType: "bosch.software.signficant.motion.sensor",
//                requiredPermission: "",
//                maxDelay: 200000,
//                flags: 0,
//#endif
//		reserved: {}
//	},

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_GRV SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_GAME_ROTATION_VECTOR_HANDLE,
		type: SENSOR_TYPE_GAME_ROTATION_VECTOR,
		maxRange: 1.0f,
		resolution: 1.0f / (1 << 24),
		power: 5.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_GYU SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_GYROSCOPE_UNCALIBRATED_HANDLE,
		type: SENSOR_TYPE_GYROSCOPE_UNCALIBRATED,
		maxRange: 2500.0f,
		resolution: 250.0f / (1 << 16),
		power: 5.0f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},

	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_MU SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_MAGNETIC_UNCALIBRATED_HANDLE,
		type: SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
		maxRange: 1600.0f,
		resolution: 0.3f,
		power: 0.5f,
		minDelay: 20000,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
                fifoReservedEventCount: 0,
                fifoMaxEventCount: 0,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	{
		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_GEOM_RY SENSOR_STR,
		vendor: VENDOR_NAME_BST,
		version: SENSOR_VER_BST,
		handle: SENSORS_GEOMAGNETIC_ROTATION_VECTOR_HANDLE,
		type: SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR,
		maxRange: 1.0f,
		resolution: 1.0f / (1 << 24),
		power: 5.63f,
#ifdef __FASTEST_MODE_100HZ__
		minDelay: 10000,
#else
		minDelay: 5000,
#endif
		fifoReservedEventCount: 0,
		fifoMaxEventCount: 0,
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
                stringType: SENSOR_STRING_TYPE_GEOMAGNETIC_ROTATION_VECTOR,
                requiredPermission: "",
                maxDelay: 200000,
                flags: 0,
#endif
		reserved: {}
	},
#endif

//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
//	{
//		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_STC SENSOR_STR,
//		vendor: VENDOR_NAME_BST,
//		version: SENSOR_VER_BST,
//		handle: SENSORS_STEP_COUNTER_HANDLE,
//		type: SENSOR_TYPE_STEP_COUNTER,
//		maxRange: 10000.0f,
//		resolution: 1.0f,
//		power: 5.63f,
//		minDelay: 0,
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
//                fifoReservedEventCount: 0,
//                fifoMaxEventCount: 0,
//#endif
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
//                stringType: SENSOR_STRING_TYPE_STEP_COUNTER,
//                requiredPermission: "",
//                maxDelay: 1000000,
//                flags: 0,
//#endif
//		reserved: {}
//	},
//	{
//		name: VENDOR_NAME_BST_SHORT BST_SENSOR_NAME_STD SENSOR_STR,
//		vendor: VENDOR_NAME_BST,
//		version: SENSOR_VER_BST,
//		handle: SENSORS_STEP_DETECTOR_HANDLE,
//		type: SENSOR_TYPE_STEP_DETECTOR,
//		maxRange: 1.0f,
//		resolution: 1.0f,
//		power: 5.63f,
//		minDelay: 0,
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
//                fifoReservedEventCount: 0,
//                fifoMaxEventCount: 0,
//#endif
//#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_3__
//                stringType: SENSOR_STRING_TYPE_STEP_DETECTOR,
//                requiredPermission: "",
//                maxDelay: 1000000,
//                flags: 0,
//#endif
//		reserved: {}
//	},
//
//#endif

};


const struct sensor_t * BstSensorInfo::getSensor(int handle)
{
	const struct sensor_t *s = NULL;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(g_bst_sensor_list); i++) {
		if (g_bst_sensor_list[i].handle == handle) {
			break;
		}
	}

	if (i < ARRAY_SIZE(g_bst_sensor_list)) {
		s = (const struct sensor_t *) g_bst_sensor_list + i;
	}

	return s;
}
