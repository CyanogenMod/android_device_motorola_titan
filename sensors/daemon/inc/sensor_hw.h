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
 * @file         sensor_hw.h
 * @date         "Wed Apr 17 14:17:11 2013 +0800"
 * @commit       "c17081e"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __SENSOR_HW_H
#define __SENSOR_HW_H
#include <stdint.h>

/* a bitmap of dependencies */
/* NOTE: limitations: hw number limited to 16 */
typedef int16_t hw_dep_set_t;

typedef struct axis_remap {
	unsigned int rx: 2;
	unsigned int ry: 2;
	unsigned int rz: 2;

	unsigned int sx: 2;
	unsigned int sy: 2;
	unsigned int sz: 2;
} axis_remap_t;


struct sensor_hw {
	const char const *name;

	/* defined as SENSOR_HW_TYPE_X */
	const int32_t type;

	uint32_t available:1;
	uint32_t fd_pollable:1;
	/* NOTE: limitations: */
	uint32_t wakeup_time:10;

	int32_t ref:8;
	uint32_t enabled:1;
	uint32_t drdy:1;
	int32_t delay:10;

	int fd_poll;
	uint32_t ts_last_update;

	void *private_data;

	pthread_mutex_t lock_ref;
	/* mandatory: */
	int (*init)(struct sensor_hw *);
	/* mandatory: */
	int (*enable)(struct sensor_hw *, int);

	/* optional: */
	int (*restore_cfg)(struct sensor_hw *);
	/* optional: */
	int (*set_delay)(struct sensor_hw *, int);

	/* mandatory if the device is not pollable */
	int (*get_drdy_status)(struct sensor_hw *);

	/* either get_data_nb() or get_data() or both should be provided */
	/* optional: 
	 * this function should get the data from the device in a non-blocking way
	 */
	int (*get_data_nb)(void *val);
	/* optional: this function get the data from the device and it might block */
	int (*get_data)(void *val);
};


struct sensor_hw_a {
	struct sensor_hw hw;

	uint32_t bw:12;
	uint32_t range:6;
	uint32_t data_bits:5;
	uint32_t place:6;

	int (*set_bw)(struct sensor_hw_a *, uint32_t bw);
	int (*set_range)(struct sensor_hw_a *, uint32_t range);
};


struct sensor_hw_m {
	struct sensor_hw hw;

	uint32_t data_bits:5;
	uint32_t place:6;
};


struct sensor_hw_g {
	struct sensor_hw hw;

	uint32_t bw:12;
	uint32_t range:6;
	uint32_t data_bits:5;
	uint32_t place:6;

	int (*set_bw)(struct sensor_hw_g *, uint32_t bw);
	int (*set_range)(struct sensor_hw_g *, uint32_t range);
};
#endif
