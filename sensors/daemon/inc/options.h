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
 * @file         options.h
 * @date         "Thu Aug 15 10:19:36 2013 +0800"
 * @commit       "97d94e6"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __OPTIONS_H
#define __OPTIONS_H
#include "sensor_def.h"
#include "configure.h"

//#define CFG_ACC_USE_RAW_DATA
#define CFG_DISCARD_OLD_CMD
#define CFG_SET_AXIS_FROM_FILE

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_A_MIN 10
#else
#define CFG_DELAY_A_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_A_MAX 200

#define CFG_DELAY_M_MIN 50
#define CFG_DELAY_M_MIN_M4G 20
#define CFG_DELAY_M_MAX 200

#define CFG_DELAY_VIRTUAL_G_MIN 20
#define CFG_DELAY_VIRTUAL_G_MAX 200

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_G_MIN 10
#else
#define CFG_DELAY_G_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_G_MAX 200

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_O_MIN 10
#else
#define CFG_DELAY_O_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_O_MAX 200

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_VG_MIN 10
#else
#define CFG_DELAY_VG_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_VG_MAX 200

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_VLA_MIN 10
#else
#define CFG_DELAY_VLA_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_VLA_MAX 200

#ifdef __FASTEST_MODE_100HZ__
#define CFG_DELAY_VRV_MIN 10
#else
#define CFG_DELAY_VRV_MIN 5
#endif //endif of __FASTEST_MODE_100HZ__
#define CFG_DELAY_VRV_MAX 200

#define CFG_DELAY_GEST_FLIP_MIN   100
#define CFG_DELAY_GEST_FLIP_MAX   100

#define FLIP_FACE_UP              105
#define FLIP_FACE_DOWN            15

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
#define CFG_DELAY_GRV_MIN CFG_DELAY_VRV_MIN
#define CFG_DELAY_GRV_MAX CFG_DELAY_VRV_MAX

#define CFG_DELAY_GU_MIN CFG_DELAY_G_MIN
#define CFG_DELAY_GU_MAX CFG_DELAY_G_MAX

#ifdef __USECASE_TYPE_M4G__
#define CFG_DELAY_MU_MIN CFG_DELAY_M_MIN_M4G
#else
#define CFG_DELAY_MU_MIN CFG_DELAY_M_MIN
#endif
#define CFG_DELAY_MU_MAX CFG_DELAY_M_MAX

#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
#define CFG_DELAY_GEOM_RV_MIN CFG_DELAY_VRV_MIN
#define CFG_DELAY_GEOM_RV_MAX CFG_DELAY_VRV_MAX
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
#define CFG_DELAY_STC_MIN CFG_DELAY_A_MIN
#define CFG_DELAY_STC_MAX CFG_DELAY_A_MAX

#define CFG_DELAY_STD_MIN CFG_DELAY_A_MIN
#define CFG_DELAY_STD_MAX CFG_DELAY_A_MAX

#endif


#endif
