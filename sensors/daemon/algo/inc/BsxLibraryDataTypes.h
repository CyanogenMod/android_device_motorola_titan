#ifndef __BSXLIBRARYDATATYPES_H__
#define __BSXLIBRARYDATATYPES_H__
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
* @file      bsxlibrarydatatypes.h
* @date      2013/02/12 created
*
* @brief
* This file provides datatypes
*
* @detail
* bsxlibrarydatatypes - file provides typedef datatypes used by library
*
*/

/************************************************************************************************************/
/*												INCLUDES													*/
/************************************************************************************************************/

#include <limits.h>
#include "BsxLibraryConstants.h"
#include "BsxLibraryCalibConstants.h"
#include "BsxLibraryErrorConstants.h"
#include <stdlib.h>
#include <stdint.h>

/************************************************************************************************************/
/*												TYPE DEFINITIONS										 	*/
/************************************************************************************************************/

/** \def Basic Datatypes */
#if __STDC_VERSION__ >= 199901L
  /** C99 standard */
    typedef int16_t			BSX_S16;
    typedef	uint16_t		BSX_U16;
    typedef int32_t		    BSX_S32;
    typedef uint32_t	    BSX_U32;

    typedef int8_t			BSX_S8;
    typedef uint8_t			BSX_U8;
    typedef int64_t			BSX_S64;
    typedef uint64_t		BSX_U64;
    typedef uint8_t			BSX_BIT;
    typedef uint32_t		BSX_BOOL;
#else
    /** C89 standard */
    /**  find correct data type for signed 16 bit variables */
#if USHRT_MAX == 0xFFFF
    typedef unsigned short BSX_U16; 			/**< 16 bit achieved with short */
    typedef signed short BSX_S16;
#elif UINT_MAX == 0xFFFF
    typedef unsigned int BSX_U16;  				/**< 16 bit achieved with int */
    typedef signed int BSX_S16;
#else
#error U16 and S16 could not be defined automatically, please do so manually
#endif
    /* find correct data type for signed 32 bit variables */
#if INT_MAX == 0x7FFFFFFF
    typedef signed int BSX_S32; 				/**< 32 bit achieved with int */
    typedef unsigned int BSX_U32;
#elif LONG_MAX == 0x7FFFFFFF
    typedef signed long int BSX_S32;       		/**< 32 bit achieved with long int */
    typedef unsigned long int BSX_U32; 			/**< 32 bit achieved with long int */
#else
#error S32 could not be defined automatically, please do so manually
#endif
    typedef signed char					BSX_S8;
    typedef unsigned char				BSX_U8;
    typedef signed long long			BSX_S64;
    typedef unsigned long long			BSX_U64;
    typedef unsigned char				BSX_BIT;
    typedef unsigned int				BSX_BOOL;
#endif

/** \def Basic Datatypes */

typedef float						BSX_F32;
typedef double						BSX_F64;


/** \def Basic numbers in use */
#define	BSX_NULL					(BSX_U8)0
#define BSX_ZERO					(0U)
#define BSX_ONE						(1U)
#define BSX_TWO						(2U)
#define BSX_THREE					(3U)
#define BSX_FOUR					(4U)
#define BSX_FIVE					(5U)
#define BSX_SIX						(6U)
#define BSX_SEVEN					(7U)
#define BSX_EIGHT					(8U)
#define BSX_NINE					(9U)
#define BSX_TEN						(10U)
#define BSX_ELEVEN					(11U)
#define BSX_TWELVE					(12U)
#define BSX_SIXTEEN					(16U)
#define BSX_TWENTYFOUR				(24U)
#define BSX_THIRTYTWO				(32U)
#define BSX_SIXTYFOUR				(64U)
#define BSX_HUNDRED					(100)
#define BSX_ONETWENTYSEVEN			(127)
#define BSX_TWOFIFTYFIVE			(255)
#define BSX_TWOFIFTYSIX				(256)

#define BSX_ZERO_S					(0)
#define BSX_ONE_S					(1)
#define BSX_TWO_S					(2)
#define BSX_THREE_S					(3)
#define BSX_FOUR_S					(4)
#define BSX_FIVE_S					(5)
#define BSX_SIX_S					(6)
#define BSX_SEVEN_S					(7)
#define BSX_EIGHT_S					(8)
#define BSX_NINE_S					(9)
#define BSX_TEN_S					(10)
#define BSX_ELEVEN_S				(11)
#define BSX_TWELVE_S				(12)
#define BSX_SIXTEEN_S				(16)
#define BSX_TWENTYFOUR_S			(24)
#define BSX_THIRTYTWO_S				(32)
#define BSX_SIXTYFOUR_S				(64)
#define BSX_HUNDRED_S				(100)
#define BSX_ONETWENTYSEVEN_S		(127)
#define BSX_TWOFIFTYFIVE_S			(255)
#define BSX_TWOFIFTYSIX_S			(256)


#define BSX_ZERO_F					(0.0f)
#define BSX_ONE_F					(1.0f)
#define BSX_TWO_F					(2.0f)
#define BSX_THREE_F					(3.0f)
#define BSX_FOUR_F					(4.0f)
#define BSX_FIVE_F					(5.0f)
#define BSX_SIX_F					(6.0f)
#define BSX_SEVEN_F					(7.0f)
#define BSX_EIGHT_F					(8.0f)
#define BSX_NINE_F					(9.0f)
#define BSX_TEN_F					(10.0f)
#define BSX_ELEVEN_F				(11.0f)
#define BSX_TWELVE_F				(12.0f)
#define BSX_SIXTEEN_F				(16.0f)
#define BSX_TWENTYFOUR_F			(24.0f)
#define BSX_THIRTYTWO_F				(32.0f)
#define BSX_SIXTYFOUR_F				(64.0f)
#define BSX_HUNDRED_F				(100.0f)
#define BSX_ONETWENTYSEVEN_F		(127.0f)
#define BSX_TWOFIFTYFIVE_F			(255.0f)
#define BSX_TWOFIFTYSIX_F			(256.0f)

/** \def Standard Conversion Factors */
#define BSX_CONVFACTOR_UNIT2MILLI	(1e3f)		/**< unit to milli */
#define BSX_CONVFACTOR_UNIT2MICRO	(1e6f)		/**< unit to micro */
#define BSX_CONVFACTOR_UNIT2NANO	(1e9f)		/**< unit to nano  */
#define BSX_CONVFACTOR_RAD2DEG		(57.295647f)  /**< radiant to degree conversion factor (180/3.1416)*/
#define BSX_CONVFACTOR_DEG2RAD		(0.0175f)		/**< degree to radiant conversion factor (3.1416/180)*/
#define BSX_NORMFACTOR				(0.008726646259722f) /**pi/(180*2)*/
#define BSX_CONVERSIONFACTOR		(0.008726666666667f) /**(BSX_PI/(180*2))*/

/* \def Standard Values */
#define BSX_PI						(3.1416f)		/**< Ratio of circle's circumference to its diameter */
#define C_LSB2uT_F32X				(0.25f)
#define C_uT2LSB_F32X				(4.0f)
/* \def Basic Status/Controls */
#define BSX_DEACTIVATE				(0U)
#define BSX_ACTIVATE				(1U)

#define BSX_FAILURE					(0U)
#define BSX_SUCCESS					(1U)

#define BSX_DISABLE					(0U)
#define BSX_ENABLE					(1U)

#define BSX_FALSE					(0U)
#define BSX_TRUE					(1U)

#define BSX_RESET					(0U)
#define BSX_SET						(1U)

/************************************************************************************************************/
/*												STRUCTURES DEFINITION										*/
/************************************************************************************************************/
/** \struct ts_Version
        \brief version format
*/
typedef struct
{
	BSX_S16 major;		/**< major version */
	BSX_S16 minor;		/**< minor version */
	BSX_S16 majorbugFix;		/**< major bux fix version */
	BSX_S16 minorbugFix;		/**< minor bux fix version */
}ts_version;

/** \struct ts_dataxyz
        \brief 3-axis data as S16
*/
typedef struct
{
	BSX_S16 x;			/**< x-axis data */
	BSX_S16 y;			/**< y-axis data */
	BSX_S16 z;			/**< z-axis data */
}ts_dataxyz;

/** \struct ts_dataxyz
        \brief 3-axis data as S16
*/
typedef struct
{
	BSX_S32 x;			/**< x-axis data */
	BSX_S32 y;			/**< y-axis data */
	BSX_S32 z;			/**< z-axis data */
}ts_dataxyzs32;

/** \struct ts_dataxyzu8
        \brief 3-axis data as U8
*/
typedef struct
{
	BSX_U8 x;			/**< x-axis data  */
	BSX_U8 y;			/**< y-axis data  */
	BSX_U8 z;			/**< z-axis data  */
}ts_dataxyzu8;

/** \struct ts_dataxyzf32
        \brief 3-axis data as F32
*/
typedef struct{
	BSX_F32 x;			/**< x-axis data  */
	BSX_F32 y;			/**< y-axis data  */
	BSX_F32 z;			/**< z-axis data  */
}ts_dataxyzf32;

/** \struct ts_dataeuler
        \brief Euler angles data as S16
*/
typedef struct
{
     BSX_S16 h;			/**< heading data */
     BSX_S16 p;			/**< pitch data  */    
     BSX_S16 r;			/**< roll data  */
     BSX_S16 y;			/**< 3D Yaw data  */
}ts_dataeuler;

/** \struct ts_dataquat
        \brief Quaternion angles data
*/
typedef struct
{
	BSX_S16  w;			/**< q[0] data of vector q  */
	BSX_S16  x;			/**< q[1] data of vector q  */
	BSX_S16  y;			/**< q[2] data of vector q  */
	BSX_S16  z;			/**< q[3] data of vector q  */
}ts_dataquat;

/** \struct ts_dataeulerf32
        \brief Euler data
*/
typedef struct
{
     BSX_F32 h;			/**< heading data */
     BSX_F32 p;			/**< pitch data  */       
     BSX_F32 r;			/**< roll data  */
     BSX_F32 y;			/**< 3D Yaw data  */
}ts_dataeulerf32;

/** \struct ts_dataquatf32
        \brief Quaternion data
*/
typedef struct
{
	BSX_F32  w;			/**< q[0] data of vector q  */
	BSX_F32  x;			/**< q[1] data of vector q  */
	BSX_F32  y;			/**< q[2] data of vector q  */
	BSX_F32  z;			/**< q[3] data of vector q  */
}ts_dataquatf32;

/** \struct ts_SensM
        \brief 3x3 sensitivity matrix
*/

typedef struct
{
	ts_dataxyzf32 x;	/**< x-axis sensitivity with respect to 3-axis */
	ts_dataxyzf32 y;	/**< y-axis sensitivity with respect to 3-axis */
	ts_dataxyzf32 z;	/**< z-axis sensitivity with respect to 3-axis */
}ts_sensmatrix;

/** \struct ts_axis
        \brief axis Configuration
*/
typedef struct
{
	BSX_S32 update_remaparray[3*3] ;
	BSX_U8 axisConfig;
	BSX_U8 axisSign;

}ts_axisconfig;


/** \struct ts_calibparam
        \brief calibration parameters 
*/
typedef struct
{
	ts_dataxyz  offset; /** 3-axis offset */
	BSX_S16		radius; /** radius */
}ts_calibparam;


/** \struct ts_tick
        \brief selection for calib or usecase mode
*/
typedef struct
{
	BSX_U8  calib; /** enable when calib module is set */
	BSX_U8	usecase; /** enable when usecase module is set */
}ts_tick;

/** \struct ts_callflag
        \brief selection for calib or usecase mode
*/
typedef struct
{
	BSX_U8  acccalib; /** select for acc calib module */
	BSX_U8	magcalib; /** select for mag calib module */
	BSX_U8	gyrocalib; /** select for mag calib module */
	BSX_U8	usecase; /** select for usecase module */
}ts_callflag;

/** \struct ts_calibparam
        \brief calibration parameters 
*/
typedef struct
{
	ts_dataxyzf32  offset;	/** 3-axis offset */
	BSX_F32		radius;		/** radius */
}ts_calibparamf32;

/** \struct ts_calibparam
        \brief calibration parameters 
*/
typedef struct
{
	ts_calibparam calibParam;
	BSX_U8	accuracy;


}ts_calibprofile;

/** \struct ts_bsxfusionlibrary
        \brief General Library Struct 
*/
typedef struct
{
	BSX_U32 workingModes;
	BSX_U32 prev_workingMode;
	BSX_U8 opMode;						/**< bsx library operation mode of the library: bit[0] = accelerometer mode; bit[1] = magnetometer mode; bit[2] = gyroscope mode;*/
	BSX_U8 flipgestureOpmode;
	BSX_U8 accFlipGesture;
	BSX_U8 orientcoordinatesystem;


}ts_bsxfusionlibrary;

/** \struct ts_workingModes
        \brief Working Modes
*/
typedef struct
{
	BSX_U32 opMode;						/**< working operation modes set*/
	/*Reserved*/
}ts_workingModes;

/** \struct ts_HWsensorSwitchList
        \brief HW sensor Switch List
*/
typedef struct
{
	BSX_U8 acc;
	BSX_U8 mag;
	BSX_U8 gyro;
}ts_HWsensorSwitchList;

/** \struct sensordata_t
        \brief sensor Data
*/
typedef struct {
	ts_dataxyzs32 data;
	BSX_U32 time_stamp;
}sensordata_t;

/** \struct libraryinput_t
        \brief Library input
*/
typedef struct {
	sensordata_t acc;
	sensordata_t mag;
	sensordata_t gyro;
} libraryinput_t;

/*!
 * @brief struct flip_t
 * Vector of face up and face down
 */
typedef struct
{
	 BSX_F32 faceup;      /**< holds the faceup angle */
	 BSX_F32 facedown;    /**< holds the facedown angle */
}flip_t;

typedef struct {
	BSX_U8 *accelspec;
	BSX_U8 *magspec;
	BSX_U8 *gyrospec;
	BSX_U8 *usecase;
	BSX_U8 accelspec_status;
	BSX_U8 magspec_status;
	BSX_U8 gyrospec_status;
	BSX_U8 usecase_status;
}initParam_t;
#endif


