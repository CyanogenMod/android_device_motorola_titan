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
 *  Disclaimer
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
 * @file         algo_adapter.h
 * @date         "Thu Mar 20 17:18:01 2014 +0800"
 * @commit       "8a08ed2"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __ALGO_ADAPTER_H
#define __ALGO_ADAPTER_H
#include "configure.h"
#include "options.h"
#include "algo_if.h"

#define CFG_TOLERANCE_TIME_PRECISION 2

#define ALGO_OPMODE_OFF 0

enum ALGO_FUSION_MODE {
	ALGO_FUSION_MODE_COMPASS = 0,
	ALGO_FUSION_MODE_M4G,
	ALGO_FUSION_MODE_IMU,
	ALGO_FUSION_MODE_9DOF,
};

enum ALGO_WORKING_MODE {
	ALGO_WORKING_MODE_SLEEP,
	ALGO_WORKING_MODE_ACCONLY,
	ALGO_WORKING_MODE_MAGONLY,
	ALGO_WORKING_MODE_ACCMAG,
	ALGO_WORKING_MODE_GYONLY,
	ALGO_WORKING_MODE_ACCGYRO,
	ALGO_WORKING_MODE_AMG,
	ALGO_WORKING_MODE_IMUPLUS,
	ALGO_WORKING_MODE_COMPASS,
	ALGO_WORKING_MODE_M4G,
#ifdef __FMC_OFF__
	ALGO_WORKING_MODE_NDOF_FMC_OFF,
#else
	ALGO_WORKING_MODE_NDOF,
#endif
};

#define ALGO_MODULE_ID_ACC (uint32_t)'_acc'
#define ALGO_MODULE_ID_MAG (uint32_t)'_mag'
#define ALGO_MODULE_ID_GYR (uint32_t)'_gyr'
#define ALGO_MODULE_ID_CMP (uint32_t)'_cmp'	/* compass */
#define ALGO_MODULE_ID_M4G (uint32_t)'_m4g'
#define ALGO_MODULE_ID_IMU (uint32_t)'_imu'
#define ALGO_MODULE_ID_9DOF (uint32_t)'_9df'
#define ALGO_MODULE_ID_VLA (uint32_t)'_vla'

#define ALGO_MODULE_ID_GEST_FLIP (uint32_t)'_gfp'       /* flip */

struct algo_product;
#define ALGO_PRODUCT_SET_DR_MIN(id, sensor, dr)\
	do {\
		struct algo_product *ap = NULL;\
		ap = fusion_get_product(id);\
		if (NULL != ap) {\
			ap->dr_##sensor##_min = dr;\
		}\
	} while (0)


#define ALGO_PRODUCT_SET_DR_MAX(id, sensor, dr)\
	do {\
		struct algo_product *ap = NULL;\
		ap = fusion_get_product(id);\
		if (NULL != ap) {\
			ap->dr_##sensor##_max = dr;\
		}\
	} while (0)


#define ALGO_PRODUCT_REGULATE_DR_ACC(ap)\
	do {\
		if (ap->dr_a < ap->dr_acc_min) {\
			ap->dr_a = ap->dr_acc_min;\
		}\
		\
		if (ap->dr_a > ap->dr_acc_max) {\
			ap->dr_a = ap->dr_acc_max;\
		}\
	} while (0)

#define ALGO_PRODUCT_REGULATE_DR_MAG(ap)\
	do {\
		if (ap->dr_m < ap->dr_mag_min) {\
			ap->dr_m = ap->dr_mag_min;\
		}\
		\
		if (ap->dr_m > ap->dr_mag_max) {\
			ap->dr_m = ap->dr_mag_max;\
		}\
	} while (0)

#define ALGO_PRODUCT_REGULATE_DR_GYRO(ap)\
	do {\
		if (ap->dr_g < ap->dr_gyro_min) {\
			ap->dr_g = ap->dr_gyro_min;\
		}\
		\
		if (ap->dr_g > ap->dr_gyro_max) {\
			ap->dr_g = ap->dr_gyro_max;\
		}\
	} while (0)


#define ALGO_PRODUCT_REGULATE_DR(ap)\
	do {\
		ALGO_PRODUCT_REGULATE_DR_ACC(ap);\
		ALGO_PRODUCT_REGULATE_DR_MAG(ap);\
		ALGO_PRODUCT_REGULATE_DR_GYRO(ap);\
	} while (0)


struct algo_module {
	const char *name;
	uint32_t id;

	int op_mode:3;
	int ref:5;

	void (*enable)(struct algo_module *, int);
};


struct algo_product {
	const uint32_t type;

	const int32_t id;

	uint32_t enable:1;
	/* bypass processing by algorithm */
	uint32_t bypass_proc:1;

	uint32_t dr_a:8;
	uint32_t dr_m:8;
	uint32_t dr_g:8;

	uint32_t dr_acc_min:8;
	uint32_t dr_acc_max:8;

	uint32_t dr_mag_min:8;
	uint32_t dr_mag_max:8;

	uint32_t dr_gyro_min:8;
	uint32_t dr_gyro_max:8;
};

extern BS_U8 g_dr_a;
extern BS_U8 g_dr_m;
extern BS_U8 g_dr_g;

struct algo_usecase_tbl_type {
	int         id;         /* relative with ALGO_FUSION_MODE */
	const char *filename;   /* usecase file name to be detected */
};

#ifdef __DEBUG_TIMING_ACCURACY__
typedef struct timing_accuracy_type{
	uint32_t num_overlap_lvl1;
	uint32_t num_overlap_lvl2;
	uint32_t biggest;
	uint32_t tolerance_low;
	uint32_t tolerance_high;
	uint32_t num_total;
}timeing_accuracy_t;
#endif

typedef struct
{
	ts_calibparam calibParam;
	BSX_U8	accuracy;
	BSX_U8 crc;
} calibprofile_crc_t;

/*
 * @brief This function returns current use case
 *
 * @param none
 *
 * @return use case id relate with ALGO_FUSION_MODE
 */
int algo_get_current_usecase(void);

/*!
 * @brief This function loads spec files from file system and set them into
 *            bsx_init, after that it calls algo_adapter_init to set up
 *            sensor_hw and working modes
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 */
int algo_init(void);

int algo_enable_product(struct algo_product *ap, int enable);

int algo_set_opmode(int32_t sensor_type, int32_t opmode);

BS_U8 algo_proc_data_a(BS_S32 ts);
int algo_get_proc_data_a(sensor_data_t *pdata);
unsigned char algo_get_data_accuracy_a();

BS_U8 algo_proc_data_m(BS_S32 ts);
int algo_get_proc_data_m(sensor_data_t *pdata);
unsigned char algo_get_data_accuracy_m();

BS_U8 algo_proc_data_g(BS_S32 ts);
int algo_get_proc_data_g(sensor_data_t *pdata);
unsigned char algo_get_data_accuracy_g();

BS_U8 algo_proc_data_o(BS_S32 ts);
int algo_get_proc_data_o(sensor_data_t *pdata);
unsigned char algo_get_data_accuracy_o();

BS_U8 algo_proc_data_ro(BS_S32 ts);
int algo_get_proc_data_ro(sensor_data_t *pdata);
unsigned char algo_get_data_accuracy_ro();

BS_U8 algo_proc_data_vg(BS_S32 ts);
int algo_get_proc_data_vg(sensor_data_t *pdata);

BS_U8 algo_proc_data_vla(BS_S32 ts);
int algo_get_proc_data_vla(sensor_data_t *pdata);

BS_U8 algo_proc_data_vrv(BS_S32 ts);
int algo_get_proc_data_vrv(sensor_data_t *pdata);

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
int algo_get_proc_data_grv(sensor_data_t *pdata);
int algo_get_proc_data_gu(sensor_data_t *pdata);
int algo_get_proc_data_mu(sensor_data_t *pdata);
#endif

int algo_get_proc_data_gest_flip(sensor_data_t *pdata);

void algo_load_calib_profile(char magic);
void algo_save_calib_profile(char magic);

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
int algo_get_proc_data_sgm(sensor_data_t *pdata);
int algo_get_proc_data_stc(sensor_data_t *pdata);
int algo_get_proc_data_std(sensor_data_t *pdata);
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
int algo_get_proc_data_geom_rv(sensor_data_t *pdata);
#endif

void algo_on_interval_changed(struct algo_product *ap, int *interval);
BS_S32 algo_proc_data(uint32_t ts);

void algo_adapter_init();
void algo_mod_init();
int algo_get_hint_interval();
void algo_get_curr_hw_dep(hw_dep_set_t *dep);
int algo_on_hw_dep_checked(const hw_dep_set_t *dep);

#ifdef __DEBUG_TIMING_ACCURACY__
void init_timing_tolerance(int type, uint32_t interval);
void print_timing_accuracy(void);
#endif

#ifdef CFG_USE_DATA_LOG
// data log functions
void data_log_init();
void data_log_input_to_algo(libraryinput_t all_sensor_data, BS_U32 ts);
void data_log_output(int fd, sensor_data_t *pdata);
void data_log_output_vrv(int fd, sensor_data_t *pdata);
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
void data_log_output_gu(int fd, sensor_data_t *pdata);
void data_log_output_mu(int fd, sensor_data_t *pdata);
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
void data_log_output_step_detect(int fd, uint64_t value);
#endif

#endif

#endif
