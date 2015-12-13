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
 * @file         channel_cntl.c
 * @date         "Thu Aug 22 09:30:08 2013 +0800"
 * @commit       "adb9206"
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
#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/fs.h>
#include <sys/syscall.h>

#define LOG_TAG_MODULE "<channel_cntl>"
#include "sensord.h"



#ifdef CFG_CHECK_DISPLAY_STATE	/* deprecated */
static pthread_mutex_t g_mutex_display_state = PTHREAD_MUTEX_INITIALIZER;
#endif

extern int g_fd_fifo_cmd;

extern int g_fd_fifo_dat;

static int channel_set_state(struct channel *ch, enum CHANNEL_STATE state, int src);

extern struct channel g_list_ch[];

static int channel_hdl2idx(int handle)
{
	int i = -1;

	for (i = 0; i < channel_get_num(); i++) {
		if (handle == g_list_ch[i].handle) {
			return i;
		}
	}

	PWARN("Invalid handle: %d", handle);

	return -1;
}


struct channel *channel_get_ch(int handle)
{
	struct channel *ch = NULL;
	int i = 0;

	i = channel_hdl2idx(handle);
	if (-1 != i) {
		ch = g_list_ch + i;
	}

	return ch;
}


int channel_get_state(int handle)
{
	struct channel *ch;
	int state;

	ch = channel_get_ch(handle);
	if (NULL == ch) {
		return (int)CHANNEL_STATE_SLEEP;
	}

	pthread_mutex_lock(&ch->lock_state);
	state = (int)ch->state;
	pthread_mutex_unlock(&ch->lock_state);

	return state;
}


static int channel_set_state(struct channel *ch, enum CHANNEL_STATE state, int src)
{
	int err = 0;
	struct sensor_provider *sp = ch->sp;

	PDEBUG("function entry");
	UNUSED_PARAM(src);
	pthread_mutex_lock(&ch->lock_state);

	PINFO("channel:%s prev_state:%d state: %d ---> %d reason: %d",
			ch->name,
			ch->prev_state,
			ch->state,
			state,
			src);

	switch (state) {
	case CHANNEL_STATE_SLEEP:
		if (CHANNEL_STATE_SLEEP != ch->state) {
			sp_enable_ch(sp, ch, 0);
			if (ch->enable)
			{
				ch->enable(ch, 0);
			}
		}
		break;
	case CHANNEL_STATE_NORMAL:
	case CHANNEL_STATE_BG:
		if (CHANNEL_STATE_SLEEP == ch->state) {
		/*  initialize the first frame,
			avoiding the first frame show a very long duration value
			in sensorlist. */
			ch->ts_last_ev = get_current_timestamp();
			sp_enable_ch(sp, ch, 1);
			if (ch->enable)
			{
				ch->enable(ch, 1);
			}
		}
		break;
	default:
		PWARN("unknown state req");
		err = -EINVAL;
		break;
	}

	if (!err) {
		ch->prev_state = ch->state;
		ch->state = state;
	}

	pthread_mutex_unlock(&ch->lock_state);

	return err;
}


int channel_get_cfg(int handle, struct channel_cfg *pcfg)
{
	int err = 0;
	struct channel *ch;

	ch = channel_get_ch(handle);
	if ((NULL == ch) || (NULL == pcfg)) {
		err = -EINVAL;
		return err;
	}

	memcpy(pcfg, &ch->cfg, sizeof(ch->cfg));
	return err;
}


int channel_set_cfg(int handle, const struct channel_cfg *pcfg)
{
	int err = 0;
	struct channel *ch;

	ch = channel_get_ch(handle);
	if ((NULL == ch) || (NULL == pcfg)) {
		err = -EINVAL;
		return err;
	}

	memcpy(&ch->cfg, pcfg, sizeof(ch->cfg));
	return err;
}


int channel_on_cmd_received(int handle, int cmd, int value)
{
	int err = 0;
	struct channel *ch = NULL;
	struct sensor_provider *sp = NULL;

	ch = channel_get_ch(handle);
	if (NULL == ch) {
		return -1;
	}

	PINFO("command to %s, cmd: %d value: %d",
			ch->name, cmd, value);

	if (!ch->cfg.availability) {
		PWARN("cmd sent to a channel which is not available");
		return -1;
	}

	sp = ch->sp;

	switch (cmd) {
	case SET_SENSOR_ACTIVE:
		if (value) {
			channel_set_state(ch, CHANNEL_STATE_NORMAL, 0);
		} else {
			channel_set_state(ch, CHANNEL_STATE_SLEEP, 0);
		}

		break;
	case SET_SENSOR_DELAY:
		if (value < SAMPLE_INTERVAL_MIN) {
			PWARN("invalid interval: %d", value);
			err = -EINVAL;
			break;
		}

		if (value < ch->cfg.interval_min) {
			value = ch->cfg.interval_min;
		}

		if (value > ch->cfg.interval_max) {
			value = ch->cfg.interval_max;
		}

		/* the interval change of one channel might have influence on
		 * the whole thread, thus protected by the sp's lock */
		PINFO("new interval for sensor %s is %d, request: %d",
				ch->name, ch->interval, value);
		sp->on_ch_interval_changed(ch, value);

		sp_recalc_interval_re(sp);

		break;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	case SET_SENSOR_FLUSH:
		err = fifo_write_flush_finish_event(handle);
		break;
#endif
	default:
		PWARN("unknown command");
		err = -EINVAL;
		break;
	}

	PDEBUG("command handle result: %d", err);
	return err;
}


#ifdef CFG_CHECK_DISPLAY_STATE
static int channel_on_display_state_change(int state)
{
	int err = 0;
	int i = 0;
	struct channel *ch;

	PDEBUG("function entry");

	PDEBUG("waiting for g_mutex_display_state...");
	pthread_mutex_lock(&g_mutex_display_state);
	PDEBUG("get hold of g_mutex_display_state");

	if (state) {
		PDEBUG("display is on");
		PDEBUG("transition 1, 4");
		for (i = 0; i < channel_get_num(); i++) {
			ch = g_list_ch + i;
			if (ch->cfg.availability) {
				if (SENSOR_HANDLE_PROXIMITY != ch->handle) {
					PDEBUG("prev state of %s is: %d",
							ch->name,
							ch->prev_state);
					channel_set_state(ch,
							ch->prev_state, 1);
				}
			}
		}
	} else {
		PDEBUG("display is off");
		PDEBUG("transition 2, 6");
		for (i = 0; i < channel_get_num(); i++) {
			ch = g_list_ch + i;
			if (ch->cfg.availability) {
				if (SENSOR_HANDLE_PROXIMITY != ch->handle) {
					channel_set_state(ch,
							CHANNEL_STATE_SLEEP, 1);
				}
			}
		}
	}

	pthread_mutex_unlock(&g_mutex_display_state);
	PDEBUG("release hold of g_mutex_display_state");
	return err;
}
#endif

int channel_get_handle_list(int8_t *p_list, int32_t size)
{
	int i;
	int num, count;
	struct channel *ch;

	if (NULL == p_list)
	{
		return -EINVAL;
	}
	num = channel_get_num();
	num = num < size? num : size;

	for (i = 0, count = 0; i < num; i ++)
	{
		ch = g_list_ch + i;
		if (ch->cfg.availability)
		{
			*p_list ++ = ch->handle;
			count ++;
		}
	}

	return count;
}

static void channel_init()
{
	int err = 0;
	int i;
	struct channel *ch;

	for (i = 0; i < channel_get_num(); i++) {
		ch = g_list_ch + i;
		ch->state = CHANNEL_STATE_SLEEP;
		ch->prev_state = CHANNEL_STATE_SLEEP;

		ch->data_status = SENSOR_ACCURACY_UNRELIABLE;
		ch->interval = 200;
		ch->ts_last_ev = 0;

		ch->private_data = NULL;
		ch->client.next = NULL;
		pthread_mutex_init(&ch->lock_state, NULL);

		if (ch->cfg.availability) {
			err = ch->init(ch);

			if (!err) {
				PINFO("channel: %s initialized",
						ch->name);
			} else {
				ch->cfg.availability = UNAVAILABLE;
				PWARN("error during init of channel: %s",
						ch->name);
			}
		}

	}
}


static void channel_post_init()
{
}


void channel_subscribe2sp()
{
	int i;
	struct channel *ch;
	struct sensor_provider *sp;

	for (i = 0; i < channel_get_num(); i++) {
		ch = g_list_ch + i;
		if (!ch->cfg.availability) {
			continue;
		}

		sp = sp_scan_for_provider(ch);
		if ((NULL == sp) || (!sp->available)) {
			PWARN("ch: %s cannot find sp", ch->name);
			ch->cfg.availability = UNAVAILABLE;
		} else {
			sp_register_ch(sp, ch);
		}
	}
}


int channel_cntl_init()
{
	int err = 0;
#ifdef CFG_CHECK_DISPLAY_STATE
	display_event_handler_t display_event_handler;
#endif
	PDEBUG("function entry");

	/* sp_preinit will do initialize for bsx library and detect which kind
	of usecase(compass, ndof, imu or m4g)*/
	sp_preinit();

	/* initialize channels according with current usecase which detected by
	sp_preinit */
	channel_init();

	channel_subscribe2sp();

	sp_init();

	channel_post_init();
#ifdef CFG_CHECK_DISPLAY_STATE
	display_event_handler.on_display_state_change =
		channel_on_display_state_change;
	register_display_event_handler(&display_event_handler);
#endif

	return err;
}


void channel_cntl_destroy()
{
	int i;
	struct channel *ch;

	PDEBUG("function entry");

	for (i = 0; i < channel_get_num(); i++) {
		ch = g_list_ch + i;
		if (ch->cfg.availability) {
			ch->state = CHANNEL_STATE_SLEEP;
			if (NULL != ch->exit) {
				ch->exit();
			}
		}
	}

#ifdef CFG_CHECK_DISPLAY_STATE
	pthread_mutex_destroy(&g_mutex_display_state);
#endif
}


void channel_cntl_dump()
{
	int i;
	int err;
	int len_cmd;
	int len_dat;
	struct channel *ch;

	PINFO("channel cntl dump...");
	for (i = 0; i < channel_get_num(); i++) {
		ch = g_list_ch + i;

		/* this line is also to depress compile warning
		while debug log is disabled */
		if ((ch->handle <= SENSOR_HANDLE_START) ||
		(ch->handle >= SENSOR_HANDLE_END))
		{
			continue;
		}
		PINFO("-----------------");
		PINFO("name: %s", ch->name);
		PINFO("addr: %p", ch);
		PINFO("availability: %d", ch->cfg.availability);
		PINFO("calib_bg: %d", ch->cfg.calib_bg);
		PINFO("interval_min: %d", ch->cfg.interval_min);
		PINFO("interval_max: %d", ch->cfg.interval_max);
		PINFO("no_delay: %d", ch->cfg.no_delay);
		PINFO("bypass_proc: %d", ch->cfg.bypass_proc);
		PINFO("dep_hw: 0x%x", ch->cfg.dep_hw);
		PINFO("prev_state: %d", ch->prev_state);
		PINFO("state: %d", ch->state);
		PINFO("data_status: %d", ch->data_status);
		PINFO("interval: %d", ch->interval);
		PINFO("ts_last_ev: %d", ch->ts_last_ev);
		PINFO("private_data: %p", ch->private_data);
	}


	len_cmd = 0;
	err = ioctl(g_fd_fifo_cmd, FIONREAD, &len_cmd);
	if (err)
	{
		PERR("err: %d, len_cmd: %d", err, len_cmd);
	}
	len_dat = 0;
	err = ioctl(g_fd_fifo_dat, FIONREAD, &len_dat);
	if (err)
	{
		PERR("err: %d, len_dat: %d", err, len_dat);
	}
}
