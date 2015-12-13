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
 * @file         event_handler.c
 * @date         "Tue July 18 13:50:34 2014 +0800"
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
#include <poll.h>
#include <signal.h>

#define LOG_TAG_MODULE "<event_handler>"
#include "sensord.h"

#ifdef CFG_SENSORD_SIMU
#define SYS_DISPLAY_STATE_WAKE  (PATH_DIR_SENSOR_STORAGE "/wait_for_fb_wake")
#define SYS_DISPLAY_STATE_SLEEP (PATH_DIR_SENSOR_STORAGE "/wait_for_fb_sleep")
#else
#define SYS_DISPLAY_STATE_WAKE "/sys/power/wait_for_fb_wake"
#define SYS_DISPLAY_STATE_SLEEP "/sys/power/wait_for_fb_sleep"
#endif

#define FAULT_LOG_FILE (PATH_DIR_SENSOR_STORAGE  "/sensord.err")

#define CHECK_FOR_DISPLAY_STATE_INTERVAL (1000000)

#define PFAULT(fd, fmt, args...) \
	do {\
		if (-1 != fd) {\
			trace_log(fd, fmt "\n", ##args);\
		}\
	} while (0);


pthread_t g_tid_check_cmd;
#ifdef CFG_CHECK_DISPLAY_STATE
pthread_t g_tid_check_display_state;


display_event_handler_t *g_display_event_handler = NULL;
#endif

int g_fd_fifo_cmd = -1;
int g_fd_fifo_dat = -1;
static pthread_mutex_t mutex_dat_fifo;

extern int g_fd_trace;
#ifdef __DEBUG_SIGALT_STACK_SUPPORT__
static int g_fd_fault = -1;
#endif

extern int channel_on_cmd_received(int handle, int cmd, int value);

/* before return read n bytes data completely*/
static size_t readn(int fd, void *buf, size_t nbyte)
{
	unsigned char *ptr;
	int nleft;
	int nread;

	ptr = (unsigned char*)buf;
	nleft = nbyte;
	while (nleft > 0) {
		nread = read(fd, ptr, nleft);
		if (nread < 0) {
			PERR("error reading file");
			return -1;
		}

		nleft -= nread;
		ptr += nread;
	}
	return nbyte - nleft;
}

int fifo_write(void *data, int size)
{
	int err;

	if (NULL == data)
	{
		return -EINVAL;
	}
	/* lock for single fifo write */
	pthread_mutex_lock(&mutex_dat_fifo);
	err = write(g_fd_fifo_dat, data, size);
	pthread_mutex_unlock(&mutex_dat_fifo);

	/* if write failed */
	if (err < 0)
	{
		err = -EIO;
	}
	else
	{
		err = 0;
	}
	return err;
}

int fifo_read(void *data, int size)
{
	if (NULL == data)
	{
		return -EINVAL;
	}
	return readn(g_fd_fifo_cmd, data, size);
}

/*!
 * @brief This function send sensor handler list to hal
 */
int handler_get_handler_list(void)
{
	int ret;
	int8_t  list[20];

	ret = channel_get_handle_list(list, 20);
	if (ret < 0)
	{
		PERR("failed to get handler list\n");
	}
	PINFO("get %d sensors\n", ret);
	ret = fifo_write(list, ret);
	return ret;
}

void check_cmd_event()
{
	struct exchange cmd;
	int ret;

	ret = fifo_read(&cmd, sizeof(cmd));

	if (sizeof(cmd) != ret) {
		PWARN("invalid cmd packet, %d %d",
						sizeof(cmd), ret);
		return;
	}

	if (CHANNEL_PKT_MAGIC_CMD == cmd.magic)
	{
		PDEBUG("command got: %d, %d, %d",
				cmd.command.code,
				cmd.command.cmd,
				cmd.command.value);

		channel_on_cmd_received(cmd.command.code,
				cmd.command.cmd,
				cmd.command.value);
	}
	else if (CHANNEL_PKT_MAGIC_LIST == cmd.magic)
	{
		ret = handler_get_handler_list();
		if (ret)
		{
			PWARN("failed at getting handler list:%d\n", ret);
		}
	}
	else {
		PWARN("discard invalid cmd packet from stream: %c",
				cmd.magic);
	}

}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
int fifo_write_flush_finish_event(int handle)
{
	int ret;
	struct exchange event;
	event.magic = CHANNEL_PKT_MAGIC_DAT;
	event.data.version = sizeof(struct exchange);
	event.data.type = SENSOR_TYPE_META_DATA;
	event.data.sensor= handle;
	return fifo_write((void *)&event, sizeof(struct exchange));
}
#endif

#ifdef CFG_CHECK_DISPLAY_STATE
static void notify_display_listeners(int state)
{
	int err;

	if (state == g_display_state) {
		PINFO("state: %d same as before", state);
	}

	PINFO("new display state: %d", state);
	g_display_state = state;

	if (NULL != g_display_event_handler) {
		if (NULL != g_display_event_handler->on_display_state_change) {
			err = g_display_event_handler->on_display_state_change(state);
		}
	}

}
#endif

void ev_loop_check_cmd()
{
	while (1) {
		check_cmd_event();
	}
}

#ifdef CFG_CHECK_DISPLAY_STATE
static void* ev_loop_check_display_state(void *pparam)
{
	int fd_s;
	int fd_w;
	char buf[64];
	int err;
	int mode;

#ifdef CFG_SENSORD_SIMU
	mode = O_RDWR;
#else
	mode = O_RDONLY;
#endif

	fd_w = open(SYS_DISPLAY_STATE_WAKE, mode);
	if (0 > fd_w) {
		PERR("error openning file: %s", SYS_DISPLAY_STATE_WAKE);
		return pparam;
	}

	fd_s = open(SYS_DISPLAY_STATE_SLEEP, mode);
	if (0 > fd_s) {
		PERR("error openning file: %s", SYS_DISPLAY_STATE_SLEEP);
		close(fd_w);
		return pparam;
	}

	PINFO("fd_w: %d fd_s: %d", fd_w, fd_s);

	while (1) {
		lseek(fd_w, 0, SEEK_SET);
		err = read(fd_w, buf, sizeof(buf));
		PINFO("read of wake file complete");
		if (err > 0) {
			notify_display_listeners(1);
		} else {
			PINFO("error reading from: %s", SYS_DISPLAY_STATE_WAKE);
		}

		lseek(fd_s, 0, SEEK_SET);
		err = read(fd_s, buf, sizeof(buf));
		PINFO("read of sleep file complete");
		if (err > 0) {
			notify_display_listeners(0);
		} else {
			PINFO("error reading from: %s", SYS_DISPLAY_STATE_SLEEP);
		}
	}

	if (0 <= fd_w) {
		close(fd_w);
	}

	if (0 <= fd_s) {
		close(fd_s);
	}

	return pparam;
}
#endif

#ifdef __DEBUG_SIGALT_STACK_SUPPORT__
static void ev_dump()
{
}

static void handler_sig_user1(int signum)
{
	/* silly, just used to fool compiler to supress warning,
	 * remove this line when needed */
	signum = signum + 0;

	dump_ver();
	sp_dump();
	channel_cntl_dump();
	hw_cntl_dump();
	ev_dump();

	sync();
}


static void handler_sig_user2(int signum)
{
	/* silly, just used to fool compiler to supress warning,
	 * remove this line when needed */
	signum = signum + 0;
}


void sighandler_generic(int signo, siginfo_t *info, void *ctx)
{
#ifdef CFG_HANDLE_FAULT_SIG
	ucontext_generic_t *uctx = (ucontext_generic_t *)ctx;
	int i;
	int n;
	long *tmp = NULL;
	char utc_time[32] = "";
	int fd_fault = g_fd_fault;
#endif

#ifdef CFG_ARCH_ARM
	long *sp = NULL;
#endif

	PINFO("pid: %d", getpid());
	switch (signo) {
	case SIGUSR1:
		handler_sig_user1(signo);
		break;
	case SIGUSR2:
		handler_sig_user2(signo);
		break;
	case SIGCONT:
		break;
	default:
#ifdef CFG_HANDLE_FAULT_SIG
		PFAULT(fd_fault, "signo: %d", info->si_signo);
		PFAULT(fd_fault, "errno: %d", info->si_errno);
		PFAULT(fd_fault, "code: %d", info->si_code);
		PFAULT(fd_fault, "addr: %p", info->si_addr);

		n = sizeof(uctx[0]) / sizeof(long);

		PFAULT(fd_fault, "total count: %d", n);
		PFAULT(fd_fault, "%d %d %d",
				sizeof(uctx->uc_stack) / sizeof(long),
				sizeof(uctx->uc_mcontext) / sizeof(long),
				sizeof(uctx->uc_sigmask) / sizeof(long));

		PFAULT(fd_fault, "%d %d %d",
				(int)&(uctx->uc_stack) - (int)uctx,
				(int)&(uctx->uc_mcontext) - (int)uctx,
				(int)&(uctx->uc_sigmask) - (int)uctx);

		tmp = (long *)uctx;
		for (i = 0; i < n; i++) {
			PFAULT(fd_fault, "%d: %08lx", i, *tmp);
			tmp++;
		}

#ifdef CFG_ARCH_ARM
		sp = (long *)uctx->uc_mcontext.arm_sp;
		sp -= 32;
		for (i = 0; i < 64; i++) {
			PFAULT(fd_fault, "%08lx: %08lx",
					(unsigned long)(sp + i), *(sp + i));
		}

#endif
		get_curr_time_str(utc_time, sizeof(utc_time));
		PFAULT(fd_fault, "time: %s", utc_time);

		dump_ver();
		sp_dump();
		channel_cntl_dump();
		hw_cntl_dump();

		channel_cntl_destroy();

		sync();
		exit(-EINVAL);
#else
		break;
#endif
	}
}

void early_ev_init()
{
	int i;
	int err = 0;
	struct sigaction old;
	struct sigaction new;

	stack_t oss;
	stack_t nss;

#ifdef CFG_HANDLE_FAULT_SIG
	g_fd_fault = open(FAULT_LOG_FILE, O_RDWR | O_APPEND | O_CREAT,
			S_IRUSR | S_IWUSR |
			S_IRGRP | S_IWGRP |
			S_IROTH | S_IWOTH);
	if (-1 == g_fd_fault) {
		PERR("fault can not be logged");
	}
#endif
	nss.ss_sp = malloc(SIGSTKSZ);
	nss.ss_size = SIGSTKSZ;
	nss.ss_flags = 0;

	memset(&new, 0, sizeof(new));
	new.sa_sigaction = sighandler_generic;
	new.sa_flags = SA_SIGINFO;

	if (NULL != nss.ss_sp) {
		new.sa_flags |= SA_ONSTACK;
	}

	for (i = 1; i < SIGTTOU; i++) {
		err = 0;
		switch (i) {
		case SIGSTOP:
		case SIGKILL:
		case SIGTRAP:
			break;
		case SIGUSR1:
		case SIGUSR2:
			err = sigaction(i, &new, &old);
			break;
		default:
#ifdef CFG_HANDLE_FAULT_SIG
			err = sigaction(i, &new, &old);
#endif
			break;
		}

		if (err) {
			PERR("reg sig for: %d error.", i);
		}
	}

	if (NULL != nss.ss_sp) {
		err = sigaltstack(&nss, &oss);
		if (err) {
			PERR("error set alt stk");
		}
	}
}
#endif //__DEBUG_SIGALT_STACK_SUPPORT__

static int fifo_init()
{
	int err = 0;
	char *filename;
#ifdef CFG_DISCARD_OLD_CMD
	char buf[16] = "";
#endif

	pthread_mutex_init(&mutex_dat_fifo, NULL);
	filename = FIFO_CMD;
	err = access(filename, F_OK | R_OK | W_OK);
	if (err) {
		PERR("can not access %s, will be created", filename);
		unlink(filename);
		err = mkfifo(filename, 0666);
		if (err) {
			PERR("error creating file: %s", filename);
			return err;
		}
	}

#ifdef CFG_DISCARD_OLD_CMD
	g_fd_fifo_cmd = open(filename, O_RDONLY | O_NONBLOCK);

	while (1) {
		err = read(g_fd_fifo_cmd, buf, sizeof(buf));
		if (err < (int)sizeof(buf)) {
			PINFO("old cmds purged out");
			break;
		}
	}

	close(g_fd_fifo_cmd);
#endif
	g_fd_fifo_cmd = open(filename, O_RDWR);
	if (g_fd_fifo_cmd < 0) {
		PERR("error openning file: %s", filename);
		err = g_fd_fifo_cmd;
		return err;
	}


	filename = FIFO_DAT;
	err = access(filename, F_OK | R_OK | W_OK);
	if (err) {
		PERR("can not access %s, will be created", filename);
		unlink(filename);
		err = mkfifo(filename, 0666);
		if (err) {
			PERR("error creating file: %s", filename);
			return err;
		}
	}

	g_fd_fifo_dat = open(filename, O_RDWR);
	if (g_fd_fifo_dat < 0) {
		PERR("error openning file: %s", filename);
		err = -EIO;
		return err;
	}

	err = fchmod(g_fd_fifo_cmd, 0666);
	err |= fchmod(g_fd_fifo_dat, 0666);
	PINFO("g_fd_fifo_cmd: %d g_fd_fifo_dat: %d", g_fd_fifo_cmd, g_fd_fifo_dat);

	return err;
}


int ev_init()
{
	int err = 0;
#ifdef CFG_CHECK_DISPLAY_STATE
	int param = 0;
#endif


#ifdef CFG_CHECK_DISPLAY_STATE
	PINFO("start thread for display state checking...");
	err = pthread_create(&g_tid_check_display_state, NULL,
			ev_loop_check_display_state, &param);
	if (err) {
		PERR("error creating thread, cannot listen to display sleep event");
	} else {
		PINFO("thread for display state checking started");
	}
#endif

	err = fifo_init();

	return err;
}


#ifdef CFG_CHECK_DISPLAY_STATE
void register_display_event_handler(display_event_handler_t *phandler)
{
	g_display_event_handler = phandler;
}


void unregister_display_event_handler(display_event_handler_t *phandler)
{
	g_display_event_handler = NULL;
}
#endif
