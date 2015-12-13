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
 * @file         util_time.c
 * @date         "Fri Aug 2 13:43:20 2013 +0800"
 * @commit       "3911fa0"
 *
 * @brief
 *
 * @detail
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>

#define LOG_TAG_MODULE "<util_time>"
#include "sensord.h"
struct clock_provider *g_cp_dft = NULL;

static struct timeval g_start_time;

/* it takes about 292.47 years for the int64_t to overflow */
static int cp_init_hr(struct clock_provider *cp)
{
	int err = 0;
	struct timespec ts;

	err = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (!err) {
		cp->start.tv_sec = ts.tv_sec;
		cp->start.tv_nsec = ts.tv_nsec;

		cp->start_ref.tv_sec = ts.tv_sec;
		cp->start_ref.tv_nsec = ts.tv_nsec;
	} else {
		PERR("error clock_gettime()");
	}

	err = clock_getres(CLOCK_MONOTONIC, &ts);
	if (!err) {
		cp->res.tv_sec = ts.tv_sec;
		cp->res.tv_nsec = ts.tv_nsec;
	} else {
		PERR("error get time resolution");
	}

	return err;
}


static void cp_get_tick_ns_hr(struct clock_provider *cp, time_tick_ns_t *t)
{
	int err = 0;
	struct timespec ts;

	UNUSED_PARAM(cp);

	err = clock_gettime(CLOCK_MONOTONIC, &ts);
	if (!err) {
		*t = ts.tv_sec * TIME_SCALE_S2NS + ts.tv_nsec;
	} else {
		PWARN("unable to get clock time");
		*t = 0;
	}
}


struct clock_provider g_cp_hr = {
	name: "CP_HR",
	available:1,
	init:cp_init_hr,
	get_tick_ns:cp_get_tick_ns_hr
};


int cp_init_rt(struct clock_provider *cp)
{
	int err = 0;
	struct timeval tv;

	err = gettimeofday(&tv, NULL);

	if (!err) {
		cp->start.tv_sec = tv.tv_sec;
		cp->start.tv_nsec = tv.tv_usec * 1000L;

		cp->start_ref.tv_sec = tv.tv_sec;
		cp->start_ref.tv_nsec = tv.tv_usec * 1000L;
	} else {
		PERR("error gettimeofday()");
	}

	cp->res.tv_sec = 0;
	cp->res.tv_nsec = 1000;

	return err;
}


void cp_get_tick_ns_rt(struct clock_provider *cp, time_tick_ns_t *t)
{
	struct timeval now;
	gettimeofday(&now, NULL);

	if ( ( (now.tv_usec < cp->start_ref.tv_nsec / 1000L)
				&& (now.tv_sec == cp->start_ref.tv_sec) )
			|| (now.tv_sec < cp->start_ref.tv_sec) ) {
		fprintf(stderr,
			"time tick overflow: %ld %ld %ld %ld",
			now.tv_sec,
			now.tv_usec,
			cp->start_ref.tv_sec,
			cp->start_ref.tv_nsec);

		cp->start_ref.tv_sec = now.tv_sec;
		cp->start_ref.tv_nsec = now.tv_usec * 1000L;
	}

	*t = (now.tv_sec - cp->start_ref.tv_sec) * TIME_SCALE_S2NS +
		(now.tv_usec * TIME_SCALE_US2NS - cp->start_ref.tv_nsec);

}


struct clock_provider g_cp_rt = {
	name: "CP_RT",
	available:1,
	init:cp_init_rt,
	get_tick_ns:cp_get_tick_ns_rt
};


void time_init()
{
	char utc_time[32] = "";

	gettimeofday(&g_start_time, NULL);
	g_cp_dft = &g_cp_hr;
	g_cp_dft->init(g_cp_dft);

	get_curr_time_str(utc_time, sizeof(utc_time));
	PINFO("cp init tick start: %ld:%ld, tick resolution:\
			%ld:%ld utc_time: %s",
			g_cp_dft->start.tv_sec,
			g_cp_dft->start.tv_nsec,
			g_cp_dft->res.tv_sec,
			g_cp_dft->res.tv_nsec,
			utc_time);
}


unsigned int get_time_tick()
{
	unsigned int ret = 0;
	struct timeval now;
	gettimeofday(&now, NULL);

	if ( ( (now.tv_usec < g_start_time.tv_usec)
				&& (now.tv_sec == g_start_time.tv_sec) )
			|| (now.tv_sec < g_start_time.tv_sec) ) {
#ifndef CFG_LOG_TO_LOGCAT
		fprintf(stderr,
			"time tick overflow: %ld %ld %ld %ld",
			now.tv_sec,
			now.tv_usec,
			g_start_time.tv_sec,
			g_start_time.tv_usec);
#endif

		g_start_time.tv_sec = now.tv_sec;
		g_start_time.tv_usec = now.tv_usec;
	}

	ret = (now.tv_sec - g_start_time.tv_sec) * 1000000 +
		(now.tv_usec  - g_start_time.tv_usec);

	return ret;
}

uint32_t get_current_timestamp(void)
{
	uint32_t ts;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	ts = tv.tv_sec * 1000000 + tv.tv_usec;

	return ts;
}


void get_curr_time_str(char *buf_str, int len)
{
	time_t now;
	struct tm *tm_now;
	struct tm tm_now_tmp;
	char buf[32] = "";

	time(&now);
	tm_now = localtime_r(&now, &tm_now_tmp);
	sprintf(buf, "%04d-%02d-%02d_%02d-%02d-%02d",
			tm_now->tm_year + 1900,
			tm_now->tm_mon + 1,
			tm_now->tm_mday,
			tm_now->tm_hour,
			tm_now->tm_min,
			tm_now->tm_sec);

	if (len > 1) {
		buf_str[len - 1] = '\0';

		memcpy(buf_str, buf, len - 1);
	}
}


void eusleep(uint32_t us)
{
	uint32_t tmp = 0;

	if (us >= TIME_SCALE_S2US) {
		tmp = (us - TIME_SCALE_S2US) / TIME_SCALE_S2US + 1;
		sleep(tmp);
		us -= tmp * TIME_SCALE_S2US;

		if (0 == us) {
			return;
		}
	}

	adv_nsleep_short(us * 1000, 1);
}

/*!
 * @brief
 * a handy function for sleeping for a short time (less than 1 sec)
 */
void adv_nsleep_short(long ns, int restart)
{
	int err;

	struct timespec req;
	struct timespec rem;

	req.tv_sec = 0;
	req.tv_nsec = ns;
	while (1) {
		err = nanosleep(&req, &rem);
		if (!err) {
			break;
		} else {
			PERR("error during sleep");
			if ((EINTR == errno) && restart) {
				req.tv_nsec = rem.tv_nsec;
				continue;
			} else {
				break;
			}
		}
	}
}

