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
 * @file         algo_if.h
 * @date         "Mon May 20 16:15:51 2013 +0800"
 * @commit       "06a64a2"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __ALGO_IF_H
#define __ALGO_IF_H

#include "BsxFusionLibrary.h"
#include "BsxLibraryCalibConstants.h"
#include "BsxLibraryConstants.h"
#include "BsxLibraryDataTypes.h"
#include "BsxLibraryErrorConstants.h"
#include "cust.h"


#define ALGO_CALIB_PROFILE_VER '3'

struct calib_profile {
	ts_calibprofile profile;
	int status;
};


struct algo_work_mode {
	const char *name;
	ts_workingModes mode;

	/* a bitmap of sensor type supported */
	const uint32_t cap;
	/* mask of type supported, a masked
	 * type will not be supported
	 * */
	uint32_t mask;
	uint32_t available:1;

	const uint32_t weight:5;

	const int min_dr_a:8;
	const int max_dr_a:8;
	const int min_dr_m:8;
	const int max_dr_m:8;
	const int min_dr_g:8;
	const int max_dr_g:8;

	void (*entry)();
	void (*exit)();
};

#define ALGO_DR_INVALID   (BSX_DATARATE_200HZ + 10)

#define ALGO_DR_1HZ BSX_DATARATE_1HZ
#define ALGO_DR_5HZ BSX_DATARATE_5HZ
#define ALGO_DR_10HZ BSX_DATARATE_10HZ
#define ALGO_DR_20HZ BSX_DATARATE_20HZ
#define ALGO_DR_25HZ BSX_DATARATE_25HZ
#define ALGO_DR_40HZ BSX_DATARATE_40HZ
#define ALGO_DR_50HZ BSX_DATARATE_50HZ
#define ALGO_DR_100HZ BSX_DATARATE_100HZ
#define ALGO_DR_125HZ BSX_DATARATE_125HZ
#define ALGO_DR_200HZ BSX_DATARATE_200HZ


#define ALGO_A_OPMODE_UNKNOWN (-1)
#define ALGO_A_OPMODE_SLEEP	BSX_OPMODE_SLEEP
#define ALGO_A_OPMODE_LOWPOWER BSX_OPMODE_REGULAR
#define ALGO_A_OPMODE_REGULAR BSX_OPMODE_REGULAR

#define ALGO_A_RANGE_2G	BSX_ACCRANGE_2G
#define ALGO_A_RANGE_4G	BSX_ACCRANGE_4G
#define ALGO_A_RANGE_8G	BSX_ACCRANGE_8G
#define ALGO_A_RANGE_16G BSX_ACCRANGE_16G

#define ALGO_A_BW_7_81HZ 	0
#define ALGO_A_BW_15_63HZ	1
#define ALGO_A_BW_31_25HZ	2
#define ALGO_A_BW_62_50HZ	3
#define ALGO_A_BW_125HZ		4


#define ALGO_M_OPMODE_UNKNOWN (-1)
#define ALGO_M_OPMODE_SLEEP BSX_OPMODE_SLEEP
#define ALGO_M_OPMODE_REGULAR BSX_OPMODE_REGULAR
#define ALGO_M_OPMODE_LOWPOWER (BSX_OPMODE_REGULAR + 0x80)
#define ALGO_M_OPMODE_HIGHACCURACY (BSX_OPMODE_REGULAR + 0x81)
#define ALGO_M_OPMODE_ALWAYSON (BSX_OPMODE_REGULAR + 0x82)


#define ALGO_G_OPMODE_SLEEP BSX_OPMODE_SLEEP
#define ALGO_G_OPMODE_REGULAR BSX_OPMODE_REGULAR

#define ALGO_G_RANGE_2000 BSX_GYRORANGE_2000DPS
#define ALGO_G_RANGE_1000 BSX_GYRORANGE_1000DPS
#define ALGO_G_RANGE_500  BSX_GYRORANGE_500DPS
#define ALGO_G_RANGE_250 BSX_GYRORANGE_250DPS
#define ALGO_G_RANGE_125 (BSX_GYRORANGE_250DPS + 1)

#define ALGO_NUM_DR 10


#define CFG_HW_DEP_A (1 << SENSOR_HW_TYPE_A)

#ifdef __GYROONLY_WORKING_MODE_SUPPORT__
#define CFG_HW_DEP_G (1 << SENSOR_HW_TYPE_G)
#else
#define CFG_HW_DEP_G ((1 << SENSOR_HW_TYPE_G) \
		| (1 << SENSOR_HW_TYPE_A))
#endif

#define CFG_HW_DEP_M ((1 << SENSOR_HW_TYPE_M) \
		| (1 << SENSOR_HW_TYPE_G))

#define CFG_HW_DEP_O ((1 << SENSOR_HW_TYPE_A)\
		| (1 << SENSOR_HW_TYPE_M)\
		| (1 << SENSOR_HW_TYPE_G))

#define CFG_HW_DEP_VG ((1 << SENSOR_HW_TYPE_A)\
		| (1 << SENSOR_HW_TYPE_G))

#define CFG_HW_DEP_VLA ((1 << SENSOR_HW_TYPE_A)\
		| (1 << SENSOR_HW_TYPE_G))

#define CFG_HW_DEP_VRV ((1 << SENSOR_HW_TYPE_A)\
		| (1 << SENSOR_HW_TYPE_M)\
		| (1 << SENSOR_HW_TYPE_G))

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
#define CFG_HW_DEP_GRV ((1 << SENSOR_HW_TYPE_A)\
		| (1 << SENSOR_HW_TYPE_G))
#define CFG_HW_DEP_GU CFG_HW_DEP_G
#define CFG_HW_DEP_MU CFG_HW_DEP_M
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
#define CFG_HW_DEP_GEOM (1 << SENSOR_HW_TYPE_M)
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
#define CFG_HW_DEP_STC (1 << SENSOR_HW_TYPE_A)
#define CFG_HW_DEP_STD (1 << SENSOR_HW_TYPE_A)
#endif

#endif
