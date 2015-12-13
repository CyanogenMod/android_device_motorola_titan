/*
 * Copyright (C) 2008 The Android Open Source Project
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
 */

#ifndef ANDROID_SENSORS_H
#define ANDROID_SENSORS_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/input.h>

#include "TargetPlatform.h"

__BEGIN_DECLS

/*****************************************************************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define UNUSED_PARAM(param) ((void)(param))

#include <android/log.h>

#define LOG_LEVEL_Q 0	/* quiet */
#define LOG_LEVEL_E 3
#define LOG_LEVEL_W 4
#define LOG_LEVEL_N 5
#define LOG_LEVEL_I 6
#define LOG_LEVEL_D 7

#ifndef CFG_LOG_LEVEL
#define CFG_LOG_LEVEL LOG_LEVEL_I
#endif

/* here use android api instead of cutils to make compatibility
	with ndk build */
#ifndef LOG_TAG
	#define LOG_TAG    "bsthal"
#endif
#ifndef LOGD
	#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif
#ifndef LOGI
	#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif
#ifndef LOGW
	#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#endif
#ifndef LOGE
	#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif
#ifndef LOGF
	#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#endif
#ifndef LOGE_IF
#define LOGE_IF(cond, ...) \
    ( (cond) \
    ? ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_E)
#define PERR LOGE
#else
#define PERR(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_E)
#define PERR_IF LOGE_IF
#else
#define PERR_IF(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_W)
#define PWARN LOGW
#else
#define PWARN(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_N)
#define PNOTICE LOGW
#else
#define PNOTICE(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_I)
#define PINFO LOGI
#else
#define PINFO(fmt, args...)
#endif

#if (CFG_LOG_LEVEL >= LOG_LEVEL_D)
#define PDEBUG LOGD
#else
#define PDEBUG(fmt, args...)
#endif


enum {
	ID_SENSOR_BASE = 0,

	ID_P,
	ID_L,


	/* sensors supported by BST */
	ID_SENSOR_BASE_BST = (ID_SENSOR_BASE + '_BST'),

	ID_A,           /* 0: acceleration */
	ID_M,           /* 1: magnetic */
	ID_GY,          /* 2: gyro */
	ID_O,           /* 3: orientation */
	ID_GR,          /* 4: gravity */
	ID_LA,          /* 5: linear acceleration */
	ID_RV,          /* 6: rotation vector */
	ID_PR,          /* 7: pressure sensor */
	ID_GEST_FLIP,   /* 8: flip gesture sensor */
	ID_SGM,         /* 9: software significant motion sensor */
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
	ID_GRV,         /* 10: game rotation vector */
	ID_GYU,         /* 11: gyro uncalibrated */
	ID_MU,          /* 12: magnetic uncalibrated */
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
	ID_GEOM_RV,     /* 13: geomagnetic rotation vector */
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
	ID_STC,         /* 14: step counter */
	ID_STD,         /* 15: step detector */
#endif

#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
	ID_SMT,         /* 16: significant motion */
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	ID_DTAP,        /* 17: double tap */
#endif

	ID_SENSOR_END_BST
};

#define SENSORS_ROTATION_VECTOR_HANDLE  (ID_RV)
#define SENSORS_LINEAR_ACCEL_HANDLE     (ID_LA)
#define SENSORS_GRAVITY_HANDLE          (ID_GR)
#define SENSORS_GYROSCOPE_HANDLE        (ID_GY)
#define SENSORS_ACCELERATION_HANDLE     (ID_A)
#define SENSORS_MAGNETIC_FIELD_HANDLE   (ID_M)
#define SENSORS_ORIENTATION_HANDLE      (ID_O)
#define SENSORS_LIGHT_HANDLE            (ID_L)
#define SENSORS_PROXIMITY_HANDLE        (ID_P)
#define SENSORS_PRESSURE_HANDLE         (ID_PR)
#define SENSORS_GEST_FLIP_HANDLE        (ID_GEST_FLIP)

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
#define SENSORS_MAGNETIC_UNCALIBRATED_HANDLE        (ID_MU)
#define SENSORS_GAME_ROTATION_VECTOR_HANDLE        (ID_GRV)
#define SENSORS_GYROSCOPE_UNCALIBRATED_HANDLE        (ID_GYU)
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
#define SENSORS_GEOMAGNETIC_ROTATION_VECTOR_HANDLE	(ID_GEOM_RV)
#endif

#ifdef __SIGNIFICANT_MOTION_SENSOR_SUPPORT__
#define SENSORS_SIGNIFICANT_MOTION_HANDLE        (ID_SMT)
#endif
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
#define SENSORS_DOUBLE_TAP_HANDLE        (ID_DTAP)
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
#define SENSORS_STEP_COUNTER_HANDLE	(ID_STC)
#define SENSORS_STEP_DETECTOR_HANDLE	(ID_STD)
#endif

#define SENSORS_SW_SIGNIFICANT_MOTION_HANDLE (ID_SGM)

/*****************************************************************************/

/*
 * The SENSORS Module
 */

/* the GP2A is a binary proximity sensor that triggers around 5 cm on
 * this hardware */
#define PROXIMITY_THRESHOLD_GP2A  5.0f

/*****************************************************************************/

#define DEV_NAME_A "bma2x2"

#define EVENT_TYPE_ACCEL_X          REL_Y
#define EVENT_TYPE_ACCEL_Y          REL_X
#define EVENT_TYPE_ACCEL_Z          REL_Z

#define EVENT_TYPE_YAW              REL_RX
#define EVENT_TYPE_PITCH            REL_RY
#define EVENT_TYPE_ROLL             REL_RZ
#define EVENT_TYPE_ORIENT_STATUS    REL_WHEEL

/* For AK8973iB */
#define EVENT_TYPE_MAGV_X           REL_DIAL
#define EVENT_TYPE_MAGV_Y           REL_HWHEEL
#define EVENT_TYPE_MAGV_Z           REL_MISC

#define EVENT_TYPE_PROXIMITY        ABS_DISTANCE
#define EVENT_TYPE_LIGHT            ABS_MISC

#define EVENT_TYPE_GYRO_X           REL_RY
#define EVENT_TYPE_GYRO_Y           REL_RX
#define EVENT_TYPE_GYRO_Z           REL_RZ


// 64 LSB = 1G for KR3DM
#define LSB                         (64.0f)
#define NUMOFACCDATA                (8.0f)

// conversion of acceleration data to SI units (m/s^2)
#define RANGE_A                     (2*GRAVITY_EARTH)
#define CONVERT_A                   (GRAVITY_EARTH / LSB / NUMOFACCDATA)
#define CONVERT_A_X                 (CONVERT_A)
#define CONVERT_A_Y                 (CONVERT_A)
#define CONVERT_A_Z                 (CONVERT_A)

// conversion of magnetic data to uT units
#define CONVERT_M                   (1.0f/16.0f)
#define CONVERT_M_X                 (-CONVERT_M)
#define CONVERT_M_Y                 (-CONVERT_M)
#define CONVERT_M_Z                 (-CONVERT_M)

/* conversion of orientation data to degree units */
#define CONVERT_O                   (1.0f/64.0f)
#define CONVERT_O_A                 (CONVERT_O)
#define CONVERT_O_P                 (CONVERT_O)
#define CONVERT_O_R                 (-CONVERT_O)

// conversion of gyro data to SI units (radian/sec)
#define RANGE_GYRO                  (2000.0f*(float)M_PI/180.0f)
#define CONVERT_GYRO                ((70.0f / 1000.0f) * ((float)M_PI / 180.0f))
#define CONVERT_GYRO_X              (CONVERT_GYRO)
#define CONVERT_GYRO_Y              (-CONVERT_GYRO)
#define CONVERT_GYRO_Z              (CONVERT_GYRO)

#define SENSOR_STATE_MASK           (0x7FFF)

/*****************************************************************************/

#define MAX_SENSORS		(20)
#define SYSFS_MAXLEN		(20)
#define SYSFS_CLASS		"/sys/class/sensors"
#define SYSFS_NAME		"name"
#define SYSFS_VENDOR		"vendor"
#define SYSFS_VERSION		"version"
#define SYSFS_HANDLE		"handle"
#define SYSFS_TYPE		"type"
#define SYSFS_MAXRANGE		"max_range"
#define SYSFS_RESOLUTION	"resolution"
#define SYSFS_POWER		"sensor_power"
#define SYSFS_MINDELAY		"min_delay"
#define SYSFS_ENABLE		"enable"
#define SYSFS_POLL_DELAY	"poll_delay"

#define COMPASS_VENDOR_AKM		"AKM"
#define COMPASS_VENDOR_ALPS		"Alps"
#define COMPASS_VENDOR_YAMAHA		"Yamaha"
#define COMPASS_VENDOR_MEMSIC		"Memsic"
#define COMPASS_VENDOR_ST		"STMicro"
#define COMPASS_VENDOR_BOSCH		"Bosch"
#define COMPASS_VENDOR_KIONIX		"Kionix"
#define COMPASS_VENDOR_INVENSENSE	"Invensense"

__END_DECLS

#endif  // ANDROID_SENSORS_H
