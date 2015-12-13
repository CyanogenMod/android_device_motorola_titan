#ifndef __BSXLIBRARYERRORCONSTANTS_H__
#define __BSXLIBRARYERRORCONSTANTS_H__
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
* @file      bsxlibraryerrorconstants.h
* @date      2013/02/12 created
*
* @brief
* This file provides error constants definition
*
* @detail
* bsxlibraryerrorconstants - file provides errors constants definition used for parsing errors returned by library
*
*/

/************************************************************************************************************/
/*												INCLUDES													*/
/************************************************************************************************************/

#include "BsxLibraryDataTypes.h"

/************************************************************************************************************/
/*											CONSTANT DEFINITIONS										 	*/
/************************************************************************************************************/

/** \def Internal States Defintion */
 
 #define BSX_STATE_ERROR   							(1)
 #define BSX_STATE_OK     							(0)
 #define BSX_STATE_PARAMETEROUTOFRANGE     			(-1)
 #define BSX_STATE_DIVIDEBYZERO   	     			(-2)
 #define BSX_STATE_NOTSUPPORTED                     (-3)
 
#endif



































