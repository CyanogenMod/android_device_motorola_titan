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
 * @file         event_handler.c
 * @date         "Tue July 18 13:50:34 2014 +0800"
 * @commit       "42bae15"
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

#define LOG_TAG_MODULE "<hw_cntl_a>"
#include "sensord.h"

#define HW_PLACE_UNDEFINED (-1)

static int g_input_dev_num_a = -1;

static int g_fd_value_a = -1;
static int g_fd_input_ev_a = -1;


int g_place_a = HW_INFO_DFT_PLACE_A;
extern struct axis_remap axis_remap_tab_a[8];


static const struct value_map map_a_range[HW_A_RANGE_MAX] = {
	{HW_A_RANGE_2G, 3},
	{HW_A_RANGE_4G, 5},
	{HW_A_RANGE_8G, 8},
	{HW_A_RANGE_16G, 12}
};

static const struct value_map map_a_bw[HW_A_BW_MAX] = {
	{HW_A_BW_8, 0x08},
	{HW_A_BW_16, 0x09},
	{HW_A_BW_31, 0x0a},
	{HW_A_BW_63, 0x0b},
	{HW_A_BW_125, 0x0c},
};

static const char * driver_names_tbl[] =
{
	"bma2x2",
	"bma250",
	NULL
};

#define DELAY_REF_BW_MAX 256000 /* in us */

static void hw_set_fast_calib_offset()
{
#ifdef HW_A_OFFSET_USE_FS
	int fd_fast_calib_a;

	int offset_filt_x;
	int offset_filt_y;
	int offset_filt_z;

	char *filename;
	int tmp;

	char path[128] = "";
	char buf[64] = "";

	filename = SENSOR_CFG_FILE_FAST_CALIB_A;
	fd_fast_calib_a = open(filename, O_RDONLY);
	if (-1 != fd_fast_calib_a) {
		tmp = read(fd_fast_calib_a, buf, ARRAY_SIZE(buf) - 1);
		close(fd_fast_calib_a);
		if (tmp > 0) {
			tmp = sscanf(buf, "%11d,%11d,%11d",
					&offset_filt_x,
					&offset_filt_y,
					&offset_filt_z);
		} else {
			PERR("error read from file: %s", filename);
			return;
		}

		if (3 != tmp) {
			PWARN("invalid format in file: %s", filename);
			return;
		} else {
			PINFO("offset_a: %d %d %d",
					offset_filt_x,
					offset_filt_y,
					offset_filt_z);
		}
	} else {
		PERR("error openning file: %s", filename);
		return;
	}


	tmp = 0;
	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a,
			SYSFS_NODE_NAME_BMA_OFFSET_FILT_X);
	tmp |= sysfs_write_int(path, offset_filt_x) ? (1<<0) : 0;

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a,
			SYSFS_NODE_NAME_BMA_OFFSET_FILT_Y);
	tmp |= sysfs_write_int(path, offset_filt_y) ? (1<<1) : 0;

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a,
			SYSFS_NODE_NAME_BMA_OFFSET_FILT_Z);
	tmp |= sysfs_write_int(path, offset_filt_z) ? (1<<2) : 0;

	if (0 == tmp) {
		PINFO("fast_calib_a set successfully");
	} else {
		PERR("error setting fast calib offset: %#x", tmp);
	}
#endif
}


static int hw_acc_read_xyzdata_fr(sensor_data_ival_t *val)
{
	int err = 0;
	int tmp;

	char buf[64] = "";

	val->x = 0;
	val->y = 0;
	val->z = 0;

	if (-1 != g_fd_value_a) {
		lseek(g_fd_value_a, 0, SEEK_SET);
		tmp = read(g_fd_value_a, buf, sizeof(buf) - 1);
		if (0 < tmp) {
			buf[tmp] = 0;
			tmp = sscanf(buf, "%11d %11d %11d",
					(&val->x), (&val->y), (&val->z));
			if (3 != tmp) {
				err = -EINVAL;
			}
		} else {
			PERR("error reading from acc");
			err = -EIO;
		}
	}

	PDEBUG("[acc] x: %d y: %d z: %d", val->x, val->y, val->z);

	return err;
}


static int hw_acc_read_xyzdata(void *data)
{
	int err;
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;

	err = hw_acc_read_xyzdata_fr(val);
	if (g_place_a >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_a + g_place_a);
	}

	return err;
}


static int hw_acc_read_xyzdata_input_ev(void *data)
{
	struct input_event ev;
	int sync = 0;
	int err = 0;
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;

	while (!sync) {
		err = read(g_fd_input_ev_a, &ev, sizeof(ev));
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

#ifdef HW_A_DATA_FULLRANGE
	val->x = val->x >> (16 - HW_INFO_BITWIDTH_A);
	val->y = val->y >> (16 - HW_INFO_BITWIDTH_A);
	val->z = val->z >> (16 - HW_INFO_BITWIDTH_A);
#endif
	if (g_place_a >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_a + g_place_a);
	}

	return err;
}


static int hw_acc_set_grange(struct sensor_hw_a *hw, uint32_t range)
{
	int err = 0;

	char path[128] = "";
	int tmp;
	int i;

	PDEBUG("grange: %d", range);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a, SYSFS_NODE_NAME_BMA_GRANGE);

	for (i = 0; i < ARRAY_SIZE(map_a_range); i++) {
		if (map_a_range[i].l == (int)range) {
			break;
		}
	}

	if (i < ARRAY_SIZE(map_a_range)) {
		tmp = map_a_range[i].r;
		err = sysfs_write_int(path, tmp);

		if (!err) {
			hw->range = range;
		}
	} else {
		PWARN("invalid grange");
		err = -EINVAL;
	}

	return err;
}


static int hw_acc_set_opmode(BS_U8 mode)
{
	int err = 0;

	char path[128] = "";

	PDEBUG("acc op_mode: %d", mode);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a, SYSFS_NODE_NAME_BMA_MODE);

	err = sysfs_write_int(path, mode);

	return err;
}

static int hw_acc_set_delay(struct sensor_hw *hw, BS_S32 delay)
{
	int err = 0;
	char path[128] = "";

	PDEBUG("delay: %d", delay);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV,
			g_input_dev_num_a,
			SYSFS_NODE_NAME_BMA_DELAY);

	err = sysfs_write_int(path, delay);
	if (!err) {
		hw->delay = delay;
	}

	return err;
}


static int hw_acc_set_bandwidth(struct sensor_hw_a *hw, uint32_t bw)
{
	int err = 0;

	char path[128] = "";
	int tmp;
	int i;

	PINFO("bw: %d, hw->bw: %d", bw, hw->bw);

	if (bw == hw->bw) {
		return 0;
	}

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a, SYSFS_NODE_NAME_BMA_BW);

	for (i = 0; i < ARRAY_SIZE(map_a_bw); i++) {
		if (map_a_bw[i].l == (int)bw) {
			tmp = (BS_U8)map_a_bw[i].r;
			break;
		}
	}

	if (i < ARRAY_SIZE(map_a_bw)) {
		err = sysfs_write_int(path, tmp);
		if (!err) {
			hw->bw = bw;
			eusleep(DELAY_REF_BW_MAX >> i);
		}
	} else {
		PWARN("invalid bw");
		err = -EINVAL;
	}

	return err;
}


static void hw_init_a_settings(struct sensor_hw *hw)
{
	/* currently we use suspend mode instead of deep suspend mode,
	 * so the settings can be done only once in init stage
	 */
	struct sensor_hw_a *hw_a = CONTAINER_OF(hw, struct sensor_hw_a, hw);
	hw_acc_set_opmode(HW_A_OPMODE_NORMAL);
	eusleep(HW_INFO_DELAY_WAKE_UP_A);
	hw_acc_set_grange(hw_a, HW_A_RANGE_2G);

	hw_a->bw = HW_A_BW_INVALID;
}


static int hw_init_a(struct sensor_hw *hw)
{
	int err = 0;
	struct sensor_hw_a *hw_a = CONTAINER_OF(hw, struct sensor_hw_a, hw);
	char path[128] = "";
	int place;
	int i = 0;

	g_input_dev_num_a = -1;
	while (driver_names_tbl[i] != NULL)
	{
		g_input_dev_num_a = sysfs_get_input_dev_num(driver_names_tbl[i]);
		i ++;
		if (g_input_dev_num_a >= 0)
		{
			break;
		}
	}
	PINFO("g_input_dev_num_a: %d", g_input_dev_num_a);

	g_fd_value_a = sysfs_open_input_dev_node(g_input_dev_num_a,
			SYSFS_NODE_NAME_BMA_VALUE, O_RDONLY);

	PINFO("g_fd_value_a: %d", g_fd_value_a);

	hw_set_fast_calib_offset();

	hw_init_a_settings(hw);

	hw->fd_pollable = 1;
	g_fd_input_ev_a = hw->fd_poll = input_open_ev_fd(g_input_dev_num_a);
	hw_a->data_bits = HW_INFO_BITWIDTH_A;

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_a, "place");
	err = sysfs_read_int(path, &place);
	if (!err && (HW_PLACE_UNDEFINED != place)) {
		PINFO("place of acc in driver: %d", place);
		/* assign -1 to g_place_m to disble further remapping */
		g_place_a = -1;
	} else {
		err = 0;
	}

	return err;
}


static int hw_enable_a(struct sensor_hw *hw, int enable)
{
	int err = 0;
	int i;
	struct sensor_hw_a *hw_a = CONTAINER_OF(hw, struct sensor_hw_a, hw);

	if (enable) {
		err = hw_acc_set_opmode(HW_A_OPMODE_NORMAL);
		eusleep(HW_INFO_DELAY_WAKE_UP_A);

		for (i = 0; i < ARRAY_SIZE(map_a_bw); i++) {
			if (map_a_bw[i].l == (int)hw_a->bw) {
				break;
			}
		}

		if (i >= ARRAY_SIZE(map_a_bw)) {
			i = 0;
		}

		eusleep(DELAY_REF_BW_MAX >> i);
	} else {
		err = hw_acc_set_opmode(HW_A_OPMODE_SUSPEND);
	}

	return err;
}


static int hw_restore_cfg_a(struct sensor_hw *hw)
{
	int err = 0;
	/* if we use deep suspend mode, then this macro should be defined */
#ifdef HW_INFO_A_CFG_LOST_IN_SUSPEND
	struct sensor_hw_a *hw_a = CONTAINER_OF(hw, struct sensor_hw_a, hw);

	err = hw_a->set_bw(hw_a, hw_a->bw);
	err |= hw_a->set_range(hw_a, hw_a->range);
#endif
	UNUSED_PARAM(hw);

	return err;
}


struct sensor_hw_a g_hw_a = {
	hw: {
			name: DEV_NAME_A,
			type: SENSOR_HW_TYPE_A,
			init:hw_init_a,
			enable:hw_enable_a,
			restore_cfg:hw_restore_cfg_a,
			set_delay:hw_acc_set_delay,
			get_data_nb:hw_acc_read_xyzdata,
			get_data:hw_acc_read_xyzdata_input_ev
		},

	set_bw:hw_acc_set_bandwidth,
	set_range:hw_acc_set_grange,
};
