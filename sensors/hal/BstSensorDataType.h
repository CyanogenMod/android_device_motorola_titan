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
 * @file         BstSensorDataType.h
 * @date         "Wed Apr 3 14:05:34 2013 +0800"
 * @commit       "5420311"
 *
 * @brief
 *
 * @detail
 *
 */


#ifndef SENSOR_DATA_TYPE_H
#define SENSOR_DATA_TYPE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

typedef struct {
	union {
		int32_t v[4];
		struct {
			int32_t x;
			int32_t y;
			int32_t z;
			/* might be used for quaternion */
			int32_t w;
		};
	};
} sensor_data_ival_t;


typedef struct {
    union {
        float v[4];
        struct {
            float x;
            float y;
            float z;
			/* might be used for quaternion */
			float w;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
			float reserved;
        };
    };

} sensor_data_vec_t;

/**
 * Union of the various types of sensor data
 * that can be returned.
 */
typedef struct {
	/* must be sizeof(struct sensor_data_t) */
	int32_t version;

	uint32_t status;
	uint32_t corrected;
	uint32_t filtered;

	/* sensor identifier */
	int32_t sensor;

	/* sensor type */
	int32_t type;

	/* time is in nanosecond */
	union {
		int64_t timestamp;
		uint32_t reserved[2];
	};

	union {
	#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
		float           data[16];

		/* relative humidity in percent */
		float           relative_humidity;

		/* step-counter */
		uint64_t        step_counter;

		/* uncalibrated gyroscope values are in rad/s */
		uncalibrated_event_t uncalibrated_gyro;

		/* uncalibrated magnetometer values are in micro-Teslas */
		uncalibrated_event_t uncalibrated_magnetic;
	#else
		float           data[4];
	#endif

        /* acceleration values are in meter per second per second (m/s^2) */
        sensor_data_vec_t   acceleration;

        /* magnetic vector values are in micro-Tesla (uT) */
        sensor_data_vec_t   magnetic;

        /* orientation values are in degrees */
        sensor_data_vec_t   orientation;

        /* gyroscope values are in rad/s */
        sensor_data_vec_t   gyro;

        /* temperature is in degrees centigrade (Celsius) */
        float           temperature;

        /* distance in centimeters */
        float           distance;

        /* light in SI lux units */
        float           light;

        /* pressure in hectopascal (hPa) */
        float           pressure;
    };

} sensor_data_t;


struct exchange {
	int magic;

	union {
		struct {
			int32_t cmd;
			int32_t code;
			int32_t value;

			int32_t reserved[6];
		} command;

		sensor_data_t data;
	};

	int64_t ts;
};


typedef char BS_S8;
typedef uint8_t BS_U8;
typedef int16_t BS_S16;
typedef uint16_t BS_U16;
typedef int32_t BS_S32;
typedef uint32_t BS_U32;

typedef float BS_F32;
typedef double BS_F64;


struct value_map {
	int l;
	int r;
};

#endif  /* ANDROID_SENSORS_INTERFACE_H */
