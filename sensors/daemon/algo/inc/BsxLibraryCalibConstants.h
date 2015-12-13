#ifndef __BSXLIBRARYCALIBCONSTANTS_H__
#define __BSXLIBRARYCALIBCONSTANTS_H__
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
* @file      bsxlibrarycalibconstants.h
* @date      2013/02/12 created
*
* @brief
* This file provides constants definition for calibration modules
*
* @detail
* bsxlibrarycalibconstants - file provides constants definition used for calibration modules
*
*/

/************************************************************************************************************/
/*												INCLUDES													*/
/************************************************************************************************************/

#include "BsxLibraryDataTypes.h"

/************************************************************************************************************/
/*											CONSTANT DEFINITIONS										 	*/
/************************************************************************************************************/

/** \def calibration modes */
#define BSX_CALIB_SLEEP     			(0U)
#define BSX_CALIB_MONITORING  			(1U)
#define BSX_CALIB_MODEOFFSETACTIVE		(2U)

/** \def data correction modes */
#define BSX_DATACORRECTION_SLEEP		(0U)       /** \def 0 = sleep */
#define BSX_DATACORRECTION_OFFSET		(1U)		/** \def 1 = offset correction */
#define BSX_DATACORRECTION_SENSDOFFSET	(2U)       /** \def 2 = offset + diagonal sensitivity correction */
#define BSX_DATACORRECTION_SENSFOFFSET	(3U)       /** \def 3 = offset + full sensitivity correction */

/** \def calibration modes */
#define BSX_CALIBSOURCE_NONE     		(0U)
#define BSX_CALIBSOURCE_CLASSIC  		(1U)
#define BSX_CALIBSOURCE_FAST			(2U)

/** \def gyro calibration modes */
#define BSX_GYROCALIB_SLEEP     		(0U)      /** \def 0= Not Active */
#define BSX_GYROCALIB_GYRODATA			(1U)      /** \ def 1=Gyroscope data only */
#define BSX_GYROCALIB_GYROACC  			(2U)      /** \ def 2=Gyroscope+Accel Data */
#define BSX_GYROCALIB_GYROACCMAG		(3U)      /** \ def 3= Gyroscope+Accel+Mag Data */

#endif






































