/*
 * Copyright (C) 2008 The Android Open Source Project
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
 */

#ifndef __BST_SENSOR_TYPE_H
#define __BST_SENSOR_TYPE_H

/* Google defined sensor, used in Android 4.3: 20
    BST extend sensor through fusion, start at 24
    BST extend sensor based on BMA interrupt
    But Significant motion sensor is Google standard sensor
*/
enum
{
	BST_SENSOR_TYPE_NONSTD_START = 24-1,
	BST_SENSOR_TYPE_GEST_FLIP,
	BSTEXT_SENSOR_TYPE_SW_SGM,
#ifdef __DOUBLE_TAP_SENSOR_SUPPORT__
	BSTEXT_SENSOR_TYPE_DOUBLE_TAP,
#endif
	BST_SENSOR_TYPE_NONSTD_END
};

#endif  // __BST_SENSOR_TYPE_H