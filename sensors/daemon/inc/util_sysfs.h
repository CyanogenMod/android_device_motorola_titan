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
 * @file         util_sysfs.h
 * @date         "Wed May 15 13:59:10 2013 +0800"
 * @commit       "efc882a"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __UTIL_SYSFS_H
#define __UTIL_SYSFS_H
#include <stdio.h>

int sysfs_get_input_dev_num(const char *pname);
int sysfs_read_int(const char *path, int *value);
int sysfs_write_int(const char *path, int value);
int sysfs_open_input_dev_node(int input_dev_num, const char *name, int mode);
FILE *sysfs_fopen_input_dev_node(int input_dev_num, const char *name, char *mode);

/*!
 *
 * @brief this function attempts to open a file to read up to 'size' bytes from
 * that file into 'buf'
 *
 * @param[i]   filename   filename to be read
 * @param[o]  buf           out put data buffer
 * @param[i]   size          input size to be read, size must be less than
 *                                   'buf' size
 *
 * @return     >= 0 success. or -1 failed
 * @retval      >=0 the count of true read size
 * @retval      -1    failed to open or read file
 */
int util_fs_read_file(const char *filename, void *buf, int size);

/*!
 *
 * @brief this function attempts to open a file to write up to 'size' bytes to
 * that file from 'buf'
 *
 * @param[i]   filename   filename to be read
 * @param[i]  buf            input data buffer
 * @param[i]   size          input size to be writen, size must be less or
 *                                   equal than 'buf' size
 *
 * @return     >= 0 success. or -1 failed
 * @retval      >=0 the count of true read size
 * @retval      -1    failed to open or read file
 */
int util_fs_write_file(const char *filename, void *buf, int size);

/*!
 * @brief This function atempts to get the size of the file
 *
 * @param[i]  filename   target filename
 *
 * @return >= 0 success, return the file size, or -1 failed.
 *
 * @retval >=0, file size
 * @retval -1,    failed to get file size
 *
 */
int util_fs_get_filesize(const char *filename);

/*!
 * @brief This function detects whether a file is existing or not
 *
 * @param[i]   filename     input file name
 *
 * @return 0 file exist, or -1 file does not exist
 */
int util_fs_is_file_exist(const char *filename);


#endif
