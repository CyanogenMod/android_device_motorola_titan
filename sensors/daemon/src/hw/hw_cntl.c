/*!
 * @section LICENSE
 *
 * (C) Copyright 2014 Bosch Sensortec GmbH All Rights Reserved
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
 * @file         hw_cntl.c
 * @date         "Wed Apr 3 14:05:34 2013 +0800"
 * @commit       "5420311"
 *
 * @brief
 *
 * @detail
 *
 */

#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#include <linux/input.h>

#define LOG_TAG_MODULE "<hw_cntl>"
#include "sensord.h"

extern struct sensor_hw_a g_hw_a;
extern struct sensor_hw_m g_hw_m;
extern struct sensor_hw_g g_hw_g;

static const struct sensor_hw *g_list_hw[] = {
#if SPT_SENSOR_HW_A
	&g_hw_a.hw,
#endif

#if SPT_SENSOR_HW_G
	&g_hw_g.hw,
#endif

#if SPT_SENSOR_HW_M
	&g_hw_m.hw,
#endif

	/* use NULL as a mark of end */
	NULL,
};


void hw_remap_sensor_data(sensor_data_ival_t *data, axis_remap_t *remap)
{
	sensor_data_ival_t tmp;
	int t;

	tmp.v[0] = data->v[0];
	tmp.v[1] = data->v[1];
	tmp.v[2] = data->v[2];

	data->x = tmp.v[remap->rx];

	t = tmp.v[0];
	tmp.v[0] = tmp.v[1];
	tmp.v[1] = t;
	data->y = tmp.v[remap->ry];

	t = tmp.v[0];
	tmp.v[0] = tmp.v[2];
	tmp.v[2] = t;
	data->z = tmp.v[remap->rz];

	if (remap->sx) {
		data->x = 0 - data->x;
	}

	if (remap->sy) {
		data->y = 0 - data->y;
	}

	if (remap->sz) {
		data->z = 0 - data->z;
	}
}


struct sensor_hw * hw_get_hw_by_id(int hw_id)
{
	struct sensor_hw *hw = NULL;
	int i;

	i = 0;
	while (NULL != (hw = (struct sensor_hw *)g_list_hw[i++])) {
		if ((hw->type == hw_id) && hw->available) {
			break;
		}
	}

	if (NULL == hw) {
		PWARN("Invalid hw: %d", hw_id);
	}

	return hw;
}


int hw_ref_up(int hw_id)
{
	int err = 0;
	struct sensor_hw *hw = NULL;

	hw = hw_get_hw_by_id(hw_id);
	if (NULL == hw) {
		return -ENODEV;
	}

	pthread_mutex_lock(&hw->lock_ref);
	hw->ref += 1;
	if (1 == hw->ref) {
		if (NULL != hw->enable) {
			err = hw->enable(hw, 1);
			/* generally it takes some time to power up a device, but
			 * the wake up time is really hw specific, so
			 * it's assumed the delay is implemented in the enable()
			 * call back function
			 * */
			if (err) {
				PWARN("error enable hw: %s",
						hw->name);
			} else {
				hw->enabled = 1;

				if (NULL != hw->restore_cfg) {
					err = hw->restore_cfg(hw);
					if (err) {
						PWARN("error restoring hw cfg of %s", hw->name);
					}
				}
			}
		}
	}

	PINFO("ref count for hw %s: %d",
			hw->name, hw->ref);
	pthread_mutex_unlock(&hw->lock_ref);

	return err;
}


int hw_ref_down(int hw_id)
{
	int err = 0;
	struct sensor_hw *hw = NULL;

	hw = hw_get_hw_by_id(hw_id);
	if (NULL == hw) {
		return -ENODEV;
	}

	pthread_mutex_lock(&hw->lock_ref);
	if (1 == hw->ref) {
		if (NULL != hw->enable) {
			err = hw->enable(hw, 0);
			if (err) {
				PWARN("error disable hw: %s",
						hw->name);
			} else {
				hw->enabled = 0;
			}
		}
	}

	if (hw->ref > 0) {
		hw->ref -= 1;
	}

	PINFO("ref count for hw %s: %d",
			hw->name, hw->ref);
	pthread_mutex_unlock(&hw->lock_ref);

	return err;
}


int hw_cntl_init()
{
	int err = 0;
	int i = 0;
	struct sensor_hw *hw;

	i = 0;
	while (NULL != (hw = (struct sensor_hw *)g_list_hw[i++])) {
		hw->available = 0;
		hw->ref = 0;
		pthread_mutex_init(&hw->lock_ref, NULL);
		hw->drdy = 0;
		hw->fd_poll = -1;
		hw->ts_last_update = 0;

		PINFO("init hw: %s, type: %d", hw->name, hw->type);
		err = hw->init(hw);
		if (err) {
			PWARN("error init hw: %s %d", hw->name, err);
		} else {
			hw->available = 1;

			err = hw->enable(hw, 0);
			if (err) {
				PWARN("error disable hw: %s %d", hw->name, err);
			}
		}

	}

	return 0;
}


uint32_t hw_peek_data_status(uint32_t bitmap_hw_ids)
{
	uint32_t ret = 0;
	int err = 0;
	struct pollfd fds[SENSOR_HW_TYPE_MAX];
	int idx[SENSOR_HW_TYPE_MAX];
	int i;
	int count = 0;

	struct sensor_hw *hw;

	for (i = 0; i < SENSOR_HW_TYPE_MAX; i++) {
		if (!(bitmap_hw_ids & (1 << i))) {
			continue;
		}

		hw = hw_get_hw_by_id(i);
		if (NULL != hw) {
			if (!hw->fd_pollable) {
				ret |= (!!(hw->get_drdy_status(hw))) << i;
			} else {
				fds[count].fd = hw->fd_poll;
				idx[count] = i;
				fds[count].events = POLLIN;
				fds[count].revents = 0;

				count++;
			}
		}
	}

	if (count > 0) {
		err = poll(fds, count, 0);

		if (err >= 0) {
			for (i = 0; i < count; i++) {
				if (fds[i].revents & POLLIN) {
					ret |= (1 << idx[i]);
				}
			}
		} else {
			PERR("error during polling");
		}
	}


	return ret;
}


void hw_cntl_dump()
{
	int i;
	struct sensor_hw *hw;

	PINFO("dump of h/w info...");

	i = 0;
	while (NULL != (hw = (struct sensor_hw *)g_list_hw[i++])) {
		PINFO("name: %s", hw->name);
		PINFO("addr: %p", hw);
		PINFO("ref: %d", hw->ref);
	}
}
