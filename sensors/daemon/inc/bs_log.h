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
 *  Disclaimer
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
 * @file         bs_log.h
 * @date         "Fri Sep 27 11:21:26 2013 +0800"
 * @commit       "8c4437a"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __BS_LOG_H
#define __BS_LOG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef CFG_LOG_WITH_TIME
#include "util_misc.h"
#define GET_TIME_TICK() (get_time_tick())
#else
#define GET_TIME_TICK() (0)
#endif

#define LOG_LEVEL_Q 0	/* quiet */
#define LOG_LEVEL_E 3
#define LOG_LEVEL_W 4
#define LOG_LEVEL_N 5
#define LOG_LEVEL_I 6
#define LOG_LEVEL_D 7

#ifndef CFG_LOG_LEVEL
#define CFG_LOG_LEVEL LOG_LEVEL_I
#endif

#ifndef LOG_TAG_MODULE
#define LOG_TAG_MODULE
#endif

#if (defined CFG_LOG_TO_LOGCAT && defined CFG_LOG_TO_TERM) ||\
	(defined CFG_LOG_TO_LOGCAT && defined CFG_LOG_TO_FILE) || \
	(defined CFG_LOG_TO_FILE && defined CFG_LOG_TO_TERM)
#error "Conflicting options!"
#endif

#ifdef CFG_LOG_TO_LOGCAT
#include<android/log.h>

/* here use android api instead of cutils to make compatibility
	with ndk build */
#define LOG_TAG    "sensord"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#ifndef LOGE_IF
#define LOGE_IF(cond, ...) \
    ( (cond) \
    ? ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

#define BS_LOG LOGD
#define OUT_CHANNEL

#else

#include <stdio.h>
#include "trace.h"
#define BS_LOG trace_log
extern int g_fd_trace;

#define OUT_CHANNEL g_fd_trace,
#endif


#if (CFG_LOG_LEVEL >= LOG_LEVEL_E)
#define PERR(fmt, args...) BS_LOG(OUT_CHANNEL "\n" "[%u]" "[E]" "BS_ERR" LOG_TAG_MODULE "<%s><%d>" fmt "<reason> %s" "\n", GET_TIME_TICK(), __FUNCTION__, __LINE__, ##args, (char *)strerror(errno))
#else
#define PERR(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_W)
#define PWARN(fmt, args...) BS_LOG(OUT_CHANNEL "\n" "[%u]" "[W]" "BS_WARN" LOG_TAG_MODULE "<%s><%d>" fmt "\n", GET_TIME_TICK(), __FUNCTION__, __LINE__, ##args)
#else
#define PWARN(fmt, args...)
#endif


#if (CFG_LOG_LEVEL >= LOG_LEVEL_N)
#define PNOTICE(fmt, args...) BS_LOG(OUT_CHANNEL "\n" "[%u]" "[N]" "BS_NOTICE" LOG_TAG_MODULE "<%s><%d>" fmt "\n", GET_TIME_TICK(), __FUNCTION__, __LINE__, ##args)
#else
#define PNOTICE(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_I)
#define PINFO(fmt, args...) BS_LOG(OUT_CHANNEL "\n" "[%u]" "[I]" "BS_INFO" LOG_TAG_MODULE "<%s><%d>" fmt "\n", GET_TIME_TICK(), __FUNCTION__, __LINE__, ##args)
#else
#define PINFO(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_D)
#define PDEBUG(fmt, args...) BS_LOG(OUT_CHANNEL "\n" "[%u]" "[D]" "BS_DBG" LOG_TAG_MODULE "<%s><%d>" fmt "\n", GET_TIME_TICK(), __FUNCTION__, __LINE__, ##args)
#else
#define PDEBUG(fmt, args...)
#endif

#endif
