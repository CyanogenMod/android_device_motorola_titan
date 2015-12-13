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
 * @file         misc.c
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
#include <linux/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "version.h"

#define LOG_TAG_MODULE "<init>"
#include "sensord.h"

extern void trace_init();

void dump_ver()
{
	ts_version version;

	(void)bsx_get_version(&version);

	PINFO("algorithm version: %d.%d.%d.%d\n",
			version.major, version.minor,
			version.majorbugFix, version.minorbugFix);
	PINFO("daemon version: %s\n", get_daemon_version());
	PINFO("build date: %s\n", get_build_date());

}

void write_version(int fd)
{
	char buf[128];
	ts_version version;

	sprintf(buf, "sensord : %s\n", get_daemon_version());
	write(fd, buf, strlen(buf));
	sprintf(buf, "build date: %s\n", get_build_date());
	write(fd, buf, strlen(buf));
	(void)bsx_get_version(&version);
	sprintf(buf, "algo version:%d.%d.%d.%d\n", version.major,
				version.minor,
				version.majorbugFix,
				version.minorbugFix);
	write(fd, buf, strlen(buf));
}

/*
 * @Description: this function will make a landmark
 * so the singletonize() function can work
 */
static void mark_land_owner()
{
	int fd;

	fd = open(PROCESS_LANDMARK, O_RDWR | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR |
			S_IRGRP | S_IWGRP |
			S_IROTH | S_IWOTH);
	if (-1 != fd) {
		fchmod(fd, 0644);
		txt_file_write_int(fd, getpid(), 1, 1);
		close(fd);
	}
}


void storage_init()
{
	char *path = NULL;
	int err = 0;
	struct stat st;

	path = PATH_DIR_SENSOR_STORAGE;

	err = access(path, F_OK);
	if (!err) {
		st.st_mode = ~S_IFDIR;
		err = stat(path, &st);
		if (!S_ISDIR(st.st_mode)) {
			PWARN("a file of the same name exists");
			/* mark error explicitly for creating the dir */
			err = -ENOTDIR;
		}
	}

	if (err) {
		PINFO("create path of storage");
		unlink(path);
		err = mkdir(path, 0777);
		if (err) {
			PERR("error creating storage dir");
		}
	}

	err = chmod(path, 0777);
	if (err) {
		PERR("error chmod on %s", path);
	}
}

int misc_init()
{
	int err = 0;

	storage_init();
	mark_land_owner();
	trace_init();

	PINFO("<<------sensord <%d> started------>>", getpid());

	dump_ver();
	time_init();

	return err;
}
