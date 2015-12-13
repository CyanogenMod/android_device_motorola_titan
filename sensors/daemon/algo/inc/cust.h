#ifndef __ALGO_CUST_H
#define __ALGO_CUST_H

/*!
* @section LICENCE
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
 
*
* @file      cust.h
* @date      2011/08/12 created
*
* @brief
* This file provides the configuration paramters constants definition for algorithm
*
* @detail
* 
*
*/



#define ALGO_CUST_A_ADV_FILT_COEF 20
#define ALGO_CUST_A_ADV_FILT_NL 4

#define ALGO_CUST_M_ADV_FILT_COEF 20
#define ALGO_CUST_M_ADV_FILT_NL 5

#define ALGO_CUST_COMPASS_HEADING_SENSITIVITY 4
#define ALGO_CUST_NDOF_ORI_CORRECT_SPEED 1
#define ALGO_CUST_MAG_FILT_MODE 1
#define ALGO_CUST_MAG_CALIB_ACC_SENSITIVITY 0
#define ALGO_CUST_MAG_CALIB_SPEED 5
#define ALGO_CUST_COMPASS_MAG_CALIB_ACC_AUTO_REC_MODE 0
#define ALGO_CUST_MAG_CALIB_PROCESS_NOISE_1 0.009f
#define ALGO_CUST_MAG_CALIB_PROCESS_NOISE_2 0.009f
#define ALGO_CUST_MAG_CALIB_PROCESS_NOISE_3 0.009f
#define ALGO_CUST_MAG_CALIB_PROCESS_NOISE_4 0.081f
#define ALGO_CUST_COMPASS_ACC_FILT_ACC_BASE_COEF 0.4f
#define ALGO_CUST_COMPASS_ACC_FILT_ACC_DYN_COEF 0.1f


#endif

