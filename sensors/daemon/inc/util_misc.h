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
 * @file         util_misc.h
 * @date         "Wed Apr 3 14:05:34 2013 +0800"
 * @commit       "5420311"
 *
 * @brief
 *
 * @detail
 *
 */

#ifndef __UTIL_MISC_H
#define __UTIL_MISC_H


#define ABS(x) ((x) > 0 ? (x) : -(x))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))
#endif

#ifndef OFFSET_OF
#define OFFSET_OF(type, member)\
	((size_t) &((type *)0)->member)
#endif

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member)\
	(type *)((char *)(ptr) - OFFSET_OF(type, member))
#endif


#define FLOAT_EQU(x, y)\
	((x > y) ? ((x - y) < 0.000001) : ((y - x) < 0.000001))

#define EMIT_BUG() \
	do {\
		fprintf(stderr, "bug at file: %s line: %d\n", __FILE__, __LINE__);\
		*((unsigned char *)0) = 0;\
	} while (0);

#define SET_VALUE_BITS(x,val,start,bits)\
	(((x) & ~(((1 << (bits)) - 1) << (start))) | ((val) << (start)))


#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define SWAP(type, a, b)\
	do {\
		type t;\
		t = a;\
		a = b;\
		b = t;\
	} while (0);


#define UNUSED_PARAM(param) ((void)(param))
#define __unused__ __attribute__((unused))

struct list_node {
	struct list_node *next;
	long data;
};

struct dlist_node {
	struct dlist_node *prev;
	struct dlist_node *next;
	long data;
};

void util_misc_init();

void get_token_pairs(const char *psz_line, char seperator,
		char *ptoken_l, char *ptoken_r);

int txt_file_read_int(int fd, int *pvalue, int rewind);
int txt_file_write_int(int fd, int value, short rewind, short nl);


void list_init(struct list_node *head);
void list_add_head(struct list_node *head, struct list_node *new);
void list_add_rear(struct list_node *head, struct list_node *new);
struct list_node * list_find_node(struct list_node *head, struct list_node *nod);
struct list_node * list_del_node(struct list_node *head, struct list_node *nod);


int get_max_comm_div(int a, int b);
int32_t get_nearest_divisible_int(uint32_t a, uint32_t b);
/* get number of 1s (set bits) in a u32 integer */
int get_num_set_bits_u32(uint32_t n);
#endif
