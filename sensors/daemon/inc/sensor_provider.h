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
 * @file         sensor_provider.h
 * @date         "Thu Aug 22 09:30:08 2013 +0800"
 * @commit       "adb9206"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __SENSOR_PROVIDER_H
#define __SENSOR_PROVIDER_H

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

struct run_entity {
	pthread_t ptid;
	int32_t tid;

	volatile uint32_t started:1;
	uint32_t op_blk:1;
	volatile uint32_t sleeping:1;

	/* NOTE: limitations */
	uint32_t interval:16;

	/* optional, can be NULL */
	void *private_data;

	void *func_fp;
	/* the owner of the run entity */
	//void *owner;
	/* condition for thread */
	pthread_cond_t cond;

	void *(*func)(void *);
};


struct sensor_provider {
	const char *name;
	/* a bitmap of sensors supported */
	uint32_t sensors;

	uint32_t available:1;
	/* type of the provider, not used now */
	uint32_t type:4;
	uint32_t client_num:6;
	int32_t ref:6;

	hw_dep_set_t curr_hw_dep;
	struct list_node *clients;
	void *buf_out;
	void *private_data;
	pthread_mutex_t lock_ref;
	struct run_entity re;

	/* return value of 0 means success, otherwise failure */
	/* mandatory */
	int (*init)(struct sensor_provider *sp);
	/* mandatory: notify the provider that a channel will be enabled or disabled,
	 * thus the provider can change its internal state, and some
	 * provider such as an algo relies on this to know the new hw dep, etc */
	int (*on_ch_enabled)(struct channel *, int);
	/* mandatory: an owner such as an algo might provide this
	 * function to indicate the scheduling interval requirement
	 */
	int (*on_ch_interval_changed)(struct channel *, int);

	/* optional: an owner such an algo might provide this
	 * function to do singal processing
	 */
	void (*proc_data)(uint32_t);

	/* optional: */
	int32_t (*get_hint_proc_interval)();
	/* mandatory: get current dependency of hw needed to produce
	 * all the required products
	 */
	void (*get_curr_hw_dep)(hw_dep_set_t *);

	/* optional: after the h/ws are enabled/disabled as needed,
	 * the provider will be notified */
	int (*on_hw_dep_checked)(const hw_dep_set_t *);

	/* optional: */
	void (*exit)(void *);
};


void sp_preinit();
void sp_init();
void sp_dump();
struct sensor_provider *sp_scan_for_provider(struct channel *ch);
void sp_register_ch(struct sensor_provider *sp, struct channel *ch);
void sp_recalc_interval_re(struct sensor_provider *sp);
void sp_enable_ch(struct sensor_provider *sp, struct channel *ch, int enable);
void* re_proc(void* pparam);

void* re_proc(void* pparam);
#endif
