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
 * @file         channel_grv.c
 * @date         "Mon Oct 27 16:36:05 2014 +0800"
 * @commit       "566ce5a"
 *
 * @brief
 *
 * @detail
 *
 */


#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
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

#define LOG_TAG_MODULE "<channel_grv>"
#include "sensord.h"

int get_data_grv(void *pbuf, int n)
{
	struct exchange *pmsg = (struct exchange *)pbuf;

	UNUSED_PARAM(n);
	algo_get_proc_data_grv(&pmsg->data);

	return 1;
}
/*!
 * @brief This function is to initialize the Game Rotation Vector Sensor
 *
 * @param channel structure
 *
 * @return 0 success, -1 fail
 */
int channel_init_grv(struct channel *ch)
{
	int usecase_id;

	if (NULL == ch)
	{
		return -1;
	}

	usecase_id = algo_get_current_usecase();

	if (usecase_id == -1)
	{
		PERR("unknown usecase id!");
		return -1;
	}
	/*if gyro sensor isn't enalbe then grv won't be available*/
	if (usecase_id == ALGO_FUSION_MODE_COMPASS || usecase_id == ALGO_FUSION_MODE_M4G)
	{
		ch->cfg.availability = UNAVAILABLE;
	}
	else
	{
		ch->cfg.no_delay     = 0;
		ch->cfg.interval_min = CFG_DELAY_GRV_MIN;
		ch->cfg.interval_max = CFG_DELAY_GRV_MAX;
		ch->cfg.dep_hw       = CFG_HW_DEP_GRV;
	}
	return 0;
}
#endif
