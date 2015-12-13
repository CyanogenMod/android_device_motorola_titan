/*!
 * @section LICENSE
 *
 * (C) Copyright 2011~2015 Bosch Sensortec GmbH All Rights Reserved
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
 
 *
 * @file         channel_std.c
 * @date         "Tue Nov 25 17:33:16 2014 +0800"
 * @commit       "4841ca7"
 *
 * @brief
 *
 * @detail
 *
 */


#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define LOG_TAG_MODULE "<channel_std>"
#include "sensord.h"

int get_data_std(void *pbuf, int n)
{
	struct exchange *pmsg = (struct exchange *)pbuf;

	UNUSED_PARAM(n);

	return algo_get_proc_data_std(&pmsg->data);
}
/*!
 * @brief This function is to initialize the Step Counter
 *
 * @param channel structure
 *
 * @return 0 success, -1 fail
 */
int channel_init_std(struct channel *ch)
{

	if (NULL == ch)
	{
		return -1;
	}

	ch->cfg.no_delay     = 0;
	ch->cfg.interval_min = CFG_DELAY_STD_MIN;
	ch->cfg.interval_max = CFG_DELAY_STD_MAX;
	ch->cfg.dep_hw       = CFG_HW_DEP_STD;

	return 0;
}

int channel_std_enable(struct channel *ch, int en)
{
	if (NULL == ch)
	{
		return -1;
	}

	(void)algo_set_opmode(SENSOR_TYPE_STD, en);

	return 0;
 }


#endif

