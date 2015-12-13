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
 * @file         channel.h
 * @date         "Thu Mar 6 17:47:08 2014 +0800"
 * @commit       "09631c4"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __CHANNEL_H
#define __CHANNEL_H

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/fs.h>

#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "configure.h"
#include "options.h"

#include "util_misc.h"

#define INTV_PROC_MIN 5

enum CHANNEL_STATE {
	CHANNEL_STATE_SLEEP = 0,
	CHANNEL_STATE_NORMAL,
	CHANNEL_STATE_BG,
};


struct channel_cfg {
	/* sensor provider */
	char const *sp_name;

	uint16_t availability:3;
	/* deprecated, not used any more */
	uint16_t calib_bg:1;
	/* set to 1 if the delay makes no sense for
	 * this sensor, e.g.: proximity, light sensors etc
	 */
	uint16_t no_delay:1;

	/* channel does not need processing of
	 * data by sensor_provider, i.e.: sp only
	 * acts as an HW manager
	 */
	uint16_t bypass_proc:1;

	/* NOTE: limitations */
	uint16_t interval_min;
	/* NOTE: limitations */
	uint16_t interval_max;

	/* deprecated, not used any more */
	hw_dep_set_t dep_hw;
};


struct channel {
	const char const *name;
	/* NOTE: limitations */
	const int32_t type;
	/* id which identify a channel uniquely */
	const int8_t handle;

	struct channel_cfg cfg;

	volatile int32_t prev_state:4;
	volatile int32_t state:4;

	/* deprecated */
	int32_t data_status:4;
	/* NOTE: limitations */
	volatile int16_t interval;

	/* NOTE: limitations */
	uint32_t ts_last_ev;

	struct sensor_provider *sp;
	void *private_data;
	struct list_node client;
	pthread_mutex_t lock_state;

	/* mandatory: */
	int (*init)(struct channel *);
	/* optional: enable algorithm support */
	int (*enable)(struct channel *, int);
	/* optional: */
	void (*exit)();
	/* mandatory: function to fetch output data from its sensor provider */
	int (*get_data)(void *, int);
	/* some sensors might be enabled multiple times */
	/* optional */
};


struct data_filter {
	/* returns the valid data which is put in @out */
	int (*filter)(struct data_filter *df,
			const void *in, int len_in, void *out, int len_out);

	void *private_data;
};

extern int channel_cntl_init();
int channel_get_handle_list(int8_t *p_list, int32_t size);
extern void channel_cntl_destroy();
extern int channel_get_state(int handle);
extern int channel_get_cfg(int handle, struct channel_cfg *cfg);
extern int channel_set_cfg(int handle, const struct channel_cfg *cfg);

extern void channel_cntl_dump();

extern int channel_init_a(struct channel *);
extern void channel_destroy_a();
extern int get_data_a(void *buf, int n);	/* acceleration */
extern int channel_enable_a(int);
extern void on_interval_changed_a(int *);

extern int channel_init_g(struct channel *);
extern void channel_destroy_g();
extern int get_data_g(void *buf, int n);	/* gyro */
extern int channel_enable_g(int);
extern void on_interval_changed_g(int *);

extern int channel_init_m(struct channel *);
extern void channel_destroy_m();
extern int get_data_m(void *buf, int n);	/* magnetic */
extern int channel_enable_m(int);
extern void on_interval_changed_m(int *);

extern int channel_init_o(struct channel *);
extern void channel_destroy_o();
extern int get_data_o(void *buf, int n);	/* orientation */
extern void on_interval_changed_o(int *);
extern int channel_enable_o(int);

extern int channel_init_vg(struct channel *);
extern void channel_destroy_vg();
extern int get_data_vg(void *buf, int n);	/* gravity */
extern int channel_enable_vg(int);
extern void on_interval_changed_vg(int *);

extern int channel_init_vla(struct channel *);
extern void channel_destroy_vla();
extern int get_data_vla(void *buf, int n);	/* linear acc */
extern int channel_enable_vla(int);
extern void on_interval_changed_vla(int *);

extern int channel_init_vrv(struct channel *);
extern void channel_destroy_vrv();
extern int get_data_vrv(void *buf, int n);	/* rotation vector */
extern int channel_enable_vrv(int);
extern void on_interval_changed_vrv(int *);

#ifdef __FLIP_GESTURE__
/* gesture flip vector */
int channel_gest_flip_init(struct channel *ch);
int channel_gest_flip_get_data(void *pbuf, int n);
int channel_gest_flip_enable(struct channel *ch, int en);
#endif

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
extern int channel_init_grv(struct channel *);
extern int get_data_grv(void *buf, int n);
extern int channel_init_gu(struct channel *);
extern int get_data_gu(void *buf, int n);
extern int channel_init_mu(struct channel *);
extern int get_data_mu(void *buf, int n);
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
extern int channel_init_geom_rv(struct channel *);
extern int get_data_geom_rv(void *buf, int n);
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
extern int channel_init_stc(struct channel *);
extern int get_data_stc(void *buf, int n);
extern int channel_stc_enable(struct channel *ch, int en);

extern int channel_init_std(struct channel *);
extern int get_data_std(void *buf, int n);
extern int channel_std_enable(struct channel *ch, int en);

#endif

extern int channel_sgm_init(struct channel *);
extern int channel_sgm_get_data(void *buf, int n);
extern int channel_sgm_enable(struct channel *ch, int en);



void fusion_proc_data(uint32_t ts);
int get_hint_interval_fusion();


int channel_get_num();
struct channel *channel_get_ch(int handle);
#endif
