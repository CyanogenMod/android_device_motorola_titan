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
 * @file         channels.c
 * @date         "Wed May 15 16:12:33 2013 +0800"
 * @commit       "6aa66ba"
 *
 * @brief
 *
 * @detail
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define LOG_TAG_MODULE "<channels.c>"
#include "sensord.h"
#include "sensor_def.h"
#include "configure.h"


struct channel g_list_ch[] = {
	{
		name: "ACCELERATION",
		type: SENSOR_TYPE_A,
		handle: SENSOR_HANDLE_ACCELERATION,

		cfg:
			{
				availability: CFG_CHANNEL_A,
				calib_bg: 0,
				sp_name: "SP_FUSION",
#ifdef CFG_BYPASS_PROC_CHANNEL_A
				bypass_proc:1
#endif
			},

		init: channel_init_a,
		get_data: get_data_a,
	},

	{
		name: "GYROSCOPE",
		type: SENSOR_TYPE_G,
		handle: SENSOR_HANDLE_GYROSCOPE,

		cfg: 
			{
				availability: CFG_CHANNEL_G,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_g,
		get_data: get_data_g,
	},

	{
		name: "MAGNETIC",
		type: SENSOR_TYPE_M,
		handle: SENSOR_HANDLE_MAGNETIC_FIELD,

		cfg:
			{
				availability: CFG_CHANNEL_M,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_m,
		get_data: get_data_m,
	},

	{
		name: "ORIENTATION",
		type: SENSOR_TYPE_O,
		handle: SENSOR_HANDLE_ORIENTATION,

		cfg:
			{
				availability: CFG_CHANNEL_O,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_o,
		get_data: get_data_o,
	},

	{
		name: "GRAVITY",
		type: SENSOR_TYPE_VG,
		handle: SENSOR_HANDLE_GRAVITY,

		cfg:
			{
				availability: CFG_CHANNEL_VG,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_vg,
		get_data: get_data_vg,
	},

	{
		name: "LINEAR_ACCELERATION",
		type: SENSOR_TYPE_VLA,
		handle: SENSOR_HANDLE_LINEAR_ACCELERATION,

		cfg:
			{
				availability: CFG_CHANNEL_VLA,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			}, 

		init: channel_init_vla,
		get_data: get_data_vla,
	},

	{
		name: "ROTATION_VECOTR",
		type: SENSOR_TYPE_VRV,
		handle: SENSOR_HANDLE_ROTATION_VECTOR,

		cfg:
			{
				availability: CFG_CHANNEL_VRV,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_vrv,
		get_data: get_data_vrv,
	},

#ifdef __FLIP_GESTURE__
	{
		name: "GEST_FLIP",
		type: SENSOR_TYPE_GEST_FLIP,
		handle: SENSOR_HANDLE_GEST_FLIP,

		cfg:
			{
				availability: VIRTUAL,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_gest_flip_init,
		get_data: channel_gest_flip_get_data,
		enable: channel_gest_flip_enable,
	},
#endif
	{
		name: "SW SIGNIFICANT SENSOR",
		type: SENSOR_TYPE_SW_SIGNIFICANT_MOTION,
		handle: SENSOR_HANDLE_SW_SIGNIFCANT_MOITON,

		cfg:
			{
				availability: VIRTUAL,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_sgm_init,
		get_data: channel_sgm_get_data,
		enable: channel_sgm_enable,
	},

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
	{
		name: "GAME_ROTATION_VECOTR",
		type: SENSOR_TYPE_GRV,
		handle: SENSOR_HANDLE_GAME_ROTATION_VECTOR,

		cfg:
			{
				availability: CFG_CHANNEL_GRV,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_grv,
		get_data: get_data_grv,
	},

	{
		name: "GYROSCOPE_UNCALIBRATED",
		type: SENSOR_TYPE_GYU,
		handle: SENSOR_HANDLE_GYROSCOPE_UNCALIBRATED,

		cfg:
			{
				availability: CFG_CHANNEL_GYU,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_gu,
		get_data: get_data_gu,
	},

	{
		name: "MAGNETIC_UNCALIBRATED",
		type: SENSOR_TYPE_MU,
		handle: SENSOR_HANDLE_MAGNETIC_FIELD_UNCALIBRATED,

		cfg:
			{
				availability: CFG_CHANNEL_MU,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_mu,
		get_data: get_data_mu,
	},
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	{
		name: "GEOMAGNETIC_ROTATION_VECTOR",
		type: SENSOR_TYPE_GEOM_RV,
		handle: SENSOR_HANDLE_GEOMAGNETIC_ROTATION_VECTOR,

		cfg:
			{
				availability: CFG_CHANNEL_GEOM_RV,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_geom_rv,
		get_data: get_data_geom_rv,
	},
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
	{
		name: "STEP_COUNTER",
		type: SENSOR_TYPE_STC,
		handle: SENSOR_HANDLE_STEP_COUNTER,

		cfg:
			{
				availability: CFG_CHANNEL_STC,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_stc,
		get_data: get_data_stc,
		enable: channel_stc_enable,
	},
	{
		name: "STEP_DETECTOR",
		type: SENSOR_TYPE_STD,
		handle: SENSOR_HANDLE_STEP_DETECTOR,

		cfg:
			{
				availability: CFG_CHANNEL_STD,
				calib_bg: 0,
				sp_name: "SP_FUSION"
			},

		init: channel_init_std,
		get_data: get_data_std,
		enable: channel_std_enable,
	},
#endif


};

int channel_get_num()
{
	return ARRAY_SIZE(g_list_ch);
}
