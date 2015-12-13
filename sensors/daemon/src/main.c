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
 * @file         main.c
 * @date         "Tue Jun 18 13:50:34 2013 +0800"
 * @commit       "42bae15"
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

#define LOG_TAG_MODULE "<main>"
#include "sensord.h"

#ifdef __DEBUG_SIGALT_STACK_SUPPORT__
extern void early_ev_init();
#endif
extern void early_trace_init();
extern void misc_init();
extern void dump_ver();

void singletonize()
{
	int fd;
	int fd_proc = -1;
	int pid_another = -1;
	char path[64] = "";
	char state = 0x00;

	char path_exe_link[128] = "";
	char path_exe_link_another[128] = "";

	char *buf;
	int err = 0;

	fd = open(PROCESS_LANDMARK, O_RDONLY);
	if (-1 != fd) {
		txt_file_read_int(fd, &pid_another, 1);
		close(fd);

		sprintf(path, "/proc/%d/stat", pid_another);
		fd_proc = open(path, O_RDONLY);
		if (-1 != fd_proc) {
			/* borrow buffer temporarily */
			buf = path_exe_link;
			err = read(fd_proc, buf, sizeof(path_exe_link) - 1);
			if (err > 0) {
				sscanf(buf, "%11d %63s %c",
						&pid_another,
						path, &state);
			}

			PINFO("state of another process <%d> %s : %c",
					pid_another, path, state);
			close(fd_proc);

			if (getpid() == pid_another) {
				return;
			}

			sprintf(path, "/proc/%d/exe", pid_another);
			memset(path_exe_link_another, 0, sizeof(path_exe_link_another));
			err = readlink(path, path_exe_link_another,
					sizeof(path_exe_link_another) - 1);
			if (err <= 0) {
				PERR("error read link: %s", path);
				return;
			}
			path_exe_link_another[err] = '\0';

			PINFO("path of previous exe: %s", path_exe_link_another);
			sprintf(path, "/proc/%d/exe", getpid());
			memset(path_exe_link, 0, sizeof(path_exe_link));
			err = readlink(path, path_exe_link,
					sizeof(path_exe_link) - 1);
			if (err <= 0) {
				PERR("error read link: %s", path);
				return;
			}
			path_exe_link[err] = '\0';
			PINFO("path of curr exe: %s", path_exe_link);
			if (0 != strncmp(path_exe_link, path_exe_link_another,
						sizeof(path_exe_link_another) - 1)) {
				PINFO("not same exe");
				return;
			}

			if ('Z' != state) {
				if (NULL != strchr("RSDTW", state)) {
					/* usually, this is caused
					 * by human manually,
					 * so output the error to the stderr directly
					 */
					printf("another instance <%d> already running, quit!",
							pid_another);
					exit(0);
				}
			}
		}
	}
}


void parse_cmd_line(int argc, char* argv[])
{
	int i;

	PINFO("argc: %d", argc);

	for (i = 0; i < argc; i++) {
		PINFO("argv[%d]: %s", i, argv[i]);
		if (0 == strcmp("-v", argv[i])) {
			dump_ver();
			exit(0);
		}
	}
}


extern void early_ev_init();
extern void early_trace_init();
extern void misc_init();

int main(int argc, char *argv[])
{
	int err = 0;

	early_trace_init();

#ifdef __DEBUG_SIGALT_STACK_SUPPORT__
	/* handle exceptions as early as possible */
	early_ev_init();
#endif

	/* this might exit the whole process if needed */
	parse_cmd_line(argc, argv);

	/* this might exit the whole process if needed */
	singletonize();

	misc_init();

	sensor_cfg_init();

	err = hw_cntl_init();
	if (err) {
		PWARN("fail to init hw");
		goto exit_clean;
	}

	err = channel_cntl_init();
	if (err) {
		PWARN("fail to init channels");
		goto exit_clean;
	}

	/* event handler register */
	err = ev_init();
	if (err) {
		PWARN("fail to init channels");
		goto exit_clean;
	}

	ev_loop_check_cmd();
exit_clean:
	return err;
}
