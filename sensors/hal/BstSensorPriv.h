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
 * @file         BstSensorPriv.h
 * @date         "Wed Apr 3 14:05:34 2013 +0800"
 * @commit       "5420311"
 *
 * @brief
 *
 * @detail
 *
 */


#ifndef __SENSOR_PRIV_H
#define __SENSOR_PRIV_H

#include "BstSensorDataType.h"

#define PATH_DIR_SENSOR_STORAGE "/data/misc/sensor"
#define FIFO_CMD (PATH_DIR_SENSOR_STORAGE "/fifo_cmd")
#define FIFO_DAT (PATH_DIR_SENSOR_STORAGE "/fifo_dat")

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))
#endif

#define SCALE_TIME_S2NS 1000000000LL
#define SCALE_TIME_MS2NS 1000000LL
#define SCALE_TIME_US2NS 1000LL


#define SET_SENSOR_ACTIVE       0x01
#define SET_SENSOR_DELAY        0x02
#define SET_SENSOR_FLUSH        0x03

#define CHANNEL_PKT_MAGIC_CMD   (int)'C'
#define CHANNEL_PKT_MAGIC_DAT   (int)'D'
#define CHANNEL_PKT_MAGIC_LIST  (int)'L'

#ifdef __HYBRID_HAL__
struct bst_axis_remap {
    int src_x : 3;
    int src_y : 3;
    int src_z : 3;

    int sign_x : 2;
    int sign_y : 2;
    int sign_z : 2;
};

#define BST_DFT_AXIS_REMAP_TAB_SZ 8
#endif

#endif