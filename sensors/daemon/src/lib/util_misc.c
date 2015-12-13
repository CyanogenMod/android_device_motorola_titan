/*!
 * @section LICENSE
 *
 * (C) Copyright 2014 Bosch Sensortec GmbH All Rights Reserved
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
 * @file         util_misc.c
 * @date         "Fri Apr 19 15:00:50 2013 +0800"
 * @commit       "9f2a6a2"
 *
 * @brief
 *
 * @detail
 *
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>

#define LOG_TAG_MODULE "<util_misc>"
#include "sensord.h"



void get_token_pairs(const char *psz_line, char seperator, char *ptoken_l, char *ptoken_r)
{
	char ch;
	int i;
	int pos = -1;

	i = 0;
	while ('\0' != (ch = psz_line[i])) {
		if (ch == seperator) {
			pos = i;
			break;
		} else {
			ptoken_l[i] = psz_line[i];
		}
		i++;
	}
	ptoken_l[i] = '\0';

	if (-1 != pos) {
		i = pos + 1;
		while ('\0' != psz_line[i]) {
			ptoken_r[i - pos - 1] = psz_line[i];
			i++;
		}
		ptoken_r[i - pos - 1] = '\0';
	} else {
		ptoken_r[0] = '\0';
	}
}

int txt_file_read_int(int fd, int *pvalue, int rewind)
{
	int err = 0;
	char buf[16] = "";	/* FIXME */
	int tmp;

	if (rewind) {
		lseek(fd, 0, SEEK_SET);
	}

	tmp = read(fd, buf, sizeof(buf) - 1);
	if (0 < tmp) {
		buf[tmp] = 0;
		tmp = sscanf(buf, "%11d", pvalue);
		if (1 != tmp) {
			err = -EINVAL;
		}
	} else {
		PERR("error reading from file");
		err = -EIO;
	}

	return err;
}


int txt_file_write_int(int fd, int value, short rewind, short nl)
{
	int err = 0;
	char buf[16] = "";	/* FIXME */
	int tmp;

	if (rewind) {
		lseek(fd, 0, SEEK_SET);
	}

	tmp = sprintf(buf, "%d%s", value, nl ? "\n" : "");

	if (tmp > 0) {
		tmp = write(fd, buf, tmp + 1);
		if (tmp <= 0) {
			err = -1;
			PERR("err writing to %d %d: %d", value, fd, tmp);
		}
	} else {
		err = -1;
	}

	return err;
}


void list_init(struct list_node *head)
{
	head->next = NULL;
}


void list_add_head(struct list_node *head, struct list_node *new)
{
	new->next = head;
}


void list_add_rear(struct list_node *head, struct list_node *new)
{
	struct list_node *cur = head;

	while (NULL != cur->next) {
		cur = cur->next;
	}

	cur->next = new;
	new->next = NULL;
}


/*
 * @description: try to find the nod with the address
 * @return
 * NULL if the nod is not found, otherwise the nod itself is returned
 */
struct list_node * list_find_node(struct list_node *head, struct list_node *nod)
{
	struct list_node *cur = head;

	while (NULL != cur) {
		if (cur == nod) {
			break;
		}

		cur = cur->next;
	}

	return cur;
}


/*
 * @return
 * the new head of the list
 */
struct list_node * list_del_node(struct list_node *head, struct list_node *nod)
{
	struct list_node *cur = head;
	struct list_node *prev = NULL;
	struct list_node *new_head = head;

	while (NULL != cur) {
		if (cur == nod) {
			if (cur == head) {
				new_head = head->next;
			}

			if (NULL != prev) {
				prev->next = cur->next;
			}

			break;
		} else {
			prev = cur;
			cur = cur->next;
		}
	}

	return new_head;
}


int get_max_comm_div(int a, int b)
{
	int mcd = 1;
	int r;

	if (0 == a) {
		return b;
	}

	if (0 == b) {
		return a;
	}

	if (a < 0) {
		a = -a;
	}

	if (b < 0) {
		b = -b;
	}

	if (a < b) {
		SWAP(int, a, b);
	}

	while (1) {
		mcd = b;
		r = a % b;

		if (0 != r) {
			a = b;
			b = r;
		} else {
			break;
		}
	}

	return mcd;
}


int32_t get_nearest_divisible_int(uint32_t a, uint32_t b)
{
	int32_t ret;

	if (0 == b) {
		/* error */
		return -1;
	}

	if (0 == a) {
		return (int32_t)b;
	}

	if (a <= b) {
		return (int32_t)a;
	}

	for (ret = b; ret > 0; ret--) {
		if (0 == (a % ret)) {
			break;
		}
	}

	return ret;
}


int get_num_set_bits_u32(uint32_t n)
{
	unsigned int i;
	int ret;

	ret = 0;
	for (i = 0; i < sizeof(n) * 8; i++) {
		if ((1<<i) & n) {
			ret++;
		}
	}

	return ret;
}
