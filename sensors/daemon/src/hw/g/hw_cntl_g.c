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
 * @file         hw_cntl_g.c
 * @date         "Mon May 20 17:25:53 2013 +0800"
 * @commit       "7ddfe5e"
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

#include <linux/input.h>

#define LOG_TAG_MODULE "<hw_cntl_g>"
#include "sensord.h"

#define HW_PLACE_UNDEFINED (-1)

static int g_input_dev_num_g = -1;

static int g_fd_value_g = -1;

static int g_fd_input_ev_g = -1;

int g_place_g = HW_INFO_DFT_PLACE_G;
extern struct axis_remap axis_remap_tab_g[8];

static const struct value_map map_g_range[HW_G_RANGE_MAX] = {
	{HW_G_RANGE_2000, 0},
	{HW_G_RANGE_1000, 1},
	{HW_G_RANGE_500, 2},
	{HW_G_RANGE_250, 3},
	{HW_G_RANGE_125, 4}
};

static const struct value_map map_g_bw[HW_G_BW_MAX] = {
	{HW_G_BW_523, 0},
	{HW_G_BW_230, 1},
	{HW_G_BW_116, 2},
	{HW_G_BW_47, 3},
	{HW_G_BW_23, 4},
	{HW_G_BW_12, 5},
	{HW_G_BW_64, 6},
	{HW_G_BW_32, 7}
};

static const char *driver_names_tbl[] = {
	"bmg160",
	NULL
};

static int hw_gyro_read_xyzdata_input_ev(void *data)
{
	struct input_event ev;
	int sync = 0;
	int err = 0;
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;

	while (!sync) {
		err = read(g_fd_input_ev_g, &ev, sizeof(ev));
		if (err < (int)sizeof(ev)) {
			PERR("error reading event");
			err = -EIO;
			return -1;
		}

		switch (ev.type) {
		case EV_ABS:
			switch (ev.code) {
			case ABS_X:
				val->x = ev.value;
				break;
			case ABS_Y:
				val->y = ev.value;
				break;
			case ABS_Z:
				val->z = ev.value;
				break;
			}
			break;
		case EV_SYN:
			PDEBUG("EV_SYN got");
			sync = 1;
			err = 0;
			break;
		}
	}

	if (g_place_g >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_g + g_place_g);
	}

	return err;
}


static int hw_gyro_read_xyzdata(void *data)
{
	int err = 0;
	int tmp;
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;
	char buf[64] = "";

	val->x = 0;
	val->y = 0;
	val->z = 0;

	if (-1 != g_fd_value_g) {
		lseek(g_fd_value_g, 0, SEEK_SET);
		tmp = read(g_fd_value_g, buf, sizeof(buf) - 1);
		if (0 < tmp) {
			buf[tmp] = 0;
			tmp = sscanf(buf, "%11d %11d %11d",
					(&val->x), (&val->y), (&val->z));
			if (3 != tmp) {
				err = -EINVAL;
			}
		} else {
			PERR("error reading from acc");
		}
	}

	if (g_place_g >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_g + g_place_g);
	}

	PDEBUG("[gyro] x: %d y: %d z: %d", val->x, val->y, val->z);

	return err;
}


static int hw_gyro_set_opmode(BS_U8 mode)
{
	int err = 0;
	char path[64] = "";

	PDEBUG("gyro op_mode: %d", mode);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_g, SYSFS_NODE_NAME_BMG_OPMODE);

	err = sysfs_write_int(path, mode);

	return err;
}


static int hw_gyro_set_delay(struct sensor_hw *hw, BS_S32 delay)
{
	int err = 0;
	char path[128] = "";

	PDEBUG("delay: %d", delay);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV,
			g_input_dev_num_g,
			SYSFS_NODE_NAME_BMG_DELAY);

	err = sysfs_write_int(path, delay);
	if (!err) {
		hw->delay = delay;
	}

	return err;
}


static int hw_gyro_set_bw(struct sensor_hw_g *hw, uint32_t bw)
{
	int err = 0;
	char path[64] = "";
	int i;
	int tmp;

	PDEBUG("gyro bw: %d", bw);
	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_g, SYSFS_NODE_NAME_BMG_BW);

	for (i = 0; i < HW_G_BW_MAX; i++) {
		if (map_g_bw[i].l == (int)bw) {
			tmp = map_g_bw[i].r;
			break;
		}
	}

	if (i < HW_G_BW_MAX) {
		err = sysfs_write_int(path, tmp);
		if (!err) {
			hw->bw = bw;
		}
	} else {
		PWARN("invalid bw");
		err = -EINVAL;
	}

	return err;
}


static int hw_gyro_set_range(struct sensor_hw_g *hw, uint32_t range)
{
	int err = 0;
	char path[64] = "";
	int i;
	int tmp;

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_g, SYSFS_NODE_NAME_BMG_RANGE);

	for (i = 0; i < HW_G_RANGE_MAX; i++) {
		if (map_g_range[i].l == (int)range) {
			tmp = map_g_range[i].r;
			break;
		}
	}

	if (i < HW_G_RANGE_MAX) {
		err = sysfs_write_int(path, tmp);
		if (!err) {
			hw->range = range;
		}
	} else {
		PWARN("invalid range");
		err = -EINVAL;
	}

	return err;
}


static void hw_init_g_settings(struct sensor_hw *hw)
{
	/* currently we use suspend mode instead of deep suspend mode,
	 * so the settings can be done only once in init stage
	 */
	struct sensor_hw_g *hw_g = CONTAINER_OF(hw, struct sensor_hw_g, hw);
	hw_gyro_set_opmode(HW_G_OPMODE_NORMAL);
	eusleep(HW_INFO_DELAY_WAKE_UP_G);
	hw_gyro_set_bw(hw_g, HW_G_BW_47);
	hw_gyro_set_range(hw_g, HW_G_RANGE_2000);
}


#ifdef HW_G_USE_INPUT_EVENT
static int hw_gyro_set_enable_inputev(BS_U8 enable)
{
	int err = 0;
	char path[128] = "";

	PDEBUG("gyro enable: %d", enable);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_g, SYSFS_NODE_NAME_BMG_ENABLE);

	enable = !!enable;

	err = sysfs_write_int(path, enable);

	return err;
}
#endif


static int hw_init_g(struct sensor_hw *hw)
{
	int err = 0;
	char path[128] = "";
	int place;
	int i = 0;

	g_input_dev_num_g = -1;
	while(driver_names_tbl[i])
	{
		g_input_dev_num_g = sysfs_get_input_dev_num(driver_names_tbl[i]);
		i ++;
		if (g_input_dev_num_g >= 0)
		{
			break;
		}
	}
	PINFO("g_input_dev_num_g: %d", g_input_dev_num_g);

	g_fd_value_g = sysfs_open_input_dev_node(g_input_dev_num_g,
			SYSFS_NODE_NAME_BMG_VALUE, O_RDONLY);

	PINFO("g_fd_value_g: %d", g_fd_value_g);

	hw->fd_pollable = 1;
	g_fd_input_ev_g = hw->fd_poll = input_open_ev_fd(g_input_dev_num_g);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_g, "place");
	if (-1 != util_fs_is_file_exist(path))
	{
		err = sysfs_read_int(path, &place);
		if (!err && (HW_PLACE_UNDEFINED != place)) {
			PINFO("place of gyro in driver: %d", place);
			/* assign -1 to g_place_g to disble further remapping */
			g_place_g = -1;
		} else {
			err = 0;
		}
	}

	hw_init_g_settings(hw);

	return err;
}


int hw_enable_g(struct sensor_hw *hw, int enable)
{
	int err = 0;

	UNUSED_PARAM(hw);

	if (enable) {
		err = hw_gyro_set_opmode(HW_G_OPMODE_NORMAL);
		eusleep(HW_INFO_DELAY_WAKE_UP_G);
	} else {
		err = hw_gyro_set_opmode(HW_G_OPMODE_SUSPEND);
	}

#ifdef HW_G_USE_INPUT_EVENT
		err |= hw_gyro_set_enable_inputev(enable);
#endif
	return err;
}


static int hw_restore_cfg_g(struct sensor_hw *hw)
{
	int err = 0;
	/* TODO: currently BW is set to 32, hence this delay */
	int delay_drdy = 10000;
	/* if we use deep suspend mode, then this macro should be defined */
#ifdef HW_INFO_G_CFG_LOST_IN_SUSPEND
	struct sensor_hw_g *hw_g = CONTAINER_OF(hw, struct sensor_hw_g, hw);

	err = hw_g->set_bw(hw_g, hw_g->bw);
	err |= hw_g->set_range(hw_g, hw_g->range);
#endif
	UNUSED_PARAM(hw);
	eusleep(delay_drdy);

	return err;
}


struct sensor_hw_g g_hw_g = {
	hw: {
			name: DEV_NAME_G,
			type: SENSOR_HW_TYPE_G,
			init:hw_init_g,
			enable:hw_enable_g,
			restore_cfg:hw_restore_cfg_g,
			set_delay:hw_gyro_set_delay,
			get_data_nb:hw_gyro_read_xyzdata,
			get_data:hw_gyro_read_xyzdata_input_ev
		},

	set_bw:hw_gyro_set_bw,
	set_range:hw_gyro_set_range,
};

