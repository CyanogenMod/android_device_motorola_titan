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
 * @file         algo_adapter.c
 * @date         "Tue Aug 27 15:51:32 2013 +0800"
 * @commit       "24cb7fe"
 *
 * @brief
 *
 * @detail
 *
 */


#include <stdio.h>
#include <errno.h>

#define LOG_TAG_MODULE "<algo_adapter_bst>"

#include "sensord.h"
#include "hw_def.h"

extern void fusion_arbitrate_dr();

extern struct algo_product *fusion_get_product(uint32_t type);

static uint32_t g_simulute_ts;

typedef struct sample_intval_type {
    BS_U16 intval;
    BS_U8 acc_dr;
    BS_U8 gyro_dr;
} sample_intval_t;

/* The sequence matches with definitions of BSX_DATARATE_XHZ in
 * BsxLibraryConstances.h */
static const sample_intval_t sample_intval[ALGO_NUM_DR] =
{
    {1000, HW_A_BW_8,   HW_G_BW_47},
    {200,  HW_A_BW_8,   HW_G_BW_47},
    {100,  HW_A_BW_8,   HW_G_BW_47},
    {50,   HW_A_BW_16,  HW_G_BW_47},
    {40,   HW_A_BW_16,  HW_G_BW_47},
    {25,   HW_A_BW_31,  HW_G_BW_47},
    {20,   HW_A_BW_31,  HW_G_BW_47},
    {10,   HW_A_BW_63,  HW_G_BW_47},
    {8,    HW_A_BW_63,  HW_G_BW_47},
    {5,    HW_A_BW_125, HW_G_BW_47}
};

static BS_U16 g_tab_hz[ALGO_NUM_DR] = {};
/* record the count of sensors which require hw gyro sensor present */
static uint32_t g_active_gyro_aps_cnt = 0;
static uint32_t g_active_aps = 0;
static hw_dep_set_t g_active_hws = 0;
static hw_dep_set_t g_hws_dep = 0;
#define HW_IS_ACTIVE(hws, id) (hws & (1 << SENSOR_HW_TYPE_ ## id))

static struct sensor_hw_a *g_p_hw_a = NULL;
static struct sensor_hw_m *g_p_hw_m = NULL;
static struct sensor_hw_g *g_p_hw_g = NULL;

static struct algo_work_mode *g_curr_work_mode = NULL;

#ifdef CFG_USE_DATA_LOG
static int g_fd_log_data_x = -1;
#endif

#define WM_IS_FUSION(wm) (((1<<SENSOR_TYPE_O) & (wm)->cap) \
                          && (1<<SENSOR_TYPE_VRV) & (wm)->cap)

/* mask only one individual capability */
#define WM_MASK_CAP(wm, type) (wm->mask |= (1<<(type)))
#define WM_UNMASK_CAP(wm, type) (wm->mask &= ~(1<<(type)))

#define WM_MASK_FUSION_CAPS(wm) \
    do { \
        WM_MASK_CAP(wm, SENSOR_TYPE_O); \
        WM_MASK_CAP(wm, SENSOR_TYPE_VRV); \
    } while (0)

static struct algo_work_mode g_algo_work_modes[] = {
    {
        name:"WM_S",
        mode:{opMode:BSX_WORKINGMODE_SLEEP},
        cap:0,
        available:1,
    },

    {
        name:"WM_A",
        mode:{opMode:BSX_WORKINGMODE_ACCONLY},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:1,
        min_dr_a:ALGO_DR_5HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_INVALID,
        max_dr_m:ALGO_DR_INVALID,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

    {
        name:"WM_M",
        mode:{opMode:BSX_WORKINGMODE_MAGONLY},
        cap:((1 << SENSOR_TYPE_M)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_MU)
#endif
            ),
        available:1,
        min_dr_a:ALGO_DR_INVALID,
        max_dr_a:ALGO_DR_INVALID,
        min_dr_m:ALGO_DR_5HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

    {
        name:"WM_AM",
        mode:{opMode:BSX_WORKINGMODE_ACCMAG},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_M)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:1,
        min_dr_a:ALGO_DR_10HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_10HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

    {
        name:"WM_G",
        mode:{opMode:BSX_WORKINGMODE_GYROONLY},
        cap:(1 << SENSOR_TYPE_G),
        /* CHECK */
        available:0,
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_INVALID,
        max_dr_m:ALGO_DR_INVALID,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

    {
        name:"WM_AG",
        mode:{opMode:BSX_WORKINGMODE_ACCGYRO},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_G)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_GYU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:1,
        /* CHECK */
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_INVALID,
        max_dr_m:ALGO_DR_INVALID,
        min_dr_g:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_g:ALGO_DR_100HZ,
#else
        max_dr_g:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
    },

    {
        name:"WM_AMG",
        mode:{opMode:BSX_WORKINGMODE_AMG},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_M)
        | (1 << SENSOR_TYPE_G)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_GYU) | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        /* CHECK */
        available:1,
        /* CHECK */
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_10HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_g:ALGO_DR_100HZ,
#else
        max_dr_g:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
    },

    {
        name:"WM_IMU+",
        mode:{opMode:BSX_WORKINGMODE_IMUPLUS},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_G)
        | (1 << SENSOR_TYPE_VG) | (1 << SENSOR_TYPE_VLA)
        | (1 << SENSOR_TYPE_VRV) | (1 << SENSOR_TYPE_O)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_GRV) | (1<<SENSOR_TYPE_GYU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:0,
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_INVALID,
        max_dr_m:ALGO_DR_INVALID,
        min_dr_g:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_g:ALGO_DR_100HZ,
#else
        max_dr_g:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
    },

    {
        name:"WM_COMPASS",
        mode:{opMode:BSX_WORKINGMODE_COMPASS},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_M)
        | (1 << SENSOR_TYPE_VG) | (1 << SENSOR_TYPE_VLA)
        | (1 << SENSOR_TYPE_VRV)
        | (1 << SENSOR_TYPE_O)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        | (1 << SENSOR_TYPE_GEOM_RV)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:0,
        min_dr_a:ALGO_DR_20HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_20HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

    {
        name:"WM_M4G",
        mode:{opMode:BSX_WORKINGMODE_M4G},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_M)
        | (1 << SENSOR_TYPE_G) | (1 << SENSOR_TYPE_VG)
        | (1 << SENSOR_TYPE_VLA)
        | (1 << SENSOR_TYPE_VRV)
        | (1 << SENSOR_TYPE_O) | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        | (1 << SENSOR_TYPE_GEOM_RV)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:0,
        min_dr_a:ALGO_DR_50HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_50HZ,
        max_dr_m:ALGO_DR_50HZ,
        min_dr_g:ALGO_DR_INVALID,
        max_dr_g:ALGO_DR_INVALID,
    },

#ifdef __FMC_OFF__
    {
        name:"WM_9DOFF",
        mode:{opMode:BSX_WORKINGMODE_NDOF_FMC_OFF},
        cap:((1<<SENSOR_TYPE_A) | (1<<SENSOR_TYPE_M)
        | (1<<SENSOR_TYPE_G) | (1<<SENSOR_TYPE_O)
        | (1<<SENSOR_TYPE_VG) | (1<<SENSOR_TYPE_VLA)
        | (1<<SENSOR_TYPE_VRV)
        | (1<<SENSOR_TYPE_GEST_FLIP)
        | (1<<SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_GRV) | (1<<SENSOR_TYPE_GYU) | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        | (1<<SENSOR_TYPE_GEOM_RV)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1<<SENSOR_TYPE_STC)
        | (1<<SENSOR_TYPE_STD)
#endif
            ),

        available:0,
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_20HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_g:ALGO_DR_100HZ,
#else
        max_dr_g:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
    },
#else
    {
        name:"WM_9DOFF",
        mode:{opMode:BSX_WORKINGMODE_NDOF},
        cap:((1 << SENSOR_TYPE_A) | (1 << SENSOR_TYPE_M)
        | (1 << SENSOR_TYPE_G) | (1 << SENSOR_TYPE_O)
        | (1 << SENSOR_TYPE_VG) | (1 << SENSOR_TYPE_VLA)
        | (1 << SENSOR_TYPE_VRV)
        | (1 << SENSOR_TYPE_GEST_FLIP)
        | (1 << SENSOR_TYPE_SW_SIGNIFICANT_MOTION)
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        | (1<<SENSOR_TYPE_GRV) | (1<<SENSOR_TYPE_GYU)
        | (1<<SENSOR_TYPE_MU)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        | (1 << SENSOR_TYPE_GEOM_RV)
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        | (1 << SENSOR_TYPE_STC)
        | (1 << SENSOR_TYPE_STD)
#endif
            ),
        available:0,
        min_dr_a:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_a:ALGO_DR_100HZ,
#else
        max_dr_a:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
        min_dr_m:ALGO_DR_20HZ,
        max_dr_m:ALGO_DR_20HZ,
        min_dr_g:ALGO_DR_100HZ,
#ifdef __FASTEST_MODE_100HZ__
        max_dr_g:ALGO_DR_100HZ,
#else
        max_dr_g:ALGO_DR_200HZ,
#endif //endif of __FASTEST_MODE_100HZ__
    },
#endif

};

static struct algo_work_mode *algo_get_matching_mode() {
    struct algo_work_mode *wm = NULL;
    struct algo_work_mode *wm_new = NULL;
    int i;
    uint32_t aps;
    uint32_t cap_num = 0;
    uint32_t cap_num_new = 0;

    for (i = 0; i < ARRAY_SIZE(g_algo_work_modes); i++) {
        wm = g_algo_work_modes + i;

        if (!wm->available) {
            continue;
        }

        aps = wm->cap;
        aps &= g_active_aps;
        if (aps & wm->mask) {
            PDEBUG("wm: %s has mask for caps: %x",
                   wm->name, wm->mask);
        }
        aps &= ~wm->mask;

        if (aps == g_active_aps) {
            if (NULL == wm_new) {
                wm_new = wm;
                cap_num_new =
                    get_num_set_bits_u32(wm_new->cap);
            } else {
                /* this is a simple way to determine
                 * which work mode has a lighter overhead
                 * it is not perfect but it works
                 */
                cap_num = get_num_set_bits_u32(wm->cap);
                if (cap_num < cap_num_new) {
                    PDEBUG("select wm:%s -> %s",
                           wm_new->name, wm->name);
                    wm_new = wm;
                    cap_num_new = cap_num;
                }
            }
        }
    }

    if (NULL == wm_new) {
        PWARN("no work mode is suitable");
    } else {
        PINFO("work mode: %s selected", wm_new->name);
    }

    return wm_new;
}


static void algo_check_new_hw_dep() {
    ts_HWsensorSwitchList hw;
    hw_dep_set_t dep_hw = 0;
    hw_dep_set_t dep_hw_change = 0;

    bsx_get_hwdependency(g_curr_work_mode->mode, &hw);

    if (hw.acc) {
        dep_hw |= (1 << SENSOR_HW_TYPE_A);
    }

    if (hw.mag) {
        dep_hw |= (1 << SENSOR_HW_TYPE_M);
    }

    if (hw.gyro) {
        dep_hw |= (1 << SENSOR_HW_TYPE_G);
    }

    dep_hw_change = dep_hw ^ g_active_hws;

    if (dep_hw_change & (1 << SENSOR_HW_TYPE_A)) {
        if (dep_hw & (1 << SENSOR_HW_TYPE_A)) {
            algo_load_calib_profile(SENSOR_MAGIC_A);
        }
    }

    if (dep_hw_change & (1 << SENSOR_HW_TYPE_M)) {
        if (dep_hw & (1 << SENSOR_HW_TYPE_M)) {
            algo_load_calib_profile(SENSOR_MAGIC_M);
        }
    }

    if (dep_hw_change & (1 << SENSOR_HW_TYPE_G)) {
        if (dep_hw & (1 << SENSOR_HW_TYPE_G)) {
            algo_load_calib_profile(SENSOR_MAGIC_G);
        }
    }

    g_hws_dep = dep_hw;
}


static void algo_mode_transit() {
    struct algo_work_mode *wm_new = NULL;

    wm_new = algo_get_matching_mode();

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
#ifdef __USECASE_TYPE_NDOF__
    struct algo_product *ap;

    ap = fusion_get_product(SENSOR_TYPE_GEOM_RV);
    if ((NULL != wm_new) && ap->enable)
    {
        if (g_active_gyro_aps_cnt != 0)
#ifdef __FMC_OFF__
            wm_new = g_algo_work_modes + ALGO_WORKING_MODE_NDOF_FMC_OFF;
#else
            wm_new = g_algo_work_modes + ALGO_WORKING_MODE_NDOF;
#endif
        else
            wm_new = g_algo_work_modes + ALGO_WORKING_MODE_COMPASS;
    }
    else
#endif
#endif
    {
        if ((NULL == wm_new) || (wm_new == g_curr_work_mode)) {
            PWARN("working mode not changed");
            return;
        }
    }

#ifdef CFG_USE_DATA_LOG
#ifdef __RAM_DATA_LOG_SUPPORT__
    // write sensor data buffer to file when sensor mode transmit
    if (-1 != g_fd_log_data_x)
    {
        if (data_log_level & DATA_LOG_USING_RAM_BUFFER)
        {
            g_trig_flag_log_data_x = 1;
        }
    }
#endif
#endif

    if (NULL != g_curr_work_mode) {
        if (NULL != g_curr_work_mode->exit) {
            g_curr_work_mode->exit();
        }
    }

    if (HW_IS_ACTIVE(g_active_hws, A)) {
        algo_save_calib_profile(SENSOR_MAGIC_A);
    }

    if (HW_IS_ACTIVE(g_active_hws, M)) {
        algo_save_calib_profile(SENSOR_MAGIC_M);
    }

    if (HW_IS_ACTIVE(g_active_hws, G)) {
        algo_save_calib_profile(SENSOR_MAGIC_G);
    }

    bsx_set_workingmode(&wm_new->mode);

    if (NULL != wm_new->entry) {
        wm_new->entry();
    }

    PINFO("work mode transit %s ---> %s",
          g_curr_work_mode ? g_curr_work_mode->name : "unknown",
          wm_new->name);
    g_curr_work_mode = wm_new;

    algo_check_new_hw_dep();
}


static void algo_resolve_internal_state() {
    if ((g_curr_work_mode->min_dr_a != ALGO_DR_INVALID)
            && (g_dr_a < g_curr_work_mode->min_dr_a)) {
        g_dr_a = g_curr_work_mode->min_dr_a;
    }

    if ((g_curr_work_mode->max_dr_a != ALGO_DR_INVALID)
            && (g_dr_a > g_curr_work_mode->max_dr_a)) {
        g_dr_a = g_curr_work_mode->max_dr_a;
    }

    if ((g_curr_work_mode->min_dr_m != ALGO_DR_INVALID)
            && (g_dr_m < g_curr_work_mode->min_dr_m)) {
        g_dr_m = g_curr_work_mode->min_dr_m;
    }

    if ((g_curr_work_mode->max_dr_m != ALGO_DR_INVALID)
            && (g_dr_m > g_curr_work_mode->max_dr_m)) {
        g_dr_m = g_curr_work_mode->max_dr_m;
    }


    if ((g_curr_work_mode->min_dr_g != ALGO_DR_INVALID)
            && (g_dr_g < g_curr_work_mode->min_dr_g)) {
        g_dr_g = g_curr_work_mode->min_dr_g;
    }

    if ((g_curr_work_mode->max_dr_g != ALGO_DR_INVALID)
            && (g_dr_g > g_curr_work_mode->max_dr_g)) {
        g_dr_g = g_curr_work_mode->max_dr_g;
    }

    PINFO("end dr: %d %d %d", g_dr_a, g_dr_m, g_dr_g);

    if (g_curr_work_mode->min_dr_a != ALGO_DR_INVALID) {
        bsx_set_accdatarate(g_dr_a);
#ifdef __DEBUG_TIMING_ACCURACY__
        init_timing_tolerance(SENSOR_HW_TYPE_A, sample_intval[g_dr_a].intval);
#endif
    }

    if (g_curr_work_mode->min_dr_m != ALGO_DR_INVALID) {
        bsx_set_magdatarate(g_dr_m);
#ifdef __DEBUG_TIMING_ACCURACY__
        init_timing_tolerance(SENSOR_HW_TYPE_M, sample_intval[g_dr_m].intval);
#endif
    }

    if (g_curr_work_mode->min_dr_g != ALGO_DR_INVALID) {
        bsx_set_gyrodatarate(g_dr_g);
#ifdef __DEBUG_TIMING_ACCURACY__
        init_timing_tolerance(SENSOR_HW_TYPE_G, sample_intval[g_dr_g].intval);
#endif
    }

}


static void algo_do_rearrange_workmode() {
    struct algo_work_mode *wm = NULL;
    int i = 0;
    BSX_U32 mode = 0;
    int usecase;

    usecase = algo_get_current_usecase();
    switch (usecase) {
    case ALGO_FUSION_MODE_IMU:
        mode = BSX_WORKINGMODE_IMUPLUS;
        break;
    case ALGO_FUSION_MODE_9DOF:
#ifdef __FMC_OFF__
        mode = BSX_WORKINGMODE_NDOF_FMC_OFF;
#else
        mode = BSX_WORKINGMODE_NDOF;
#endif
        break;
    case ALGO_FUSION_MODE_COMPASS:
        mode = BSX_WORKINGMODE_COMPASS;
        break;
    case ALGO_FUSION_MODE_M4G:
        mode = BSX_WORKINGMODE_M4G;
        break;
    default:
        break;
    }

    for (i = 0; i < ARRAY_SIZE(g_algo_work_modes); i++) {
        wm = g_algo_work_modes + i;
        /* disable gyro if m4g mode */
        if (mode == BSX_WORKINGMODE_M4G) {
            WM_MASK_CAP(wm, SENSOR_TYPE_G);
        }

        /* enable working mode for the use case */
        if (wm->mode.opMode == mode) {
            PINFO("fusion workmode enabled: %s", wm->name);
            wm->available = 1;
            /* enable virtual gyro for m4g working mode */
            if (mode == BSX_WORKINGMODE_M4G) {
                WM_UNMASK_CAP(wm, SENSOR_TYPE_G);
            }
        }
    }
}

void algo_adapter_init() {
    int i;
    struct sensor_hw *hw;
#ifdef CFG_USE_DATA_LOG
    char buf[512];
#endif

    for (i = 0; i < ALGO_NUM_DR; i++) {
        g_tab_hz[i] = 1000 / sample_intval[i].intval;
    }

    hw = hw_get_hw_by_id(SENSOR_HW_TYPE_A);
    if (hw) {
        g_p_hw_a = CONTAINER_OF(hw,
                                struct sensor_hw_a, hw);
    }

    hw = hw_get_hw_by_id(SENSOR_HW_TYPE_M);
    if (hw) {
        g_p_hw_m = CONTAINER_OF(hw,
                                struct sensor_hw_m, hw);
    }

    hw = hw_get_hw_by_id(SENSOR_HW_TYPE_G);
    if (hw) {
        g_p_hw_g = CONTAINER_OF(hw,
                                struct sensor_hw_g, hw);
    }

    algo_do_rearrange_workmode();
    algo_mode_transit();
#ifdef CFG_USE_DATA_LOG
    sprintf(buf, "/data/misc/sensor/sensor_data_bsx.log", get_build_date());
    g_fd_log_data_x = open(buf,
                           O_RDWR | O_CREAT | O_TRUNC,
                           S_IRUSR | S_IWUSR |
                           S_IRGRP | S_IWGRP |
                           S_IROTH | S_IWOTH);

    if (-1 != g_fd_log_data_x) {
        write_version(g_fd_log_data_x);
        sprintf(buf, "\n%s",
                "scheduling timestamp\ta.x\ta.y\ta.z\ttimestamp acc\t"
                "m.x\tm.y\tm.z\ttimestamp mag\t"
                "g.x\tg.y\tg.z\ttimestampe gyro\t"
                "gfs.status\tgfs.x\tgfs.y\tgfs.z\t\n");
        write(g_fd_log_data_x, buf, strlen(buf));
    }
#endif
}

static int algo_is_data_invalid(sensordata_t *p_data) {
    int zero_times = 0;       /* how many zeros in axis */

    if (p_data->data.x == 0) {
        zero_times++;
    }

    if (p_data->data.y == 0) {
        zero_times++;
    }

    if (p_data->data.z == 0) {
        zero_times++;
    }

    if (zero_times >= 2) {
        return 0;        //-EINVAL;
    }

    return 0;
}

#ifdef __DEBUG_TIMING_ACCURACY__
const char *hw_sensor_names[] =
{
    "acc", "gyro", "mag"
};

timeing_accuracy_t sensor_dbg_timing_status[SENSOR_HW_TYPE_MAX];

void init_timing_tolerance(int type, uint32_t interval)
{
    interval *= 1000;
    sensor_dbg_timing_status[type].biggest = interval * 2;
    sensor_dbg_timing_status[type].tolerance_high = (interval * 105) / 100;
    sensor_dbg_timing_status[type].tolerance_low = (interval * 95) / 100;
    PWARN("biggest :%d\n",
          sensor_dbg_timing_status[type].biggest);
    PWARN("tolerance high :%d\n",
          sensor_dbg_timing_status[type].tolerance_high);
    PWARN("tolerance_low :%d\n",
          sensor_dbg_timing_status[type].tolerance_low);
}

void print_timing_accuracy(void)
{
    int i = 0;

    for (i = 0; i < SENSOR_HW_TYPE_MAX; i++)
    {
        PWARN("timing accuracy info for %s:\n", hw_sensor_names[i]);
        PWARN("total :%d\n", sensor_dbg_timing_status[i].num_total);
        PWARN("level1(over 5percent):%d\n",
              sensor_dbg_timing_status[i].num_overlap_lvl1);
        PWARN("level2(over 100percent):%d\n",
              sensor_dbg_timing_status[i].num_overlap_lvl2);
    }
}
#endif

/*!
 * @brief this function update a sensor data with time stampe
 *
 * @param p_sensor[i]  an instance of sensor_hw
 * @param ts[i]            schedule time stamp for trigging read data from
 *                                driver
 * @param dr_index[i]  data rate index relative to the table sample_intvl
 * @param p_data[o]    out put xyz data and time stamp
 *
 * @return 0 success, or none zero failed
 * @retval -EBUSY     timestamp have not reached sample duration
 * @retval 0               success
 */
static int algo_update_sensor_data(struct sensor_hw *p_sensor,
                                       uint32_t ts, BS_U8 dr_index, sensordata_t *p_data) {
    int err = 0;
    sensor_data_ival_t val;
    uint32_t current_ts = 0;
    uint32_t ts_diff = 0;
    uint32_t expected_interval;

    /* get time duration and add a time tolerance */
    current_ts = ts;
    ts_diff = current_ts - p_sensor->ts_last_update;
    ts_diff += 200;
    expected_interval = sample_intval[dr_index].intval * 1000;

    /* period too short */
    if (ts_diff < expected_interval) {
        return 0;
    }

    err = p_sensor->get_data_nb(&val);
    if (!err) {
        /* use scheduling or calibrated scheduling timestamp
           as sensor timestamp */
#ifndef __SENSOR_TIMESTAMP_SCHEDULING__
        current_ts = get_current_timestamp();
#endif
        /*p_data->time_stamp = current_ts;*/
        p_data->time_stamp = g_simulute_ts;
        p_data->data.x = (BS_S16) val.x;
        p_data->data.y = (BS_S16) val.y;
        p_data->data.z = (BS_S16) val.z;
    }
#ifdef __DEBUG_TIMING_ACCURACY__
    if (p_sensor->ts_last_update)
    {
        timeing_accuracy_t *p_timing_dbg =
            &sensor_dbg_timing_status[p_sensor->type];
        uint32_t elapse;

        p_timing_dbg->num_total++;
        elapse = current_ts - p_sensor->ts_last_update;
        if (elapse > p_timing_dbg->biggest)
        {
            p_timing_dbg->num_overlap_lvl2++;
        }
        else if ((elapse < p_timing_dbg->tolerance_low)
                 || (elapse > p_timing_dbg->tolerance_high))
        {
            p_timing_dbg->num_overlap_lvl1++;
        }
    }
#endif
    p_sensor->ts_last_update = ts;

    return err;
}

/*!
 * @brief This function update all sensors' data periodically
 *
 * @param ts[i]       schedule time stamp for trigging read data from
 *                                driver
 *
 * @return 0 success, or none zero failed
 * @retval -EBUSY     timestamp have not reached sample duration
 * @retval 0          success
 */
static int algo_update_sensor_all(uint32_t ts, libraryinput_t *p_sensor_data) {
    int ret = 0;

    if (HW_IS_ACTIVE(g_active_hws, A) || HW_IS_ACTIVE(g_active_hws, G))
        g_simulute_ts += sample_intval[g_dr_a].intval * 1000;
    else
        g_simulute_ts += sample_intval[g_dr_m].intval * 1000;

    /* update magnetic first if magnetic will just use 'ts' directly
       without query realtime timestamp */
    if (HW_IS_ACTIVE(g_active_hws, M)) {
        ret = algo_update_sensor_data(&g_p_hw_m->hw, ts, g_dr_m,
                                      &p_sensor_data->mag);
        if (ret && (-ETIME != ret)) {
            PERR("read mag data failed:%d\n", ret);
            return ret;
        }
        ret = algo_is_data_invalid(&p_sensor_data->acc);
        if (ret) {
            PERR("acc data is invalid %d\n", ret);
            return ret;
        }
    }

    if (HW_IS_ACTIVE(g_active_hws, A)) {
        ret = algo_update_sensor_data(&g_p_hw_a->hw, ts, g_dr_a,
                                      &p_sensor_data->acc);
        if (ret && (-ETIME != ret)) {
            PERR("read acc data failed:%d\n", ret);
            return ret;
        }

        ret = algo_is_data_invalid(&p_sensor_data->acc);
        if (ret) {
            PERR("acc data is invalid %d\n", ret);
            return ret;
        }
    }

    if (HW_IS_ACTIVE(g_active_hws, G)) {
        ret = algo_update_sensor_data(&g_p_hw_g->hw, ts, g_dr_g,
                                      &p_sensor_data->gyro);
        if (ret && (-ETIME != ret)) {
            PERR("read gyro data failed:%d\n", ret);
            return ret;
        }
    }

    return ret;
}

static int algo_hz2data_rate(int hz) {
    int dr = 0;

    int i;

    for (i = 0; i < ALGO_NUM_DR; i++) {
        if (hz <= g_tab_hz[i]) {
            dr = i;
            break;
        }
    }

    if (i >= ALGO_NUM_DR) {
        dr = ALGO_NUM_DR - 1;
    }

    return dr;
}


int algo_enable_product(struct algo_product *ap, int enable) {
    int err = 0;

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
#ifdef __USECASE_TYPE_NDOF__
    if ((ap->type == SENSOR_TYPE_G) || (ap->type == SENSOR_TYPE_GYU)
            || (ap->type == SENSOR_TYPE_O)|| (ap->type == SENSOR_TYPE_VRV)
            || (ap->type == SENSOR_TYPE_GRV))
    {
        if (enable)
        {
            g_active_gyro_aps_cnt++;
        }
        else
        {
            if (g_active_gyro_aps_cnt > 0)
                g_active_gyro_aps_cnt--;
            else
                g_active_gyro_aps_cnt = 0;
        }
    }
#endif
#endif
    PDEBUG("g_active_gyro_aps_cnt = %d", g_active_gyro_aps_cnt);
    enable = !!enable;
    g_active_aps = SET_VALUE_BITS(g_active_aps, enable, ap->type, 1);
    algo_mode_transit();

    fusion_arbitrate_dr();
    algo_resolve_internal_state();
    return err;
}

int algo_update_sample_rate(void) {
    int ret = 0;

    if (HW_IS_ACTIVE(g_active_hws, A)) {
        if (g_p_hw_a && g_p_hw_a->set_bw) {
            ret = g_p_hw_a->set_bw(g_p_hw_a, sample_intval[g_dr_a].acc_dr);
        }
    }

    if (ret) {
        PERR("failed to change bandwidth for acc:%d\n", g_dr_a);
        return ret;
    }

    if (HW_IS_ACTIVE(g_active_hws, G)) {
        if (g_p_hw_g && g_p_hw_g->set_bw) {
            ret = g_p_hw_g->set_bw(g_p_hw_g, sample_intval[g_dr_g].gyro_dr);
        }
    }

    if (ret) {
        PERR("failed to change bandwidth for gyro:%d\n", g_dr_g);
    }
    return ret;
}

void algo_on_interval_changed(struct algo_product *ap, int *itvl) {
    int hz;
    int dr;
    int interval = *itvl;

    if (interval <= 0) {
        return;
    }

    interval = get_nearest_divisible_int(1000, interval);
    *itvl = interval;

    hz = 1000 / interval;
    dr = algo_hz2data_rate(hz);

    PINFO("type: %d interval: %d new hz: %d dr: %d",
          ap->type, interval, hz, dr);

    ap->dr_a = dr;
    ap->dr_m = dr;
    ap->dr_g = dr;

    fusion_arbitrate_dr();
    algo_resolve_internal_state();

    algo_update_sample_rate();
}


void algo_get_curr_hw_dep(hw_dep_set_t *dep) {
    *dep = g_hws_dep;
}


int algo_on_hw_dep_checked(const hw_dep_set_t *dep) {
    g_active_hws = *dep;

    return algo_update_sample_rate();
}


int algo_get_hint_interval() {
    int ret = 0;

    int intv_a = sample_intval[g_dr_a].intval;
    int intv_m = sample_intval[g_dr_m].intval;
    int intv_g = sample_intval[g_dr_g].intval;

    ret = 1000;

    if (HW_IS_ACTIVE(g_active_hws, A)) {
        ret = get_max_comm_div(ret, intv_a);
    }

    if (HW_IS_ACTIVE(g_active_hws, M)) {
        ret = get_max_comm_div(ret, intv_m);
    }

    if (HW_IS_ACTIVE(g_active_hws, G)) {
        ret = get_max_comm_div(ret, intv_g);
    }

    return ret;
}

static int algo_read_calib_profile(char magic, void *profile) {
    int err = 0;
    char *filename;

    switch (magic) {
    case SENSOR_MAGIC_A:
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_A;
        break;
    case SENSOR_MAGIC_M:
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_M;
        break;
    case SENSOR_MAGIC_G:
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_G;
        break;
    default:
        PWARN("unknown sensor");
        return -1;
    }


//	err = util_fs_read_file(filename, profile, sizeof(ts_calibprofile));
    err = util_fs_read_file(filename, profile, sizeof(calibprofile_crc_t));
    return err;
}

void algo_load_calib_profile(char magic) {
#if 1
    calibprofile_crc_t profile;
    ts_calibprofile *profile_p = (ts_calibprofile * ) & profile;
    BSX_U8 *profile_pu8;
    BSX_U8 crc = 0;
    int i;
    int err;
    /* reading calibration profile */
    err = algo_read_calib_profile(magic, &profile);
    if (err != 10) {
        PERR("failed on reading calibration profile:%c\n", magic);
        return;
    }
    /* Do CRC check */
    profile_pu8 = &profile;
    for (i = 0; i < sizeof(calibprofile_crc_t); i++) {
        crc += profile_pu8[i];
    }
    if (crc != 0) {
        PERR("profile <%c> CRC check failed!", magic);
        return;
    }
    /* set profile to bsx */
    switch (magic) {
    case SENSOR_MAGIC_A:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_acccalibprofile(profile_p);
        }
        break;
    case SENSOR_MAGIC_M:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_magcalibprofile(profile_p);
        }
        break;
    case SENSOR_MAGIC_G:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_gyrocalibprofile(profile_p);
        }
        break;

    default:
        PWARN("unknown sensor");
        return;
    }

    if (SENSOR_ACCURACY_HIGH == profile.accuracy) {
        PINFO("profile <%c> is loaded: %d %d %d %d %d",
              magic,
              (int) profile.calibParam.offset.x,
              (int) profile.calibParam.offset.y,
              (int) profile.calibParam.offset.z,
              (int) profile.calibParam.radius,
              (int) profile.accuracy
             );
    }
#else
    ts_calibprofile profile;
    int err;

    /* reading calibration profile */
    err = algo_read_calib_profile(magic, &profile);
    PERR("read calib return err %d", err);

    if (err == -1) {
        PERR("failed on reading calibration profile:%c\n", magic);
        return;
    }

    /* set profile to bsx */
    switch (magic) {
    case SENSOR_MAGIC_A:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_acccalibprofile(&profile);
        }
        break;
    case SENSOR_MAGIC_M:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_magcalibprofile(&profile);
        }
        break;
    case SENSOR_MAGIC_G:
        if (profile.accuracy == SENSOR_ACCURACY_HIGH) {
            bsx_set_gyrocalibprofile(&profile);
        }
        break;

    default:
        PWARN("unknown sensor");
        return;
    }

    if (SENSOR_ACCURACY_HIGH == profile.accuracy) {
        PINFO("profile <%c> is loaded: %d %d %d %d %d",
              magic,
              (int)profile.calibParam.offset.x,
              (int)profile.calibParam.offset.y,
              (int)profile.calibParam.offset.z,
              (int)profile.calibParam.radius,
              (int)profile.accuracy
             );
    }
#endif
}


void algo_save_calib_profile(char magic) {
#if 1
    calibprofile_crc_t profile;
    ts_calibprofile *profile_p = (ts_calibprofile * ) & profile;
    char *filename = NULL;
    int ret;
    BSX_U8 *profile_pu8;
    int i;

    switch (magic) {

    case SENSOR_MAGIC_A:
        bsx_get_acccalibprofile(profile_p);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_A;
        break;

    case SENSOR_MAGIC_M:
        bsx_get_magcalibprofile(profile_p);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_M;
        break;

    case SENSOR_MAGIC_G:
        bsx_get_gyrocalibprofile(profile_p);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_G;
        break;
    default:
        PWARN("unknown sensor");
        return;
    }

    if (profile.accuracy != SENSOR_ACCURACY_HIGH) {
        return;
    }
    /* Calculate CRC for profile */
    profile.crc = 0;
    profile_pu8 = &profile;
    for (i = 0; i < sizeof(calibprofile_crc_t) - 1; i++) {
        profile.crc -= profile_pu8[i];
    }

    ret = util_fs_write_file(filename, &profile, sizeof(calibprofile_crc_t));
    if (ret == -1) {
        PERR("save caliration file failed:%s\n", filename);
    }
#else
    ts_calibprofile profile;
    char *filename = NULL;
    int ret;

    switch (magic) {

    case SENSOR_MAGIC_A:
        bsx_get_acccalibprofile(&profile);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_A;
        break;

    case SENSOR_MAGIC_M:
        bsx_get_magcalibprofile(&profile);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_M;
        break;

    case SENSOR_MAGIC_G:
        bsx_get_gyrocalibprofile(&profile);
        filename = SENSOR_CFG_FILE_SYS_PROFILE_CALIB_G;

        PERR("check offset before save: offset xyz %d %d %d", profile.calibParam.offset.x,
             profile.calibParam.offset.y, profile.calibParam.offset.z);

        break;
    default:
        PWARN("unknown sensor");
        return;
    }

    if (profile.accuracy != SENSOR_ACCURACY_HIGH)
    {
        return;
    }
    ret = util_fs_write_file(filename, &profile, sizeof(profile));
    if (ret == -1)
    {
        PERR("save caliration file failed:%s\n", filename);
    }
#endif
}

BS_S32 algo_proc_data(uint32_t ts) {
    int err = 0;
    int ret = 0;
#ifdef CFG_USE_DATA_LOG
    char line[256] = "";
#endif
    static libraryinput_t all_sensor_data;

    /* read all sensor data */
    ret = algo_update_sensor_all(ts, &all_sensor_data);
    if (ret != 0) {
        PERR("update sensor data failed:%d\n", ret);
        return ret;
    }

    /* record data in log file */
#ifdef CFG_USE_DATA_LOG
    if (-1 != g_fd_log_data_x) {
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%11u,%6d,%6d,%6d,%11u,", ts,
                       all_sensor_data.acc.data.x,
                       all_sensor_data.acc.data.y,
                       all_sensor_data.acc.data.z,
                       all_sensor_data.acc.time_stamp);
        if (ret > 0) {
            write(g_fd_log_data_x, line, ret);
        }

        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%6d,%6d,%6d,%11u,",
                       all_sensor_data.mag.data.x,
                       all_sensor_data.mag.data.y,
                       all_sensor_data.mag.data.z,
                       all_sensor_data.mag.time_stamp);
        if (ret > 0) {
            write(g_fd_log_data_x, line, ret);
        }

        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%6d,%6d,%6d,%11u",
                       all_sensor_data.gyro.data.x,
                       all_sensor_data.gyro.data.y,
                       all_sensor_data.gyro.data.z,
                       all_sensor_data.gyro.time_stamp);
        if (ret > 0) {
            write(g_fd_log_data_x, line, ret);
        }

    }
#endif

    bsx_dostep(&all_sensor_data);
    return err;
}

int algo_set_opmode(int32_t sensor_type, int32_t opmode) {
    int ret = 0;
    switch (sensor_type) {
    case SENSOR_TYPE_GEST_FLIP:
        bsx_set_flipgesture_detecttime(1000);
        bsx_set_flipgesture_angle(FLIP_FACE_UP, FLIP_FACE_DOWN);
        if (opmode) {
            bsx_set_flipgesture_opmode(BSX_FLIP_GESTURE_MODE_REGULAR);
        }
        else {
            bsx_set_flipgesture_opmode(BSX_FLIP_GESTURE_MODE_SLEEP);
        }
        break;
    case SENSOR_TYPE_SW_SIGNIFICANT_MOTION:
        if (opmode) {
            bsx_set_significantmotiondetectionopmode(1);
        }
        else {
            bsx_set_significantmotiondetectionopmode(0);
        }
        break;
    case SENSOR_TYPE_STC:
    case SENSOR_TYPE_STD:
        if (opmode) {
            uint8_t mode = 0;
            bsx_get_stepdetectionopmode(&mode);
            if (mode == 0) {
                bsx_set_stepdetectionopmode(1);
            }
        }
        else {
            bsx_set_stepdetectionopmode(0);
        }
        break;
    default:
        PERR(" sensor type %d not find", sensor_type);
        break;
    }
    return ret;
}

int algo_get_proc_data_a(sensor_data_t *pdata) {
    int err = 0;
    BS_U8 status = 0;

    err = bsx_get_acccordata((ts_dataxyzf32 * ) & pdata->acceleration);

    bsx_get_acccalibaccuracy(&status);
#ifdef __DEBUG_DATALOG_WITH_ACCURACY__
    if(BSX_THREE != status)
    {
        char line[64];
        int ret;

        if (-1 != g_fd_log_data_x) {
            ret = snprintf(line, ARRAY_SIZE(line) - 1,
                           "\t\twarning! acc accuracy status :%d\n",
                           status);
            write(g_fd_log_data_x, line, ret);
        }
    }
#endif
    pdata->status = status;

    return err;
}

int algo_get_proc_data_m(sensor_data_t *pdata) {
    int err = 0;
    BS_U8 status = 0;

    err = bsx_get_magfiltdata1((ts_dataxyzf32 * ) & pdata->magnetic);

    bsx_get_magcalibaccuracy(&status);
#ifdef __DEBUG_DATALOG_WITH_ACCURACY__
    if(BSX_THREE != status)
    {
        char line[64];
        int ret;

        if (-1 != g_fd_log_data_x) {
            ret = snprintf(line, ARRAY_SIZE(line) - 1,
                           "\t\twarning! mag accuracy status :%d \n",
                           status);
            write(g_fd_log_data_x, line, ret);
        }
    }
#endif
    pdata->status = status;
    return err;
}

int algo_get_proc_data_o(sensor_data_t *pdata) {
    int err = 0;
    BS_U8 status = 0;

    err = bsx_get_orientdata_euler_deg((ts_dataeulerf32 * ) & pdata->orientation);
    bsx_get_orient_datastatus(&status);
    pdata->status = status;
#ifdef __DEBUG_DATALOG_WITH_ACCURACY__
    if(BSX_THREE != status)
    {
        char line[64];
        int ret;

        if (-1 != g_fd_log_data_x) {
            ret = snprintf(line, ARRAY_SIZE(line) - 1,
                           "\t\twarning!orientation mag accuracy status :%d \n",
                           status);
            if (ret > 0) {
                write(g_fd_log_data_x, line, ret);
            }
        }
    }
#endif

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output(g_fd_log_data_o, pdata);
#endif

    return err;
}

int algo_get_proc_data_g(sensor_data_t *pdata) {
    int err = 0;
    ts_dataxyzf32 data;
    BS_U8 status = 0;

    err = bsx_get_gyrocordata_dps((ts_dataxyzf32 * ) & data);
    pdata->gyro.x = (float) data.x / (180 / PI);
    pdata->gyro.y = (float) data.y / (180 / PI);
    pdata->gyro.z = (float) data.z / (180 / PI);

    bsx_get_gyrocalibaccuracy(&status);
#ifdef __DEBUG_DATALOG_WITH_ACCURACY__
    if(BSX_THREE != status)
    {
        char line[256];
        int ret;

        if (-1 != g_fd_log_data_x) {
            ret = snprintf(line, ARRAY_SIZE(line) - 1,
                           "%d,%f,%f,%f\n",
                           status, pdata->gyro.x, pdata->gyro.y,
                           pdata->gyro.z);
            if (ret > 0) {
                write(g_fd_log_data_x, line, ret);
            }
        }
    }
#endif
    pdata->status = status;

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_AMG)
        data_log_output(g_fd_log_data_g, pdata);
#endif

    return err;
}

int algo_get_proc_data_vg(sensor_data_t *pdata) {
    bsx_get_gravity((ts_dataxyzf32 * ) & pdata->data);

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output(g_fd_log_data_vg, pdata);
#endif

    return 0;
}

int algo_get_proc_data_vla(sensor_data_t *pdata) {
    bsx_get_linearacceleration((ts_dataxyzf32 * ) & pdata->data);

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output(g_fd_log_data_vla, pdata);
#endif

    return 0;
}

int algo_get_proc_data_vrv(sensor_data_t *pdata) {
    int err = 0;
    ts_dataquatf32 quat;

    err = bsx_get_orientdata_quat((ts_dataquatf32 * ) & quat);
#ifdef ALGO_FIX_QUAT_SIGN
    quat.x = 0 - quat.x;
    quat.y = 0 - quat.y;
    quat.z = 0 - quat.z;
#endif

    if (quat.w < 0) {
        quat.w = 0 - quat.w;
        quat.x = 0 - quat.x;
        quat.y = 0 - quat.y;
        quat.z = 0 - quat.z;
    }

    pdata->data[0] = quat.x;
    pdata->data[1] = quat.y;
    pdata->data[2] = quat.z;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
    pdata->data[3] = quat.w;
    bsx_get_headingaccuracy_rad(&(pdata->data[4]));
#else
    pdata->data[3] = 0;
#endif

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output_vrv(g_fd_log_data_vrv, pdata);
#endif

    return err;
}

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
int algo_get_proc_data_grv(sensor_data_t *pdata)
{
    int err = 0;
    ts_dataquatf32 quat;

    err = bsx_get_imuorientdata_quat((ts_dataquatf32*)&quat);

    if (quat.w < 0) {
        quat.w = 0 - quat.w;
        quat.x = 0 - quat.x;
        quat.y = 0 - quat.y;
        quat.z = 0 - quat.z;
    }

    pdata->data[0] = quat.x;
    pdata->data[1] = quat.y;
    pdata->data[2] = quat.z;
    pdata->data[3] = 0;
    pdata->data[4] = 0;

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output_vrv(g_fd_log_data_grv, pdata);
#endif

    return err;
}

int algo_get_proc_data_gu(sensor_data_t *pdata)
{
    int err = 0;
    ts_dataxyzf32 data;
    BS_U8 status = 0;

    bsx_get_gyrorawdata_dps((ts_dataxyzf32*)&data);
    pdata->uncalibrated_gyro.x_uncalib = (float) data.x / (180 / PI);
    pdata->uncalibrated_gyro.y_uncalib = (float) data.y / (180 / PI);
    pdata->uncalibrated_gyro.z_uncalib = (float) data.z / (180 / PI);

    bsx_get_gyrooffset_dps((ts_dataxyzf32*)&data);
    pdata->uncalibrated_gyro.x_bias = (float) data.x / (180 / PI);
    pdata->uncalibrated_gyro.y_bias = (float) data.y / (180 / PI);
    pdata->uncalibrated_gyro.z_bias = (float) data.z / (180 / PI);

    bsx_get_gyrocalibaccuracy(&status);
    pdata->status = status;

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_MU_GU)
        data_log_output_gu(g_fd_log_data_gu, pdata);
#endif

    return err;
}

int algo_get_proc_data_mu(sensor_data_t *pdata)
{
    int err = 0;
    BS_U8 status = 0;
    ts_dataxyzf32 data;

    bsx_get_magsoftironcorrecteddata((ts_dataxyzf32*)&pdata->uncalibrated_magnetic);
    bsx_get_magoffsets((ts_dataxyzf32*)&data);
    pdata->uncalibrated_magnetic.x_bias = (float) data.x;
    pdata->uncalibrated_magnetic.y_bias = (float) data.y;
    pdata->uncalibrated_magnetic.z_bias = (float) data.z;

    bsx_get_magcalibaccuracy(&status);
    pdata->status = status;

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_MU_GU)
        data_log_output_mu(g_fd_log_data_mu, pdata);
#endif

    return err;
}
#endif

int algo_get_proc_data_gest_flip(sensor_data_t *pdata) {
    uint8_t flip_status;
    static uint8_t flip_status_back = SENSOR_GEST_FLIP_VAL_UNKNOWN;

    (void) bsx_get_flipgesture_status(&flip_status);

    if (flip_status_back != flip_status) {
        pdata->data[0] = flip_status;
        pdata->data[1] = 0;
        pdata->data[2] = 0;
        flip_status_back = flip_status;
        return 1;
    }
    return 0;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__

int algo_get_proc_data_sgm(sensor_data_t *pdata) {
    uint16_t sgm_status;
    static uint8_t smg_status_prev = 0;

    bsx_get_significantmotiondetectionstatus(&sgm_status);

    if (smg_status_prev != sgm_status) {
        /* if not a valid status value, then won't report*/
        if (sgm_status == 0) {
            smg_status_prev = sgm_status;
            return 0;
        }

        pdata->data[0] = sgm_status;
        pdata->data[1] = 0;
        pdata->data[2] = 0;
        smg_status_prev = sgm_status;
        return 1;
    }
    return 0;

}

int algo_get_proc_data_stc(sensor_data_t *pdata) {
    int ret = 0;
    uint64_t steps = 0;
    static uint64_t pre_steps = 0;

    bsx_get_stepcount(&steps);

    if (pre_steps != steps) {
        pdata->step_counter = steps;
        pre_steps = steps;
        ret = 1;
        PINFO("get step counter %lld pre_steps %lld", pdata->step_counter, pre_steps);

    }

    return ret;

}

int algo_get_proc_data_std(sensor_data_t *pdata) {
    int ret = 0;
    uint8_t step_status = 0;
    static uint8_t pre_step_status = 0;

    (void) bsx_get_stepdetectionstatus(&step_status);

    if (pre_step_status != step_status) {
        /* if not a valid status then won't report
           it as a step detector event*/
        if (step_status == 0) {
            pre_step_status = step_status;
            return 0;
        }
        pdata->data[0] = step_status;
        pdata->data[1] = 0;
        pdata->data[2] = 0;
        pre_step_status = step_status;
        ret = 1;

    }

    return ret;
}

#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__

int algo_get_proc_data_geom_rv(sensor_data_t *pdata) {
    int err = 0;
    ts_dataquatf32 quat;

    err = bsx_get_orientdata_quat((ts_dataquatf32 * ) & quat);

    if (quat.w < 0) {
        quat.w = 0 - quat.w;
        quat.x = 0 - quat.x;
        quat.y = 0 - quat.y;
        quat.z = 0 - quat.z;
    }

    pdata->data[0] = quat.x;
    pdata->data[1] = quat.y;
    pdata->data[2] = quat.z;
    pdata->data[3] = quat.w;
    bsx_get_headingaccuracy_rad(&(pdata->data[4]));
    PINFO("get geom data %f %f %f %f %f", quat.x, quat.y, quat.z, quat.w, pdata->data[4]);

#ifdef CFG_USE_DATA_LOG
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
        data_log_output_vrv(g_fd_log_data_grv, pdata);
#endif

    return err;
}

#endif

#ifdef CFG_USE_DATA_LOG
/* data log functions*/
void data_log_init()
{
    /* read file to decide the level of data log */
    int fd;
    int count;
    char buf[512] = "";
    char *pline = NULL;
    int i;

    fd = open(SENSOR_CFG_FILE_DATA_LOG_LEVEL, O_RDONLY);
    if (fd < 0)
    {
        PINFO("data log level file does not exist, no data log");
        return;
    }

    count = read(fd, buf, ARRAY_SIZE(buf) - 1);
    close(fd);
    for (i=15; i<count; i++)
    {
        if ((buf[i] != ' ') && (buf[i] != 0x09))
            break;
    }

    if ((buf[i] >= '0') && (buf[i] <= '9'))
        data_log_level = buf[i] - '0';
    else if ((buf[i] >= 'a') && (buf[i] <= 'f'))
        data_log_level = buf[i] + 10 - 'a';
    else if ((buf[i] >= 'A') && (buf[i] <= 'F'))
        data_log_level = buf[i] + 10 - 'A';
    else
        return;

    if ((buf[i+1] >= '0') && (buf[i+1] <= '9'))
        data_log_level = data_log_level *16 + buf[i+1] - '0';
    else if ((buf[i+1] >= 'a') && (buf[i+1] <= 'f'))
        data_log_level = data_log_level *16 + buf[i+1] + 10 - 'a';
    else if ((buf[i+1] >= 'A') && (buf[16] <= 'F'))
        data_log_level = data_log_level *16 + buf[i+1] + 10 - 'A';
    PINFO("data log level = %d", data_log_level);

    if ((data_log_level & DATA_LOG_LEVEL_INPUT)||(data_log_level & DATA_LOG_USING_RAM_BUFFER))
    {
        sprintf(buf, "/data/misc/sensor/sensor_data_bsx.log");
        g_fd_log_data_x = open(buf,
                               O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR |
                               S_IRGRP | S_IWGRP |
                               S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_x) {
#ifdef __RAM_DATA_LOG_SUPPORT__
            if (data_log_level & DATA_LOG_USING_RAM_BUFFER)
            {
                g_buf_ptr_log_data_x[0] = malloc(DATA_LOG_BUF_SIZE_X * sizeof(char));
                g_buf_ptr_log_data_x[1] = malloc(DATA_LOG_BUF_SIZE_X * sizeof(char));
                if ( (NULL == g_buf_ptr_log_data_x[0]) || (NULL == g_buf_ptr_log_data_x[1]))
                {
                    PERR("malloc memory fail for sensor_data_bsx.log");
                    close(g_fd_log_data_x);
                    g_fd_log_data_x = -1;
                    if (NULL != g_buf_ptr_log_data_x[0])
                        free(g_buf_ptr_log_data_x[0]);
                    if (NULL != g_buf_ptr_log_data_x[1])
                        free(g_buf_ptr_log_data_x[1]);
                    return;
                }
                g_fill_ptr_log_data_x[0] = g_buf_ptr_log_data_x[0];
                g_fill_ptr_log_data_x[1] = g_buf_ptr_log_data_x[1];
            }
#endif
            write_version(g_fd_log_data_x);
            sprintf(buf, "\n%s",
                    "scheduling_timestamp,\ta.x,a.y,a.z,timestamp_acc,\t"
                    "m.x,m.y,m.z,timestamp_mag,\t"
                    "g.x,g.y,g.z,timestampe_gyro\n");
            write(g_fd_log_data_x, buf, strlen(buf));
        }
    }

    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_AMG)
    {
        g_fd_log_data_a = open("/data/misc/sensor/sensor_data_a.log",
                               O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR |
                               S_IRGRP | S_IWGRP |
                               S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_a) {
            pline = "a.x,  a.y,  a.z,  timestamp acc\n";
            write(g_fd_log_data_a, pline, strlen(pline));
        }
        g_fd_log_data_m = open("/data/misc/sensor/sensor_data_m.log",
                               O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR |
                               S_IRGRP | S_IWGRP |
                               S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_m) {
            pline = "m.x,  m.y,  m.z,  timestamp mag\n";
            write(g_fd_log_data_m, pline, strlen(pline));
        }
        g_fd_log_data_g = open("/data/misc/sensor/sensor_data_g.log",
                               O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR |
                               S_IRGRP | S_IWGRP |
                               S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_g) {
            pline = "g.x,  g.y,  g.z,  timestampe gyro\n";
            write(g_fd_log_data_g, pline, strlen(pline));
        }
    }

    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
    {
        g_fd_log_data_vrv = open("/data/misc/sensor/sensor_data_vrv.log",
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR |
                                 S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_vrv) {
            pline = "vrv.x,  vrv.y,  vrv.z,  vrv.data3,  vrv.data4,  timestamp vrv\n";
            write(g_fd_log_data_vrv, pline, strlen(pline));
        }
        g_fd_log_data_o = open("/data/misc/sensor/sensor_data_o.log",
                               O_RDWR | O_CREAT | O_TRUNC,
                               S_IRUSR | S_IWUSR |
                               S_IRGRP | S_IWGRP |
                               S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_o) {
            pline = "o.h,  o.p,  o.r,  timestamp o, accuracy\n";
            write(g_fd_log_data_o, pline, strlen(pline));
        }
        g_fd_log_data_vg = open("/data/misc/sensor/sensor_data_vg.log",
                                O_RDWR | O_CREAT | O_TRUNC,
                                S_IRUSR | S_IWUSR |
                                S_IRGRP | S_IWGRP |
                                S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_vg) {
            pline = "vg.x,	vg.y,  vg.z,  timestamp vg\n";
            write(g_fd_log_data_vg, pline, strlen(pline));
        }
        g_fd_log_data_vla = open("/data/misc/sensor/sensor_data_vla.log",
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR |
                                 S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_vla) {
            pline = "vla.x,  vla.y,  vla.z,  timestamp vla\n";
            write(g_fd_log_data_vla, pline, strlen(pline));
        }
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        g_fd_log_data_stc = open("/data/misc/sensor/sensor_data_stc.log",
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR |
                                 S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_stc)
        {
            pline = "step counter, time now\n";
            write(g_fd_log_data_stc, pline, strlen(pline));
        }
        g_fd_log_data_std = open("/data/misc/sensor/sensor_data_std.log",
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR |
                                 S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_std)
        {
            pline = "step detector, time now\n";
            write(g_fd_log_data_std, pline, strlen(pline));
        }
#endif
    }

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_VIRTUAL_SENSOR)
    {
        g_fd_log_data_grv = open("/data/misc/sensor/sensor_data_grv.log",
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR |
                                 S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_grv) {
            pline = "grv.x,  grv.y,  grv.z,  grv.data3,  grv.data4,  timestamp grv\n";
            write(g_fd_log_data_grv, pline, strlen(pline));
        }
    }

    if (data_log_level & DATA_LOG_LEVEL_OUTPUT_MU_GU)
    {
        g_fd_log_data_gu = open("/data/misc/sensor/sensor_data_gu.log",
                                O_RDWR | O_CREAT | O_TRUNC,
                                S_IRUSR | S_IWUSR |
                                S_IRGRP | S_IWGRP |
                                S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_gu) {
            pline = ("gu.x,	gu.y,  gu.z,  gu.x_bias,  gu.y_bias,  gu.z_bias,  "
                     "g.x,  g.y,	g.z,	g_filt.x,  g_filt.y,  g_filt.z, accuracy, timestamp gu\n");
            write(g_fd_log_data_gu, pline, strlen(pline));
        }
        g_fd_log_data_mu = open("/data/misc/sensor/sensor_data_mu.log",
                                O_RDWR | O_CREAT | O_TRUNC,
                                S_IRUSR | S_IWUSR |
                                S_IRGRP | S_IWGRP |
                                S_IROTH | S_IWOTH);
        if (-1 != g_fd_log_data_mu) {
            pline = ("mu.x,	mu.y,  mu,z,  mu.x_bias,  mu.y_bias,  mu.z_bias,  "
                     "m.x,  m.y,	m.z,	accuracy, timestamp mu\n");
            write(g_fd_log_data_mu, pline, strlen(pline));
        }
    }
#endif
}

void data_log_input_to_algo(libraryinput_t all_sensor_data, BS_U32 ts)
{
    int ret = 0;

    if (-1 != g_fd_log_data_x) {
#ifdef __RAM_DATA_LOG_SUPPORT__
        if (data_log_level & DATA_LOG_USING_RAM_BUFFER)
        {
            ret = snprintf(g_fill_ptr_log_data_x[g_buf_index_log_data_x], 250,
                           "%11u,%d,%d,%d,%11u,%d,%d,%d,%11u,%d,%d,%d,%11u ;\n",
                           ts,
                           all_sensor_data.acc.data.x,
                           all_sensor_data.acc.data.y,
                           all_sensor_data.acc.data.z,
                           all_sensor_data.acc.time_stamp,
                           all_sensor_data.mag.data.x,
                           all_sensor_data.mag.data.y,
                           all_sensor_data.mag.data.z,
                           all_sensor_data.mag.time_stamp,
                           all_sensor_data.gyro.data.x,
                           all_sensor_data.gyro.data.y,
                           all_sensor_data.gyro.data.z,
                           all_sensor_data.gyro.time_stamp
                          );
            if (ret > 0)
            {
                g_fill_ptr_log_data_x[g_buf_index_log_data_x] += strlen(g_fill_ptr_log_data_x[g_buf_index_log_data_x]);
            }
            else
            {
                PERR("print string overflow");
            }
            if ( ((g_fill_ptr_log_data_x[g_buf_index_log_data_x] - g_buf_ptr_log_data_x[g_buf_index_log_data_x]) > DATA_LOG_BUF_OVER_LIMIT_X) ||
                    (ts > last_data_log_time_x+DATA_LOG_TIMEOUT_X) ||
                    g_trig_flag_log_data_x)
            {
                g_trig_flag_log_data_x = 0;
                g_fill_ptr_log_data_x[g_buf_index_log_data_x] = '\0';
                write(g_fd_log_data_x, g_buf_ptr_log_data_x[g_buf_index_log_data_x], strlen(g_buf_ptr_log_data_x[g_buf_index_log_data_x]));
                last_data_log_time_x = ts;
                g_fill_ptr_log_data_x[g_buf_index_log_data_x] = g_buf_ptr_log_data_x[g_buf_index_log_data_x];
                g_buf_index_log_data_x = !g_buf_index_log_data_x;
            }
        }
        else
#endif
        {
            char line[256] = "";
            ret = snprintf(line, ARRAY_SIZE(line) - 1,
                           "%11u,%6d,%6d,%6d,%11u,%6d,%6d,%6d,%11u,%6d,%6d,%6d,%11u\n",
                           ts,
                           all_sensor_data.acc.data.x,
                           all_sensor_data.acc.data.y,
                           all_sensor_data.acc.data.z,
                           all_sensor_data.acc.time_stamp,
                           all_sensor_data.mag.data.x,
                           all_sensor_data.mag.data.y,
                           all_sensor_data.mag.data.z,
                           all_sensor_data.mag.time_stamp,
                           all_sensor_data.gyro.data.x,
                           all_sensor_data.gyro.data.y,
                           all_sensor_data.gyro.data.z,
                           all_sensor_data.gyro.time_stamp
                          );
            if (ret > 0) {
                write(g_fd_log_data_x, line, ret);
            }
        }
    }
}

/* data log function for sensors with only x,y,z data */
void data_log_output(int fd, sensor_data_t *pdata)
{
    char line[256] = "";
    int ret;

    if (-1 != fd) {
        struct timeval time_now;
        gettimeofday (&time_now, NULL);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %lu, %d\n",
                       pdata->data[0],pdata->data[1],
                       pdata->data[2], time_now.tv_sec*1000 + time_now.tv_usec/1000,
                       pdata->status);
        if (ret > 0) {
            write(fd, line, ret);
        }
    }
}

/* vrv and grv sensor need to log 5 data */
void data_log_output_vrv(int fd, sensor_data_t *pdata)
{
    char line[256] = "";
    int ret;

    if (-1 != fd) {
        struct timeval time_now;
        gettimeofday (&time_now, NULL);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %f, %f, %lu ms\n",
                       pdata->data[0],pdata->data[1],
                       pdata->data[2], pdata->data[3],
                       pdata->data[4], time_now.tv_sec*1000 + time_now.tv_usec/1000);
        if (ret > 0) {
            write(fd, line, ret);
        }
    }
}

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
void data_log_output_gu(int fd, sensor_data_t *pdata)
{
    char line[256] = "";
    int ret;
    ts_dataxyzf32 data;
    BS_U8 status = 0;

    if (-1 != fd) {
        struct timeval time_now;
        gettimeofday (&time_now, NULL);

        bsx_get_gyrocordata_dps((ts_dataxyzf32*)&data);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %f, %f, %f, %f, %f, %f, ",
                       pdata->uncalibrated_gyro.x_uncalib,pdata->uncalibrated_gyro.y_uncalib,pdata->uncalibrated_gyro.z_uncalib,
                       pdata->uncalibrated_gyro.x_bias, pdata->uncalibrated_gyro.y_bias,pdata->uncalibrated_gyro.z_bias,
                       (float) data.x / (180 / PI),(float) data.y / (180 / PI),(float) data.z / (180 / PI) );
        if (ret > 0) {
            write(fd, line, ret);
        }

        bsx_get_gyrofiltdata1_dps((ts_dataxyzf32*)&data);
        bsx_get_gyrocalibaccuracy(&status);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %d, %lu ms\n",
                       (float) data.x / (180 / PI),(float) data.y / (180 / PI),(float) data.z / (180 / PI),
                       status, time_now.tv_sec*1000 + time_now.tv_usec/1000);
        if (ret > 0) {
            write(fd, line, ret);
        }
    }
}

void data_log_output_mu(int fd, sensor_data_t *pdata)
{
    BS_U8 status = 0;
    ts_dataxyzf32 data;
    char line[256] = "";
    int ret;

    if (-1 != fd) {
        struct timeval time_now;
        gettimeofday (&time_now, NULL);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %f, %f, %f, ",
                       pdata->uncalibrated_magnetic.x_uncalib,pdata->uncalibrated_magnetic.y_uncalib,pdata->uncalibrated_magnetic.z_uncalib,
                       pdata->uncalibrated_magnetic.x_bias, pdata->uncalibrated_magnetic.y_bias,pdata->uncalibrated_magnetic.z_bias);
        if (ret > 0) {
            write(fd, line, ret);
        }
        bsx_get_magfiltdata1((ts_dataxyzf32*)&data);
        bsx_get_magcalibaccuracy(&status);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%f, %f, %f, %d, %lu ms\n",
                       data.x,data.y,data.z, status, time_now.tv_sec*1000 + time_now.tv_usec/1000);
        if (ret > 0) {
            write(fd, line, ret);
        }
    }
}
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
void data_log_output_step_detect(int fd, uint64_t value)
{
    char line[256] = "";
    int ret;

    if (-1 != fd)
    {
        time_t now = time(NULL);
        struct tm timenow = {0};
        char strTmp[50] = {0};

        localtime_r(&now, &timenow);
        strftime(strTmp, 50, "%H:%M:%S", &timenow);
        ret = snprintf(line, ARRAY_SIZE(line) - 1,
                       "%lld, %s \n",
                       value, strTmp);
        if (ret > 0) {
            write(fd, line, ret);
        }

    }
}
#endif


#endif