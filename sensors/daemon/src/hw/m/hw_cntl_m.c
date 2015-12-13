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
 * @file         hw_cntl_m.c
 * @date         "Wed May 22 18:08:39 2013 +0800"
 * @commit       "062e8e2"
 *
 * @brief
 *
 * @detail
 *
 */

#include <stdio.h>
#include <errno.h>

#include <linux/input.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_TAG_MODULE "<hw_cntl_m>"
#include "sensord.h"

#define HW_PLACE_UNDEFINED (-1)

static int g_input_dev_num_m = -1;

static int g_fd_value_m = -1;
static int g_fd_op_mode_m = -1;
static int g_fd_input_ev_m = -1;

int g_place_m = HW_INFO_DFT_PLACE_M;
extern struct axis_remap axis_remap_tab_m[8];

struct bmm_cfg {
	int rept_xy;
	int rept_z;
} g_bmm_cfg;

static const char *driver_names_tbl[] = {
	"bmm050",
	NULL
};

static int hw_mag_set_opmode(BS_U8 mode)
{
	int err = 0;
	char path[128] = "";

	PDEBUG("mag op_mode: %d", mode);
	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_m, SYSFS_NODE_NAME_BMM_OP_MODE);

	err = sysfs_write_int(path, mode);

	return err;
}

static void hw_mag_validate_val(sensor_data_ival_t *val)
{
	sensor_data_ival_t tmp_val = *val;

	PDEBUG("%d %d %d", tmp_val.x, tmp_val.y, tmp_val.z);
	if ((int32_t)(INT_MIN) != tmp_val.x) {
	} else {
		PWARN("overflow in x");
		val->x = -(int32_t)(SHRT_MAX);
	}

	if ((int32_t)(INT_MIN) != tmp_val.y) {
	} else {
		PWARN("overflow in y");
		val->y = -(int32_t)(SHRT_MAX);
	}

	if ((int32_t)(INT_MIN) != tmp_val.z) {
	} else {
		PWARN("overflow in z");
		val->z = -(int32_t)(SHRT_MAX);
	}
}

static int hw_mag_read_xyzdata(void *data)
{
	int err = 0;
	int tmp;
	char buf[64] = "";
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;

	val->x = 0;
	val->y = 0;
	val->z = 0;

	if (-1 != g_fd_value_m) {
		lseek(g_fd_value_m, 0, SEEK_SET);
		tmp = read(g_fd_value_m, buf, sizeof(buf) - 1);
		if (0 < tmp) {
			buf[tmp] = 0;
			tmp = sscanf(buf, "%11d %11d %11d",
					(&val->x), (&val->y), (&val->z));

			if (3 != tmp) {
				err = -EINVAL;
			}
		} else {
			PERR("error reading from mag");
			err = -EIO;
		}
	}

	if (-1 != g_fd_op_mode_m) {
		lseek(g_fd_op_mode_m, 0, SEEK_SET);
		/* forced mode */
		tmp = write(g_fd_op_mode_m, "1", 1);
		if (1 != tmp) {
			PERR("err setting forced mode");
		}
	}

	hw_mag_validate_val(val);

	if (g_place_m >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_m + g_place_m);
	}

	PDEBUG("[mag] x: %hd y: %hd z: %hd", val->x, val->y, val->z);
	return err;
}


static int hw_mag_read_xyzdata_input_ev(void *data)
{
	struct input_event ev;
	int sync = 0;
	int err = 0;
	sensor_data_ival_t *val = (sensor_data_ival_t *)data;

	while (!sync) {
		err = read(g_fd_input_ev_m, &ev, sizeof(ev));
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

	hw_mag_validate_val(val);
	if (g_place_m >= 0) {
		hw_remap_sensor_data(val, axis_remap_tab_m + g_place_m);
	}

	return err;
}


#ifdef HW_M_USE_INPUT_EVENT
static int hw_mag_set_enable_input_ev(BS_U8 enable)
{
	int err = 0;

	char path[128] = "";

	PDEBUG("mag enable: %d", enable);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_m, SYSFS_NODE_NAME_BMM_ENABLE);

	enable = !!enable;

	err = sysfs_write_int(path, enable);

	return err;
}
#endif


static int hw_mag_set_delay(struct sensor_hw *hw, BS_S32 delay)
{
	int err = 0;
	char path[128] = "";

	PINFO("delay: %d", delay);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV,
			g_input_dev_num_m,
			SYSFS_NODE_NAME_BMM_DELAY);

	err = sysfs_write_int(path, delay);
	if (!err) {
		hw->delay = delay;
	}

	return err;
}



static void hw_init_m_settings(struct sensor_hw *hw)
{
	g_bmm_cfg.rept_xy = BMM_REGULAR_REPXY;
	g_bmm_cfg.rept_z = BMM_REGULAR_REPZ;
	hw->private_data = &g_bmm_cfg;
}


static int hw_init_m(struct sensor_hw *hw)
{
	int err = 0;
	char path[128] = "";
	int place;
	int i = 0;

	g_input_dev_num_m = -1;
	while(driver_names_tbl[i])
	{
		g_input_dev_num_m = sysfs_get_input_dev_num(driver_names_tbl[i]);
		i ++;
		if (g_input_dev_num_m >= 0)
		{
			break;
		}
	}
	PINFO("g_input_dev_num_m: %d", g_input_dev_num_m);

	g_fd_op_mode_m = sysfs_open_input_dev_node(g_input_dev_num_m,
			SYSFS_NODE_NAME_BMM_OP_MODE, O_WRONLY);
	g_fd_value_m = sysfs_open_input_dev_node(g_input_dev_num_m,
			SYSFS_NODE_NAME_BMM_VALUE, O_RDONLY);

	PINFO("g_fd_value_m: %d g_fd_op_mode_m: %d",
			g_fd_value_m, g_fd_op_mode_m);

	hw->fd_pollable = 1;
	g_fd_input_ev_m = hw->fd_poll = input_open_ev_fd(g_input_dev_num_m);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_m, "place");
	err = sysfs_read_int(path, &place);
	if (!err && (HW_PLACE_UNDEFINED != place)) {
		PINFO("place of mag in driver: %d", place);
		/* assign -1 to g_place_m to disble further remapping */
		g_place_m = -1;
	} else {
		err = 0;
	}

	hw_init_m_settings(hw);

	return err;
}


static int hw_enable_m(struct sensor_hw *hw, int enable)
{
	int err = 0;

	if (enable) {
		err = hw_mag_set_opmode(HW_M_OMMODE_SLEEP);
		eusleep(HW_INFO_DELAY_WAKE_UP_M);
		hw_init_m_settings(hw);
	} else {
		err = hw_mag_set_opmode(HW_M_OMMODE_SUSPEND);
	}

#ifdef HW_M_USE_INPUT_EVENT
		err |= hw_mag_set_enable_input_ev(enable);
#endif
	return err;
}


static int hw_restore_cfg_m(struct sensor_hw *hw)
{
	int err = 0;
#ifdef HW_INFO_M_CFG_LOST_IN_SUSPEND
	struct bmm_cfg *data = (struct bmm_cfg *)hw->private_data;
	char path[128] = "";

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_m, SYSFS_NODE_NAME_BMM_REPT_XY);
	err = sysfs_write_int(path, data->rept_xy);

	sprintf(path, "%s/input%d/%s",
			SYSFS_PATH_INPUT_DEV, g_input_dev_num_m, SYSFS_NODE_NAME_BMM_REPT_Z);
	err |= sysfs_write_int(path, data->rept_z);

	eusleep(2000);
	hw_mag_set_opmode(HW_M_OMMODE_FORCED);
	/* 10ms is enough for current rept settings, a little longer make it safe */
	eusleep(18000);
#endif
	return err;
}


struct sensor_hw_m g_hw_m = {
	hw: {
			name: DEV_NAME_M,
			type: SENSOR_HW_TYPE_M,
			init:hw_init_m,
			enable:hw_enable_m,
			restore_cfg:hw_restore_cfg_m,
			set_delay:hw_mag_set_delay,
			get_data_nb:hw_mag_read_xyzdata,
			get_data:hw_mag_read_xyzdata_input_ev
		},
};

