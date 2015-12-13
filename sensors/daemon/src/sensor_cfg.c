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
 * @file         sensor_cfg.c
 * @date         "Fri Apr 19 15:00:50 2013 +0800"
 * @commit       "9f2a6a2"
 *
 * @brief
 *
 * @detail
 *
 */

#include <stdio.h>
#include <errno.h>

#define LOG_TAG_MODULE "<sensor_cfg>"
#include "sensord.h"

extern int g_place_a;	/* placement of acc sensor */
extern int g_place_m;	/* placement of mag sensor */
extern int g_place_g;	/* placement of mag sensor */

const ts_sensmatrix softiron_default = {
	{1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, 0.0, 1.0}
};

#ifdef __BMA250E__
const unsigned char bma_spec[] = { /* bma250e */
	37,0,1,1,0,7,9,150,0,10,180,0,5,0,1,0,176,4,32,3,0,0,64,
	65,1,1,1,2,2,2,3,4,4,1,1,25,213,};
#elif defined __BMA222E__
const unsigned char bma_spec[] = { /* bma222e */
	37,0,4,1,0,7,9,150,0,8,180,0,5,0,1,0,176,4,32,3,0,0,64,
	65,1,1,1,2,2,2,3,4,4,1,1,72,215,};
#elif  (defined __BMA255__) || (defined __BMC055__)\
 || (defined __BMC056__) || (defined __BMI055__)
const unsigned char bma_spec[] = { /* bma255 */
	37,0,2,1,0,7,9,150,0,12,180,0,5,0,1,0,176,4,82,3,0,0,64,
	65,1,1,1,1,2,2,2,3,3,1,1,172,167,};
#elif (defined __BMI058__) || (defined __BMA280__)
const unsigned char bma_spec[] = { /* bma280 */
	37,0,3,1,0,7,9,150,0,14,180,0,5,0,1,0,176,4,82,3,0,0,64,
	65,1,1,1,1,2,2,2,3,3,1,1,1,169,};
#else
#warning not define BMA, use bma255 as default
const unsigned char bma_spec[] = { /* bma255 */
	37,0,2,1,0,7,9,150,0,12,180,0,5,0,1,0,176,4,82,3,0,0,64,
	65,1,1,1,1,2,2,2,3,3,1,1,172,167,};
#endif

const unsigned char bmg160_spec[] = {
	14,0,1,1,1,7,200,136,19,16,1,1,203,8,};

#if defined (__BMC055__) || defined (__BMC056__)
const unsigned char bmm_spec[] = { /* bmm050 */
	39,0,1,1,232,3,232,3,232,3,20,100,112,23,0,0,128,61,205,
	204,76,63,0,0,224,64,1,1,1,1,1,1,1,1,1,1,1,11,213,};
#elif defined (__BMC150__) || defined (__BMM150__)
const unsigned char bmm_spec[] = { /* bmm150 */
	39,0,2,1,20,5,20,5,196,9,20,100,112,23,0,0,128,61,205,204,
	76,63,0,0,32,65,1,1,1,1,1,1,1,1,1,1,1,138,155,};
#else
/* waring not define BMM, use bmm150 as defalut */
const unsigned char bmm_spec[] = { /* bmm150 */
	39,0,2,1,20,5,20,5,196,9,20,100,112,23,0,0,128,61,205,204,
	76,63,0,0,224,64,1,1,1,1,1,1,1,1,1,1,1,62,103,};

#endif

const unsigned char usecase_spec[] = { /* usecase defalut */
	64,6,1,1,1,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,51,179,62,205,204,12,63,0,0,0,0,51,51,51,63,205,204,76,63,
	1,0,7,2,23,183,209,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,183,209,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,183,
	209,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,189,55,134,53,189,55,134,53,189,55,134,53,0,0,0,0,0,0,16,66,
	232,3,5,0,45,0,132,3,176,4,150,0,8,150,0,12,1,3,1,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,51,51,179,62,205,
	204,12,63,0,0,0,0,51,51,51,63,205,204,76,62,1,3,2,0,5,0,65,1,64,1,36,0,120,0,4,1,20,20,2,2,0,4,0,0,128,63,
	205,204,204,61,154,153,153,63,205,204,204,62,205,204,204,61,1,0,20,0,16,4,120,0,8,0,0,5,154,153,25,63,154,
	153,25,63,80,0,9,0,30,0,232,3,80,0,65,0,4,0,4,0,0,128,62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,62,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,62,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,181,254,22,55,181,254,22,55,
	181,254,22,55,139,222,169,56,0,0,224,64,12,1,1,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,
	63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,205,204,204,61,1,7,3,19,50,163,4,205,12,25,40,4,12,0,3,154,153,153,62,154,153,153,62,205,204,204,62,
	154,153,25,63,154,153,153,62,0,0,128,62,154,153,153,62,236,81,184,62,205,204,76,63,205,204,76,63,205,204,76,
	63,205,204,76,63,205,204,76,62,205,204,76,62,205,204,76,62,205,204,76,62,1,194,184,178,62,53,250,142,60,10,0,
	10,0,0,2,0,10,0,80,119,86,61,12,0,0,128,62,143,194,245,60,10,215,163,60,100,128,52,45,70,1,10,0,80,0,0,0,192,
	63,0,0,0,64,9,2,0,0,200,65,0,0,128,66,0,0,128,65,0,0,192,63,205,204,76,61,194,184,178,61,50,37,59,24,71,0,0,
	160,64,154,153,25,63,80,119,86,61,0,1,205,204,76,63,0,0,96,64,0,0,32,64,205,204,204,61,4,143,194,245,60,2,1,
	2,4,5,1,10,176,4,100,0,10,215,35,60,10,0,10,0,0,0,250,67,0,0,122,68,0,0,160,63,0,0,72,66,0,0,128,63,154,153,
	25,62,154,153,153,61,0,0,32,66,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,63,
	128,150,24,75,0,0,0,0,0,0,0,0,0,0,0,0,128,150,24,75,0,0,0,0,0,0,0,0,0,0,0,0,128,150,24,75,0,0,192,64,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,192,
	64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,128,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,128,64,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,128,64,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,23,183,209,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,183,209,56,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,183,209,56,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,215,35,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,172,197,39,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,172,197,39,55,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,172,197,39,55,0,36,116,73,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	36,116,73,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,116,73,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,10,3,4,25,64,18,24,0,64,114,8,0,12,86,109,

};


static void set_cfg_axis()
{
	int err = 0;
	int fd;
	char magic;
	int count;
	int place;
	char buf[64] = "";
	char *pline = NULL;     /* start of a line */
	char *pline_end = NULL;
	int i;

	fd = open(SENSOR_CFG_FILE_SYS_AXIS, O_RDONLY);

	if (-1 == fd) {
		PERR("axis config file does not exist, assuming no remap is needed");
		err = -EIO;
		goto exit_err_clean;
	} else {
		/* bytes exceeding 63 will be disgarded */
		count = read(fd, buf, ARRAY_SIZE(buf) - 1);
		if (0 < count) {
			buf[count] = '\0';
		}
		for (i = 0; i < count; i++) {
			if ('\r' == buf[i]) {
				buf[i] = '\n';
			}
		}

		pline_end = buf - 1;
		while (NULL != pline_end) {
			pline = pline_end + 1;

			pline_end = strchr(pline, '\n');
			if (NULL != pline_end) {
				*pline_end = '\0';
			}

			if ('\0' == pline[0]) {
				continue;
			}

			count = sscanf(pline, "place_%c=%11d", &magic, &place);
			PDEBUG("count: %d magic: %c place: %d",
						count, magic, place);

			if (2 != count) {
				err = -EINVAL;
				PWARN("invalid line:%s in config file: %s",
				      pline, SENSOR_CFG_FILE_SYS_AXIS);
				goto exit_err_clean;
			}

			PINFO("g_place_%c: %d", magic, place);
			switch(magic) {
			case SENSOR_MAGIC_A:
				g_place_a = place;
				break;
			case SENSOR_MAGIC_M:
				g_place_m = place;
				break;
			case SENSOR_MAGIC_G:
				g_place_g = place;
				break;
			default:
				err = -EINVAL;
				PWARN("Unknown sensor");
				goto exit_err_clean;
				break;
			}
		}
	}

exit_err_clean:
	if (err) {
		PWARN("error reading from config file: %s, using default",
				SENSOR_CFG_FILE_SYS_AXIS);
	}

	if (-1 != fd) {
		close(fd);
	}
}


void sensor_cfg_init()
{
	set_cfg_axis();
}

