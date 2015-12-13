#ifndef __BSXFUSIONLIBRARY_H__
#define __BSXFUSIONLIBRARY_H__
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
* @file      bsxfusionlibrary.h
* @date      2013/02/12 created
*
* @brief
* This file provides an interface to use the functionality of nine degree of freedom software library
*
* @detail
* bsxfusionlibrary - nine degree of freedom software library for the processing of the
* accelerometer, magnetometer and gyroscope sensor data.The library supports different
* operational modes of the sensors. Additionally, the library provides the virtual sensors
* like compass, imu, 9dof sensors for orientation processing, which uses the sensor fusion
* algorithm for estimation of the orientation processing or sensor calibration methods.
*
*/

/************************************************************************************************************/
/*												INCLUDES													*/
/************************************************************************************************************/

#include "BsxLibraryDataTypes.h"

/************************************************************************************************************/
/*											 GENERAL INTERFACE												*/
/************************************************************************************************************/

/*!
* @brief		Get version of the bsx Fusion Library.
* 				Version is a structure of four element which
* 				consists of major, minor, minorbugfix and majorbugfix.
* 			 	e.g. major=3, minor=0, minorbugfix =1 and
* 			  	majorbugfix = 0 implies its bsx 3.0.1.0
* @param 	* version: Pointer to version structure
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @Usage Guide	Call this API to get the version of
* 				the fusion library after initialization is done
*/
BSX_S8 bsx_get_version(ts_version*);

/*!
* @brief		This API initializes the main library process.
* 				If the input pointer is NULL then it will
* 				initialize with default values defined in the library.
*
* @param  		Inputparams->accelspec : Pointer to accelspec char array that holds settings for particular accel sensor
*				Inputparams->magspec : Pointer to magspec char array that holds settings for particular mag sensor
*				Inputparams->gyrospec : Pointer to gyrospec char array that holds settings for particular gyro sensor
*				Inputparams->usecaseconfig : Pointer to usecase char array that holds settings for particular usecase
*
*				Inputparams->accelspec_status holds the status if the spec is error free and not modified from original
*				0 imply error in accel spec char array
*				1 imply No error in accel spec array and spec corresponds to bma250
*				2 imply No error in accel spec array and spec corresponds to bma255
*				3 imply No error in accel spec array and spec corresponds to bma280
*
*				Inputparams->magspec_status holds the status if the spec is error free and not modified from original
*				0 imply error in mag spec char array
*				1 imply No error in mag spec array and spec corresponds to bmm050
*				2 imply No error in mag spec array and spec corresponds to bmm150
*
*				Inputparams->gyrospec_status holds the status if the spec is error free and not modified from original
*				0 imply error in gyro spec char array
*				1 imply No error in gyro spec array and spec corresponds to bmg160
*
*				Inputparams->usecase_status holds the status if the spec is error free and not modified from original
*				0 imply error in usecase char array
*				1 imply No error in usecase char array
*
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_init(initParam_t*);

/*!
* @brief    reset dynamic state of the library
* @param    none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset(void);

/*!
* @brief    Main library process - divided into three process running at different frequency - dopreprocess , docalibration and dousecase
*             dopreprocess - preprocessing sensor, calibration and usecase
*             docalibration - calibration of accel,mag,gyro,fmc
*             dousecase - COMPASS/M4G , IMU , NDOF processing
* @param    libraryinput_p -> pointer to sensor data
*       	structure which includes sensor data S32 type and
*         	 time stamp in microseconds of U64 type
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_dostep(libraryinput_t* );

/*!
* @brief    Pre Process Library (includes preprocessing of accel, mag, gyro,fmc, compass)
* @param    libraryinput_p -> pointer to sensor data
*       	structure which includes sensor data S32 type and
*         	 time stamp in microseconds of U64 type
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_dopreprocess(libraryinput_t*);

/*!
* @brief    Sensor Calibration Layer - (includes calibration of accel, mag, gyro, fmc when corresponding tick.calib is enabled)
* @param    None
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_docalibration(void);


/*!
* @brief    Sensor Usecase Layer - responsible for all use-case processing (COMPASS,M4G,IMU,NDOF)
* @param    None
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_dousecase(void);

/************************************************************************************************************/
/*											 ACCELEROMETER API												*/
/************************************************************************************************************/

/*!
* @brief 	Initialization of accelerometer modules with
*			the default values(BMA250) defined inside the library.
* @param 	Pointer to accelspec char array that holds settings for particular accel sensor
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_init_acc(BSX_U8*);

/*!
* @brief	Sets the measurement range for accelerometer
* @param	Measrange(unsigned char):measurement range of accelerometer.
*			Permissible values 0 -> 2G
*                              1 -> 4G
*                              2 -> 8G
*                              3 -> 16G
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_accrange(BSX_U8);

/*!
* @brief	Get the measurement range for accelerometer
* @param	*measrange: pointer to measurement range(unsigned char)
* 			of accelerometer
*			Output values 0 -> 2G
*                       1 -> 4G
*                       2 -> 8G
*                       3 -> 16G
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accrange(BSX_U8*);

/*!
* @brief	Set the calling datarate for the accelerometer
* @param	accdatarate(unsigned char): data rate of accelerometer
*			permissible values
*				0 -> 1Hz
*				1 -> 5Hz
*				2 -> 10Hz
*				3 -> 20Hz
*				4 -> 25Hz
*				5 -> 40Hz
*				6 -> 50Hz
*				7 -> 100Hz
*				8 -> 125Hz
*				9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_accdatarate(BSX_U8);

/*!
* @brief	Get the datarate for the accelerometer
* @param	accdatarate(*unsigned char): data rate of accelerometer
*			Output values
*				0 -> 1Hz
*				1 -> 5Hz
*				2 -> 10Hz
*				3 -> 20Hz
*				4 -> 25Hz
*				5 -> 40Hz
*				6 -> 50Hz
*				7 -> 100Hz
*				8 -> 125Hz
*				9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accdatarate(BSX_U8*);

/*!
* @brief	Set the accel calib accuracy threshold for the accelerometer
* @param	threshold	-> calib accuracy threshold of accelerometer
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_acccalib_accuracythreshold(BSX_U8);

/*!
* @brief	Set the accelerometer calibration profile(calib offset and status)
* @param	* calibprofile: Pointer to Accelerometer calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_acccalibprofile(ts_calibprofile*);

/*!
* @brief	Get the acceleromter calibration profile
* @param	* calibprofile: Pointer to Accelerometer calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_acccalibprofile(ts_calibprofile*);

/*!
* @brief	reset the accelerometer calibration module
* @param	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset_acccalib(void);

/************************************************************************************************************/
/*										PEDOMETER DATA INTERFACE										*/
/************************************************************************************************************/
/*!
* @brief	This API is used to set the pedometer stepdetection module
* @param	none
* @return   zero for enable, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_set_stepdetectionopmode(BSX_U8);

/*
* @brief This API is used to get the stepdetection opmode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
 */
BSX_S8  bsx_get_stepdetectionopmode(BSX_U8*);

/*
* @brief   This API is used to set the stepeventdetection opmode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
 */
BSX_S8  bsx_set_stepeventdetectionopmode(BSX_U8);

/*
* @brief   This API is used to get the stepeventdetection opmode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_get_stepeventdetectionopmode(BSX_U8*);

/* @brief   This API is used to set the stepdetectionrobustness opmode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*
*/
BSX_S8  bsx_set_stepdetectionrobustness(BSX_U8);

/*
* @brief    This API is used to get the stepdetectionrobustness opmode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*
*/
BSX_S8 bsx_get_stepdetectionrobustness(BSX_U8*);

/*
* @brief   This API used to get the status stepdetection
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*
*/
BSX_S8  bsx_get_stepdetectionstatus(BSX_U8*);

/*!
* @brief  	Get the stepcounter value from the algorithm
* @param  	*U64: it returns the value will be equal to 2^(64)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_get_stepcount(BSX_U64*);

/*!
 * @brief  This API is used to set the step detection mean constant
 *
 * @param   meanconst(BSX_F32  meanconst)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */

BSX_S8 bsx_set_stepdetectionmeanconst(BSX_F32 meanconst);

/*!
 * @brief  This API is used to get the step detection mean constant
 *
 * @param   meanconst(BSX_F32*  meanconst)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_get_stepdetectionmeanconst(BSX_F32* meanconst);

/*!
 * @brief  This API is used to set the step detection envmin variable
 *
 * @param   envmin(BSX_F32  envmin)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_set_stepdetectionenvmin(BSX_F32 envmin);

/*!
 * @brief  This API is used to get the step detection envmin variable
 *
 * @param   envmin(BSX_F32* envmin)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_get_stepdetectionenvmin(BSX_F32* envmin);

/*!
 * @brief  This API is used to set the step detection coefficient variable
 *
 * @param   coefficient(BSX_F32  coefficient)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_set_stepdetectioncoefficient(BSX_F32 coefficient);

/*!
 * @brief  This API is used to get the step detection coefficient variable
 *
 * @param   coefficient(BSX_F32* coefficient)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_get_stepdetectioncoefficient(BSX_F32* coefficient);

/*!
 * @brief  This API is used to set the step detection mean step frequency variable
 *
 * @param   meanstepfreq(BSX_F32  meanstepfreq)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_set_stepdetectionmeanstepfreq(BSX_F32 meanstepfreq);
/*!
 * @brief  This API is used to get the step detection mean step frequency variable
 *
 * @param   meanstepfreq(BSX_F32* meanstepfreq)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_get_stepdetectionmeanstepfreq(BSX_F32* meanstepfreq);

/*!
 * @brief  This API is used to set the step detection steps to buffer
 *
 * @param   nstepstobuff(BSX_U32  nstepstobuff)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_set_stepdetectionnstepstobuff(BSX_U32 nstepstobuff);

/*!
 * @brief  This API is used to get the step detection steps to buffer
 *
 * @param   nstepstobuff(BSX_U32*nstepstobuff)
 * @return  zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */
BSX_S8 bsx_get_stepdetectionnstepstobuff(BSX_U32* nstepstobuff);

/*!
* @brief  	Resets the stepcounter value to zero it will resets
*          the StepCount to initial vlaue as zero.
* @param   None
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset_stepcount(void);

/************************************************************************************************************/
/*										ACCELEROMETER DATA INTERFACE										*/
/************************************************************************************************************/

/*!
* @brief  	Get the raw accelerometer data(x,y and z direction) in m/s^2
* @param  	*rawaccdata: pointer to accelerometer raw data structure
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accrawdata(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected accelerometer data(x,y and z direction)
* 			in m/s^2. Corrected data = raw data – offset.
* @param  	accData 	-> accel data in ms^2
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_acccordata(ts_dataxyzf32*);


/*!
* @brief  	Get the corrected & filtered accelerometer data in ms^2
*			from digital filter 1
* @param  	*filtaccdata-> pointer to accelerometer  data structure
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accfiltdata1(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered accelerometer data in ms^2
*		    from digital filter 2
* @param  	*filtaccdata: pointer to accelerometer data structure
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accfiltdata2(ts_dataxyzf32*);

/*!
* @brief  	Get the estimated offsets of the accelerometer in m/s^2
* @param  	*offset: pointer to structure having estimated offsets
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accoffset(ts_dataxyzf32*);

/*!
* @brief  	Get the accelerometer calibration accuracy status
* @param  	*accuracyStatus -> current accelerometer calibration accuracy status
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_acccalibaccuracy(BSX_U8*);

/*!
* @brief  	Get the acc raw data in mg
* @param  	*rawdata_mg -> current accelerometer raw data in mg
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accrawdata_mg(ts_dataxyzf32* );

/*!
* @brief  	Get the acc cor data in mg
* @param  	*cordata_mg -> current accelerometer cor data in mg
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_acccordata_mg(ts_dataxyzf32* );

/*!
* @brief  	Get the acc filt data1 in mg
* @param  	*filtdata1_mg -> current accelerometer filter data1 in mg
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accfiltdata1_mg(ts_dataxyzf32* );

/*!
* @brief  	Get the acc filt data2 in mg
* @param  	*filtdata2_mg -> current accelerometer filter data2 in mg
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accfiltdata2_mg(ts_dataxyzf32* );

/*!
* @brief  	Get the acc offsets in mg
* @param  	*accoffsets_mg -> current accelerometer offsets in mg
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accoffsets_mg(ts_dataxyzs32* );

/************************************************************************************************************/
/*											 MAGNETOMETER API												*/
/************************************************************************************************************/

/*!
* @brief	Initialization of magnetometer modules with
* 			the default values(bmm050) defined inside the library.
* @param	magspec : Pointer to magspec char array that holds settings for particular mag sensor
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_init_mag(BSX_U8*);

/*!
* @brief	Set the calling datarate for the magnetometer
* @param	magdatarate(unsigned char)-> data rate of magnetometer
*			permissible values
*				0 -> 1Hz
*				1 -> 5Hz
*				2 -> 10Hz
*				3 -> 20Hz
*				4 -> 25Hz
*				5 -> 40Hz
*				6 -> 50Hz
*				7 -> 100Hz
*				8 -> 125Hz
*				9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_magdatarate(BSX_U8);

/*!
* @brief	Get the datarate for the magnetometer
* @param	magdatarate(*unsigned char)-> data rate of magnetometer
*			Output values
*				0 -> 1Hz
*				1 -> 5Hz
*				2 -> 10Hz
*				3 -> 20Hz
*				4 -> 25Hz
*				5 -> 40Hz
*				6 -> 50Hz
*				7 -> 100Hz
*				8 -> 125Hz
*				9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magdatarate(BSX_U8*);

/*!
* @brief	Set the magnetomter calibration profile(calib offset and status)
* @param	*calibprofile-> pointer to magnetometer calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_magcalibprofile(ts_calibprofile*);

/*!
* @brief	Get the magnetomter calibration profile
* @param	*calibprofile-> magnetometer calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magcalibprofile(ts_calibprofile*);

/*!
* @brief 	reset magnetometer calibration
* @param 	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset_magcalib(void);

/************************************************************************************************************/
/*										MAGNETOMETER CALIB INTERFACE								*/
/************************************************************************************************************/

/*!
* @brief 	set magnetometer calibration process Noise. This API
* 			should be invoked only on special request from the
* 			customer. It is expected to use in daemon for specific
* 			customer requirement only. Input noise parameters should
* 			be provided by system engineers only. Parameters will
* 			be provided if required to meet specific customer requirement.
* @param 	processNoise(*float)-> pointer to processNoise vector
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @UsageGuide	Call this API to  set magnetometer calibration process
* 				Noise works for usecase Compass and accelerometer
* 				and magnetometer should be enabled
*/
BSX_S8 bsx_set_magcalib_processnoise( BSX_F32 *);

/*!
* @brief 	Get magnetometer calibration process Noise. This
* 			API is invoked to get the process noise either
* 			default parameters or parameters set by using set
* 			API. It can be used to check the set parameters
* 			in algorithm.
* @param 	processNoise(*float)-> pointer to processNoise vector
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @UsageGuide	Call this API to  get magnetometer calibration process
* 				Noise works for usecase Compass and accelerometer
* 				and magnetometer should be enabled
*/
BSX_S8 bsx_get_magcalib_processnoise( BSX_F32 *);

/************************************************************************************************************/
/*									       SOFT IRON CORRECTION INTERFACE								    */
/************************************************************************************************************/

/*!
* @brief  	Used to set the sensitivity correction matrix for soft iron
* 			correction.this API should be used with correct matrix as
* 			input else algorithm will behave unexpectedly. SIC matrix
* 			is predetermined either from SIC app, customer provided
* 			or other methods based on customer platform.
* @param  	correctMatrix : correction matrix 3x3 (pointer to float
* 			array of 9 elements i.e. 3 by 3 matrix)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_softiron_correctionmatrix(ts_sensmatrix);

/*!
* @brief  	get the sensitivity correction matrix.
* 			This API can be used to retrieve the default
* 			SIC matrix (If it’s not set explicitly using set API)
* 			or set by using set API.
* @param  	*correctMatrix : pointer to correction matrix 3x3
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @UsageGuide	Call this API to  get the soft iron correction matrix.
* 				This API can be used to retrieve the default SIC matrix
* 				(If it’s not set explicitly using set API) or set by
* 				using set API.
*/
BSX_S8 bsx_get_softiron_correctionmatrix(ts_sensmatrix*);
/************************************************************************************************************/
/*									      ACCEL DYNAMIC FOR DISTORTION INTERFACE						    */
/************************************************************************************************************/

/*!
* @brief	Sets the accelerometer dynamic detection mode.
* 			This API is used to set the mode of distortion
* 			detection based on accelerometer dynamics. It
* 			can be made 0->1 when accel dynamics should be
* 			used for distortion detection and 1->0 when it
* 			is preferred not to use accel dynamics for distortion
* 			detection. This provision was provide for special
* 			usecase of coil testing when magnetic field is moved
* 			around and sensor is static, in this case the flag
* 			should be 1->0.
* @param	mode -> Enable/Disable mode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_accdyn_distdetmode(BSX_U8 );

/*!
* @brief	gets the accelerometer dynamic detection mode
* @param	mode -> pointer to mode (Enable/Disable mode status)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_accdyn_distdetmode(BSX_U8 *);
/************************************************************************************************************/
/*										MAGNETOMETER DATA INTERFACE											*/
/************************************************************************************************************/

/*!
* @brief 	Get the raw 3-axis magnetometer data in microtesla
* @param  	*rawmagdata	-> mag data in microtesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magrawdata(ts_dataxyzf32*);

/*!
* @brief  	Get the 3-axis corrected magnetometer data in MicroTesla
*			Corrected = raw – offset.
* @param  	*corMagData	-> mag data in microtesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magcordata(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis magnetometer data in microtesla
* 			from digital filter 1
* @param  	* p_filtmagdata: mag data in microtesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magfiltdata1(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis magnetometer data in microtesla
* 			from digital filter 2
* @param  	*filtmagdata: mag data in MicroTesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magfiltdata2(ts_dataxyzf32*);

/*!
* @brief  	Get the estimated parameter of the magnetometer
* @param 	offsets -> estimated offsets in microtesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magoffsets(ts_dataxyzf32*);

/*!
* @brief  	Get the magnetometer calibration accuracy status
* @param  	accuracyStatus 	-> current magnetometer calibration accuracy status
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_magcalibaccuracy(BSX_U8 *);

/*!
* @brief  	get the soft iron correction data.
* 			This API is the interface for magnetometer soft iron corrected(using SIC matrix) and uncalibrated data
* 			from generic data preprocessing. Data interface is only for ndof and compass module.
* @param  	*sicdata : pointer to corrected data
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @UsageGuide	Call this API to  get the soft iron corrected and uncalibrated mag data.
* 				(If it’s not set explicitly using set API) or set by
* 				using set API.
*/
BSX_S8 bsx_get_magsoftironcorrecteddata(ts_dataxyzf32* );

/*!
* @brief  	get the soft iron correction data.
* 			This API is the interface for magnetometer soft iron corrected(using SIC matrix) and uncalibrated data
* 			from generic data preprocessing. Data interface is only for ndof and compass module.
* @param  	*sicdata : pointer to corrected data microtesla
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @UsageGuide	Call this API to  get the soft iron corrected and uncalibrated mag data.
* 				(If it’s not set explicitly using set API) or set by
* 				using set API.
*/
BSX_S8 bsx_get_magsoftironcorrecteddata_lsb(ts_dataxyzf32*);

/************************************************************************************************************/
/*											 GYROSCOPE API													*/
/************************************************************************************************************/

/*!
* @brief	Initialization of gyroscope modules with the default values(bmg160)
* @param	gyrospec : Pointer to gyrospec char array that holds settings for particular gyro sensor
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_init_gyro(BSX_U8*);

/*!
* @brief	Set the measurement range for gyroscope
* @param	Measrange -> measurement range of gyroscope
*				Permissible Range
*					0 -> 2048 Dps
*					1 -> 2000 Dps
*					2 -> 1000 Dps
*					3 -> 500 Dps
*					4 -> 250 Dps
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_gyrorange(BSX_U8);

/*!
* @brief	Get the measurement range for gyroscope
* @param	*measrange -> pointer to measurement range of gyroscope
*				Output Range
*					0 -> 2048 Dps
*					1 -> 2000 Dps
*					2 -> 1000 Dps
*					3 -> 500 Dps
*					4 -> 250 Dps
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrorange(BSX_U8*);

/*!
* @brief	Set the calling datarate for the gyroscope
* @param	datarate-> datarate of gyroscope
*				permissible values
*					0 -> 1Hz
*					1 -> 5Hz
*					2 -> 10Hz
*					3 -> 20Hz
*					4 -> 25Hz
*					5 -> 40Hz
*					6 -> 50Hz
*					7 -> 100Hz
*					8 -> 125Hz
*					9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_gyrodatarate(BSX_U8);

/*!
* @brief	Get the calling datarate for the gyroscope
* @param	*datarate-> pointer to datarate of gyroscope
*				permissible values
*					0 -> 1Hz
*					1 -> 5Hz
*					2 -> 10Hz
*					3 -> 20Hz
*					4 -> 25Hz
*					5 -> 40Hz
*					6 -> 50Hz
*					7 -> 100Hz
*					8 -> 125Hz
*					9 -> 200Hz
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrodatarate(BSX_U8*);

/*!
* @brief	Set the gyroscope calibration profile
* @param	calibprofile-> gyroscope calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_gyrocalibprofile(ts_calibprofile*);

/*!
* @brief	Get the gyroscope calibration profile
* @param	*calibprofile-> gyroscope calibration profile
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrocalibprofile(ts_calibprofile*);

/*!
* @brief 	reset gyroscope calibration
* @param 	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset_gyrocalib(void);

/************************************************************************************************************/
/*										GYROSCOPE DATA INTERFACE											*/
/************************************************************************************************************/

/*!
* @brief  	Get the raw 3-axis gryoscope data in degrees/sec
* @param  	gyroData 	-> gyro data in degrees/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrorawdata_dps(ts_dataxyzf32*);

/*!
* @brief  	Get the raw 3-axis gryoscope data in radians/sec
* @param  	gyroData 	-> gyro data in radians/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrorawdata_rps(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected 3-axis gryoscope data in degrees/sec
* @param  	gyroData 	-> gyro data in degrees/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrocordata_dps(ts_dataxyzf32*);

/*!
* @brief  	Get the raw 3-axis gryoscope data in radians/sec
* @param  	gyroData 	-> gyro data in radians/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrocordata_rps(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis gyroscope data in degrees/sec
*			from digital filter 1
* @param  	gyroData 	-> gyro data in degrees/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrofiltdata1_dps(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis gyroscope data in radians/sec
*			from digital filter 1
* @param  	gyroData 	-> gyro data in radians/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrofiltdata1_rps(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis gyroscope data in degrees/sec
*			from digital filter 2
* @param  	gyroData 	-> gyro data in degrees/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrofiltdata2_dps(ts_dataxyzf32*);

/*!
* @brief  	Get the corrected & filtered 3-axis gyroscope data in radians/sec
*			from digital filter 2
* @param  	gyroData 	-> gyro data in radians/sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrofiltdata2_rps(ts_dataxyzf32*);

/*!
* @brief  	Get the estimated parameter of the gyroscope
* @param  	offsets 	-> estimated offsets in degrees per sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrooffset_dps(ts_dataxyzf32*);

/*!
* @brief  	Get the estimated parameter of the gyroscope
* @param  	offsets 	-> estimated offsets in radians per sec
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrooffset_rps(ts_dataxyzf32*);

/*!
* @brief  	Get the gyroscope calibration accuracy status
* @param  	*gyrocalibaccuracy 	-> current gyroscope calibration accuracy status
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gyrocalibaccuracy(BSX_U8*);

/************************************************************************************************************/
/*										ORIENTATION PROCESSING API												*/
/************************************************************************************************************/

/*!
* @brief	Initialization of orientation modules
*			For compass mode -> compass module is initialized
*			For IMU or NDOF mode -> NDOF module
*			For ALL mode -> compass and NDOF module
* @param	usecaseconfig : Pointer to usecase char array that holds settings for particular usecase
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_init_orient(BSX_U8*);

/*!
* @brief	Reset of Orientation module
* @param	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_reset_orient(void);

/*!
* @brief	Restart of Orientation module
* @param	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_restart_orient(void);

/*!
* @brief	Heading base threshold for calcuting accuracy status
* @param	none
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_HeadingAccuracyBaseThresh(BSX_F32 );
/************************************************************************************************************/
/*										ORIENTATION DATA INTERFACE											*/
/************************************************************************************************************/

/*!
* @brief  	Get the orientation quaternion data
* @param  	*quatData -> quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientdata_quat(ts_dataquatf32 *);

/*!
* @brief  	Get the orientation quaternion data in lsb(scaling factor 2^14=16384)
* @param  	quatData 	->  quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientdata_quat_lsb(ts_dataquat *);

/*!
* @brief  	Get the orientation euler data(heading, pitch and roll) in degrees
* @param  	*eulerData -> euler data (h,p,r)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientdata_euler_deg(ts_dataeulerf32 *);

/*!
* @brief  	Get the orientation euler data(heading, pitch and roll) in radians
* @param  	eulerData 	-> euler data (h,p,r)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientdata_euler_rad(ts_dataeulerf32 *);

/*!
* @brief  	Get the orientation data accuracy status
* @param  	accuracyStatus 	-> orientation accuracy status
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orient_datastatus(BSX_U8*);

/*!
* @brief  	Get the imu orientation quaternion data
* @param  	*quatData -> quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_imuorientdata_quat(ts_dataquatf32 *);


/*!
* @brief  	Get the imu orientation quaternion data in lsb
* @param  	*quatData -> quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_imuorientdata_quat_lsb(ts_dataquat *);

/*!
* @brief  	Get the imu orientation euler data(heading, pitch and roll) in degrees
* @param  	*eulerData -> euler data (h,p,r)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_imuorientdata_euler_deg(ts_dataeulerf32 *);

/*!
* @brief  	Get the imu orientation euler data(heading, pitch and roll) in radians
* @param  	eulerData 	-> euler data (h,p,r)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_imuorientdata_euler_rad(ts_dataeulerf32 *);

/*!
* @brief  	Get the orientation data accuracy status
* @param  	accuracyStatus 	-> orientation accuracy status
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orient_datastatus( BSX_U8* );

/*!
* @brief  	Get the heading data accuracy in radians
* @brief    Heading status refers to accuracy level of orientation data(from library) from
* @brief	true heading calculation. This comparison is done with magnetic data coupling.
* @param 	*headingaccrad -> Pointer to read the error in heading in radians
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_headingaccuracy_rad( BSX_F32* );

/*!
* @brief  	Get the heading data accuracy in degree
* @brief    Heading status refers to accuracy level of orientation data(from library) from
* @brief	true heading calculation. This comparison is done with magnetic data coupling.
* @param 	*headingaccdeg -> Pointer to read the error in heading in degrees
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_headingaccuracy_deg( BSX_F32* );

/************************************************************************************************************/
/*									COMPASS CONFIG  INTERFACE    											*/
/************************************************************************************************************/
/*!
* @brief	Get the filter mode for compass and M4G
* @param	*mode -> filter mode for compass
* 				Output values
*					0 -> Bypass
*					1 -> Low
*					2 -> Medium
*					3 -> High
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_compass_filtermode(BSX_U8*);

/*!
* @brief	Set the acc noise threshold for compass and M4G
* @param	accnoisethresh	-> accel noise threshold
* @param	magnoisethresh	-> mag noise threshold
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_compass_magnoise( BSX_F32);
BSX_S8 bsx_set_compass_magfilterparam(BSX_F32,BSX_F32);


/**!
* @brief	Set the acc noise threshold for compass and M4G
* @param	accnoisethresh	-> accel noise threshold
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_compass_accel_filternoise(BSX_F32);

/*!
* @brief	Get the acc noise threshold for compass and M4G
* @param	accnoisethresh	-> accel noise threshold
* @param	magnoisethresh	-> mag noise threshold
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_compass_filternoise(BSX_F32*,BSX_F32*);

/*!
* @brief	Set the filter noise parameters, the filter params
* 			are generated by filed engineer.
* @param	basecoef	-> base coefficient
* @param	dyncoef		-> dynamic coefficient
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
//BSX_S8 bsx_set_compass_filterparam(BSX_F32, BSX_F32);

/*!
* @brief	Set the accel filter noise parameters, the filter params
* 			are generated by filed engineer.
* @param	accelbasecoef		-> accel base coefficient
* @param	acceldyncoef		-> accel dynamic coefficient
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_compass_accel_filterparam(BSX_F32, BSX_F32);

/*!
* @brief	Get the filter noise param for compass and M4G
* @param	basecoef	-> base coefficient
*			dyncoef		-> dynamic coefficient
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_compass_filterparam(BSX_F32*, BSX_F32*,BSX_F32*, BSX_F32*);

//CR_19 changes
/*!
* @brief	Set the accel dynamic threshold for  M4G
* @param	accdynthres	-> accel dynamic threshold
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_m4g_accdynthreshold( BSX_U8);

/*!
* @brief	Get the accel dynamic threshold for  M4G
* @param	accdynthres	-> accel dynamic threshold
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_m4g_accdynthreshold( BSX_U8*);
/************************************************************************************************************/
/*										GRAVITY VECTOR DATA INTERFACE										*/
/************************************************************************************************************/

/*!
* @brief  	Get the gravity vector data in ms^2
* @param  	*gravityVectorData -> gravity vector data in ms^2
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_gravity(ts_dataxyzf32 *);

/************************************************************************************************************/
/*									 LINEAR ACCELERATION DATA INTERFACE										*/
/************************************************************************************************************/

/*!
* @brief  	Get the linear acceleration data in ms^2
* @param  	linearAcc 	-> linear accel data in ms^2
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_linearacceleration(ts_dataxyzf32 *);
/************************************************************************************************************/
/*									       POWER MODES INTERFACE										    */
/************************************************************************************************************/


/*
* @brief 	Set the working opmode. Input is a structure of one
* 			element of BSX_U32 type and contains encoded opmode.
*			  e.g.
*        		workingModeMagOnly   =98308;
*        		workingModeAccMag    =98309;
*        		workingModeAccGyro   =524305;
*        		workingModeImuPlus   =525329;
* @param 	ts_workingModesStruct* - Pointer to working mode
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_workingmode(ts_workingModes *);

/*!
* @brief 	Get the sensor switch list for the current working mode.
* 			HWsensorSwitchList  is a structure of three elements
* 			BSX_U8 acc,BSX_U8 mag,BSX_U8 gyro.This function gets
* 			the status of these three alements for the
* 			given working mode.e.g. if working mode
* 			is BSX_WORKINGMODE_MAGONLY then acc = 0,mag = 1,gyro = 0;
* @param 	* workingModes : Pointer to working mode constants
*			* HWsensorSwitchList : Pointer to hardware switch list
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @Usage Guide	Call this API to get sensor state based on working modes
*/
BSX_S8 bsx_get_hwdependency(ts_workingModes,ts_HWsensorSwitchList* );

/************************************************************************************************************/
/*										  Significant Motion API												*/
/************************************************************************************************************/

/*!
* @brief 	This API is used to set the significant motion opmode
* @param	mode(unsigned short)
*				mode = 1 activate significant motion
*				mode >=0 deactivate significant motion
*			when mode is set to 1, the significant motion
*			initializes to Initial values
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_set_significantmotiondetectionopmode(BSX_U16);

/*!
* @brief 	This API is used to get the significant motion opmode
* @param	*mode(pointer to unsigned short)
*				mode = 1 significant motion active
*				mode >=0 significant motion inactive
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_get_significantmotiondetectionopmode(BSX_U16*);

/*!
* @brief 	This API is used to get the significant motion detection status
* 			once the motion is detected the significant motion
* 			is deactivated. algorithm can be activated by calling set
* 			opmode API.
* @param	*status(pointer to unsigned short)
*				status = 1 motion detected
*				status >=0 no motion detected
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8  bsx_get_significantmotiondetectionstatus(BSX_U16*);

/************************************************************************************************************/
/*										  FLIP GESTURE API												*/
/************************************************************************************************************/

/*!
* @brief 	returns flip gesture opmode
* @param	*Opmode(pointer to unsigned char)
*				Mode = 0 implies flip gesture mode is off
*				Mode >=1 implies flip gesture mode is On
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_flipgesture_opmode(BSX_U8* );

/*!
* @brief 	sets flip gesture opmode
* @param	Opmode(pointer to unsigned char)
*				Mode = 0 implies flip gesture mode is off
*				Mode >=1 implies flip gesture mode is On
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
* @usage guide 	Call this API to set flip gesture opmode. works
* 				for all usecase(Compass,IMU,M4G,NDOF) and accelerometer
* 				should be enabled. To get the flip gesture outputs,
* 				flip gesture opmode should be enabled. Flipgesture
* 				opmode will be off by default.
*/
BSX_S8 bsx_set_flipgesture_opmode(BSX_U8 );

/*!
* @brief 	Gets the threshold of faceUp and faceDown angle
* @param	faceup : Angle for faceup
*			facedown:Angle for facedown
*			expected values
*			100 < faceup <170
*			10 < facedown < 80
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_flipgesture_angle(BSX_F32* ,BSX_F32* );

/*!
* @brief 	sets flip gesture angle
* @param	faceup : Angle for faceup
*			facedown:Angle for facedown
*			Range ->values should be between specified min and max,
*			else values wont be set
*				 FACEUP_MIN      100
*				 FACEUP_MAX      170
*				 FACEDOWN_MIN    10
*				 FACEDOWN_MAX    80
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_flipgesture_angle(BSX_F32 ,BSX_F32 );

/*!
* @brief 	Get the detect time in mili seconds for flip gesture module
* @params	Time : Detection time
*			300 ms < Time < 2000 ms
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_flipgesture_detecttime(BSX_U16* );

/*!
* @brief 	To set the detect time for flip gesture module in milliseconds
* @params	Time : Detection time
*			300 ms < Time < 2000 ms
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_flipgesture_detecttime(BSX_U16 );

/*!
* @brief 	calibration tick
* @params	Status:
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_calibrationcalltick(BSX_U8* );

/*!
* @brief 	Usecase tick
* @params	Status:
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_usecasecalltick(BSX_U8* );

/*!
* @brief 	returns flip gesture status
* @params	Status: Detection Status
*				status = 0 implies facing is unknown
*				status = 1 implies facing Up
*				status = 2 implies facing Down
*				status = 3 implies facing Neutral
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_flipgesture_status(BSX_U8* );

/*!
* @brief	Read orient coordinate system - to get the co-ordinate system (WIN 8 or Android) which has been set already
* @param	*coordinatesys -> coordinate systems
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientcoordinatesystem(BSX_U8* );

/*!
* @brief	Sets orient coordinate system ( this api sets co - ordinate system corresponding to win 8 or android that has been passed as argument)
* @param	coordinatesys -> coordinate systems
* coordinatesys -1 -> ANDROID (default)
* coordinatesys -2 -> WIN8
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_orientcoordinatesystem(BSX_U8 );

/*!
* @brief  	Get rotation matrix
* @param  	*rotmatrix 	-> orientation rotation matrix
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_orientrotationmatrix( BSX_F32* );

/*!
 * @brief calcualtes orient accuracy from accel ,mag and gyro status
 * @params accel status, magStatus, gyroStatus
 * @return   zero for success, non-zero failed
 * @retval  0 -> Success
 * @retval	1 -> Error
 */

BSX_U8 bsx_calcorientaccuracy(BSX_U8,BSX_U8,BSX_U8);

/*!
* @brief  	 Get the geo magnetic rotationvector
* @param  	*georotationquat -> Orientation quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_georotationvector_quat(ts_dataquatf32*);

/*!
* @brief  	Get the geo magnetic rotation vector heading data accuracy in radians
* @brief    Heading status refers to accuracy level of orientation data(from library) from
* @brief	true heading calculation. This comparison is done with magnetic data coupling.
* @param 	*headingaccrad -> Pointer to read the error in heading in radians
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_geoheadingaccuracy_rad( BSX_F32*);

/*!
* @brief  	Get the geo magnetic rotation vector heading data accuracy in Degrees
* @brief    Heading status refers to accuracy level of orientation data(from library) from
* @brief	true heading calculation. This comparison is done with magnetic data coupling.
* @param 	*headingaccrad -> Pointer to read the error in heading in Degrees
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_geoheadingaccuracy_deg( BSX_F32*);

/************************************************************************************************************/
/*										  PRE-FILTER PARAMETERS API										    */
/************************************************************************************************************/


/*!
* @brief	Set the heading sensitivity for compass and M4G
* @param	sensitivity -> sensitivity mode for compass
* 				Permissible values
*					0 - 5: sensitivity mode values of 0 to 5 is defined
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_compass_headingsensitivity(BSX_U8);

/*!
* @brief	Set the orientation correction speed for NDOF
* @param	correction speed -> orientation correction speed for NDOF
* 				Permissible values
*					0 - 5: correction speed values of 0 to 5 is defined
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_ndof_orientcorrectionspeed(BSX_U8);

/*!
* @brief	Set the Magnetic data filter mode
* @param	mode -> mode for magnetic filter
* 				Permissible values
*					0 - 5: sensitivity mode values of 0 to 5 is defined
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_magfiltermode(BSX_U8);

/*!
* @brief	Set the Magnetic calibration accuracy status sensitivity
* @param	sensitivity -> sensitivity mode for magnetic calibration accuracy status
* 				Permissible values
*					0 - 5: sensitivity mode values of 0 to 5 is defined
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_magcalibacc_sensitivity(BSX_U8);

/*!
* @brief	Set the Magnetic calibration speed
* @param	calibration speed -> magnetic calibration speed
* 				Permissible values
*					0 - 5: correction speed values of 0 to 5 is defined
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_magcalibrationspeed(BSX_U8);

/*!
* @brief	Set the Magnetic calibration accuracy auto recovery mode
* @param	mode -> auto recovery mode for magnetic calibration accuracy
* 				Permissible values [0,1]
*					0: disable
*					1: Enable
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_set_compass_magcalibaccautorecmode(BSX_U8);

/*!
* @brief  	 Get the geo magnetic rotationvector values in lsb
* @param  	*georotationquat -> Orientation quaternion data (w,x,y,z)
* @return   zero for success, non-zero failed
* @retval   0 -> Success
* @retval	1 -> Error
*/
BSX_S8 bsx_get_georotationvector_quat_lsb(ts_dataquat*);

#endif

