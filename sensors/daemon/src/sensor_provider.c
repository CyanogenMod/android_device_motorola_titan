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
 * @file         sensor_provider.c
 * @date         "Tue Aug 27 15:51:32 2013 +0800"
 * @commit       "24cb7fe"
 *
 * @brief
 *
 * @detail
 *
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#include <linux/fs.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/syscall.h>


#define LOG_TAG_MODULE "<sensor_provider>"
#include "sensord.h"
static pthread_mutex_t g_mutex_dat_fifo;
extern int g_fd_fifo_dat;

extern struct algo g_sp_algo_fusion;
extern struct sensor_provider g_sp_pressure;

static const struct sensor_provider *g_list_sp[] = {
	&g_sp_algo_fusion.sp,
	NULL	/* the terminator */
};

#ifdef __DEBUG_TIMING_ACCURACY__
static unsigned int total_sleep_count = 0, over_sleep_count = 0;
static unsigned int peek_sleep_duration = 0;
#endif


void sp_preinit()
{
	int i = 0;
	struct run_entity *re;
	struct sensor_provider *sp;
	int err = 0;

	i = 0;
	while (NULL != (sp = (struct sensor_provider *)g_list_sp[i++])) {
		sp->available = 0;
		sp->client_num = 0;
		sp->ref = 0;

		sp->curr_hw_dep = 0;
		sp->clients = NULL;
		sp->buf_out = NULL;

		pthread_mutex_init(&sp->lock_ref, NULL);

		re = &sp->re;
		re->ptid = -1;
		re->tid = -1;

		re->started = 0;
		re->sleeping = 0;
		re->interval = 1000;

		pthread_cond_init(&re->cond, NULL);

		err = sp->init(sp);
		if (err) {
			PWARN("error init of sensor provider: %s", sp->name);
			continue;
		}

		sp->available = 1;
	}
}


static void sp_sync_re()
{
	int i = 0;
	struct sensor_provider *sp;

	i = 0;
	while (NULL != (sp = (struct sensor_provider *)g_list_sp[i++])) {
		if (!sp->available) {
			continue;
		}

		while (!sp->re.started) {
			eusleep(1000);
		}
	}

	PINFO("all available threads are ready now");
}


void sp_init()
{
	int err = 0;
	int i = 0;
	int tmp;
	struct exchange *data = NULL;
	struct sensor_provider *sp;
	pthread_attr_t thr_attr;
	size_t stk_size;

	pthread_mutex_init(&g_mutex_dat_fifo, NULL);

	pthread_attr_init(&thr_attr);
	pthread_attr_getstacksize(&thr_attr, &stk_size);
	PINFO("stack size: %d", stk_size);

	i = 0;
	while (NULL != (sp = (struct sensor_provider *)g_list_sp[i++])) {
		if (0 == sp->client_num) {
			sp->available = 0;
			continue;
		}

		data = (struct exchange *)sp->buf_out;
		if (NULL == data) {
			data = (struct exchange *)calloc(sp->client_num,
					sizeof(struct exchange));
			if (NULL == data) {
				PERR("no mem for %s", sp->name);
				/* wait for some time for system to release some resource for us */
				eusleep(100000);
				continue;
			}

			sp->buf_out = data;
		}

		for (tmp = 0; tmp < sp->client_num; tmp++) {
			data[tmp].magic = CHANNEL_PKT_MAGIC_DAT;
			data[tmp].data.version = sizeof(data[0]);
		}

		err = pthread_create(&sp->re.ptid,
				NULL,
				sp->re.func,
				(void*)sp);

		if (!err) {
			PINFO("thread created for provider: %s",
					sp->name);
		} else {
			PERR("error creating thread for provider: %s",
					sp->name);
		}
	}

	sp_sync_re();
}


struct sensor_provider *sp_scan_for_provider(struct channel *ch)
{
	struct sensor_provider *sp = NULL;
	int i;

	i = 0;
	while (NULL != (sp = (struct sensor_provider *)g_list_sp[i++])) {
		if ((1 << ch->type) & sp->sensors) {
			if (!strcmp(ch->cfg.sp_name, sp->name)) {
				break;
			}
		}
	}

	return sp;
}


void sp_register_ch(struct sensor_provider *sp, struct channel *ch)
{
	sp->client_num++;
	ch->sp = sp;
}


static int sp_re_check_dep_hw(struct sensor_provider *sp)
{
	int err = 0;
	hw_dep_set_t new_dep_hw = 0;
	hw_dep_set_t changed = 0;
	int i;
	int enable;

	PDEBUG("check dependency of %s",
			sp->name);

	sp->get_curr_hw_dep(&new_dep_hw);
	changed = new_dep_hw ^ sp->curr_hw_dep;

	for (i = 0; i < (int)SENSOR_HW_TYPE_MAX; i++) {
		if ((changed >> i) & 0x01) {
			enable = ((new_dep_hw >> i) & 0x01);
			if (enable) {
				err = hw_ref_up(i);
			} else {
				err = hw_ref_down(i);
			}

			if (err) {
				PWARN("<hw_dep> %s@%d %d -> %d err: %d",
						sp->name,
						i,
						(sp->curr_hw_dep >> i) & 0x01,
						(new_dep_hw >> i) & 0x01,
						err);

				new_dep_hw ^= (1 << i);
			}
		}
	}

	sp->curr_hw_dep = new_dep_hw;

	return err;
}


void sp_recalc_interval_re(struct sensor_provider *sp)
{
	struct list_node *cur;
	struct channel *ch;
	struct run_entity *re;
	int val = 1000;

	re = &sp->re;

	if (NULL != sp->get_hint_proc_interval) {
		re->interval = sp->get_hint_proc_interval();
	} else {
		cur = sp->clients;
		while (NULL != cur) {
			ch = CONTAINER_OF(cur, struct channel, client);
			if (CHANNEL_STATE_SLEEP != ch->state) {
				val = get_max_comm_div(val, ch->interval);
			}

			cur = cur->next;
		}

		re->interval = val;
	}

	if (re->interval < INTV_PROC_MIN) {
		re->interval = INTV_PROC_MIN;
	}

	PINFO("new interval for sp is: %d", re->interval);
}


void sp_enable_ch(struct sensor_provider *sp, struct channel *ch, int enable)
{
	struct list_node *head;
	int err;

	head = sp->clients;

	if (enable) {
		if (NULL == list_find_node(head, &ch->client)) {
			list_add_head(head, &ch->client);
		}

		sp->clients = &ch->client;
	} else {
		head = list_del_node(head, &ch->client);
		sp->clients = head;
	}


	/* notice provider that some channel will be switched on/off */
	/* provider should know that the h/w might not be switched on/off yet */
	err = sp->on_ch_enabled(ch, enable);
	if (err) {
		PWARN("on_ch_enabled: %d error for %s", enable, ch->name);
	}

	sp_re_check_dep_hw(sp);

	if (NULL != sp->on_hw_dep_checked) {
		sp->on_hw_dep_checked(&sp->curr_hw_dep);
	}

	sp_recalc_interval_re(sp);

	if (ch->cfg.bypass_proc) {
		/* no need to update the ref,
		 * thus return */
		PDEBUG("sp act as hw manager only for: %s",  ch->name);
		return;
	}

	if (enable) {
		sp->ref += 1;
		if (1 == sp->ref) {
			/* CHECK: lock_cond is already locked */
			pthread_cond_signal(&sp->re.cond);
		}
	} else {
		if (sp->ref > 0) {
			sp->ref -= 1;
		}
	}
}


static int sp_report_data(void *buf, int n)
{
	if (n > 0) {
		fifo_write(buf, n * sizeof(struct exchange));
	}

	return 0;
}

void sp_sleep(unsigned int delay)
{
#ifdef __DEBUG_TIMING_ACCURACY__
	struct timeval time_now;
	struct timeval time_before;
	unsigned int diff_duration = 0;
	unsigned int real_duration = 0;
	/* tolerance duration is expected less than 1 ms */
	unsigned int tolerance_duration = 1000;

	gettimeofday (&time_before, NULL);
#endif

/*	according with test nano sleep has no advantage!
	following functions have been tested:
	nano_sleep_ex which is built on clock_nanosleep
	eusleep which is built on nanosleep

	Notice: linux is not RTOS system, sleep function does not
	guarentee the timing accuracy, according with my test,
	normally there have 0.7% of rate that the delay will be longer
	than orignal delay. And the average over delay is about 5ms, with
	peek over delay 10ms according with my measurement.
*/
/*	nano_sleep_ex(delay); */
	eusleep(delay);

#ifdef __DEBUG_TIMING_ACCURACY__
	gettimeofday (&time_now, NULL);
	real_duration = (time_now.tv_sec - time_before.tv_sec) * 1000000
			+ (time_now.tv_usec - time_before.tv_usec);
	diff_duration = real_duration - delay;

	total_sleep_count ++;
	if (diff_duration > tolerance_duration)
	{
		over_sleep_count ++;
		if (peek_sleep_duration < diff_duration)
		{
			peek_sleep_duration = diff_duration;
		}
	}
#endif
}

void* re_proc(void* pparam)
{
	int    sleep_time = 0;
	struct run_entity *re = NULL;
	struct sensor_provider *sp = NULL;
	struct channel *ch = NULL;
	struct list_node *cur = NULL;
	struct exchange *data = NULL;
	unsigned int time_start = 0;
	unsigned int time_now = 0;
	unsigned int elapse = 0;
	int  num = 0;
	int  ret = 0;
#ifdef __SCHEDULING_TIMESTAMP_CALIBRATED__
	unsigned int cali_timestamp = 0;
#endif

	sp = (struct sensor_provider *)pparam;
	re = &sp->re;
	re->tid = (int)syscall(__NR_gettid);
	re->started = 1;

	data = (struct exchange *)sp->buf_out;
	while (1) {

		if (0 == sp->ref) {
#ifdef __DEBUG_TIMING_ACCURACY__
			print_timing_accuracy();
			PWARN ("peek sleep duration: %d, over sleep account: %d"
				"total sleep count: %d)",
				peek_sleep_duration,
				over_sleep_count, total_sleep_count);
#endif
#ifdef __SCHEDULING_TIMESTAMP_CALIBRATED__
			cali_timestamp = 0;
#endif
			/* wait for the sensor to be restarted */
			pthread_mutex_lock(&sp->lock_ref);
			ret = pthread_cond_wait(&re->cond,
						&sp->lock_ref);
			pthread_mutex_unlock(&sp->lock_ref);
			if (ret) {
				PERR("error on waiting...%d", ret);
				continue;
			}
		}

		/* start to proc sensor signal */
		time_start = get_current_timestamp();
		if (NULL != sp->proc_data) {
#ifdef __SCHEDULING_TIMESTAMP_CALIBRATED__
			sp->proc_data(cali_timestamp);
			cali_timestamp += re->interval * 1000;
#else
			sp->proc_data(time_start);
#endif
		}

		cur = sp->clients;
		num = 0;
		while (NULL != cur) {
			ch = CONTAINER_OF(cur, struct channel, client);

			if ((CHANNEL_STATE_NORMAL == ch->state)
			    && !ch->cfg.bypass_proc) {

				/* elapse value is at least one frame, hence no possibility of
				less than 5ms */
				elapse = time_start - ch->ts_last_ev;

				/* no delay is for event type sensor, which need to report
				immediately after data update. */
				if (ch->cfg.no_delay ||
						(elapse >= (uint32_t)ch->interval * 1000)) {
					ret = ch->get_data(data + num, sp->client_num - num);
					if (ret > 0) {
						struct timespec tsnsec;
						clock_gettime(CLOCK_BOOTTIME, &tsnsec);
						data[num].data.sensor = ch->handle;
						data[num].data.type = ch->type;
						data[num].ts = (int64_t)((tsnsec.tv_sec * TIME_SCALE_S2NS) + tsnsec.tv_nsec);
						num ++;
					}
					ch->ts_last_ev = time_start;
				}
			}

			cur = cur->next;
		}

		sp_report_data(data, num);

		/* caculate sleep duration */
		time_now = get_current_timestamp();
		elapse = time_now - time_start;
		if (elapse >= re->interval * 1000)
		{
			continue;
		}

		sleep_time = re->interval * 1000 - elapse;
		if (sleep_time > 200)
		{
			sp_sleep(sleep_time);
		}

	}

	re->started = -1;
	PINFO("thread %s terminated", sp->name);
	pthread_exit((void *)&(re->tid));
	return (pparam);
}


void sp_dump()
{
	struct sensor_provider *sp;
	struct run_entity *re;
	int i;

	PINFO("sensor provider dump...");
	i = 0;
	while (NULL != (sp = (struct sensor_provider *)g_list_sp[i++])) {
		re = &sp->re;
		if (re == NULL)
			continue;
		PINFO("-----------------");
		PINFO("name: %s", sp->name);
		PINFO("addr: %p", sp);
		PINFO("ptid: %ld", re->ptid);
		PINFO("tid: %d", re->tid);
		PINFO("started: %d", re->started);
		PINFO("client_num: %d", sp->client_num);
		PINFO("ref: %d", sp->ref);
		PINFO("interval: %d", re->interval);
	}
}
