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
 * @file         BstSensor.cpp
 * @date         "Wed Jul 10 16:42:40 2013 +0800"
 * @commit       "0bdd1f5"
 *
 * @brief
 *
 * @detail
 *
 */


#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <time.h>

#include <linux/fs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>


#include "BstSensor.h"
#include "BstSensorInfo.h"
#include "sensors.h"

#include "TargetPlatform.h"

#define GET_HANDLES_TRY_NUM         20

const int BstSensor::s_tab_id2handle[BST_SENSOR_NUM_MAX] = {
    BST_SENSOR_HANDLE_ACCELERATION,             /* 0 */
#ifdef __USECASE_TYPE_ACCEL_ONLY__
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    BST_SENSOR_HANDLE_GEST_FLIP,
#else
    BST_SENSOR_HANDLE_MAGNETIC_FIELD,           /* 1 */
    BST_SENSOR_HANDLE_GYROSCOPE,                /* 2 */
    BST_SENSOR_HANDLE_ORIENTATION,              /* 3 */
    BST_SENSOR_HANDLE_GRAVITY,                  /* 4 */
    BST_SENSOR_HANDLE_LINEAR_ACCELERATION,      /* 5 */
    BST_SENSOR_HANDLE_ROTATION_VECTOR,          /* 6 */
    BST_SENSOR_HANDLE_PRESSURE,                 /* 7 */
    BST_SENSOR_HANDLE_GEST_FLIP,                /* 8 */
#ifdef __UNSTANDARD_SENSOR_SUPPORT__
    BST_SENSOR_HANDLE_SW_SIGNIFICANT_MOTION,
#else
    -1,
#endif
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
    BST_SENSOR_HANDLE_GAME_ROTATION_VECTOR,
    BST_SENSOR_HANDLE_GYROSCOPE_UNCALIBRATED,
    BST_SENSOR_HANDLE_MAGNETIC_FIELD_UNCALIBRATED,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    BST_SENSOR_HANDLE_GEOMAGNETIC_ROTATION_VECTOR,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
#ifdef __UNSTANDARD_SENSOR_SUPPORT__
    BST_SENSOR_HANDLE_STEP_COUNTER,
    BST_SENSOR_HANDLE_STEP_DETECTOR,
#else
    -1,
    -1,
#endif
#endif
#endif
    -1,                                         /* 11 */
    -1                                          /* 12 */
};


const int BstSensor::s_tab_handle2id[BST_SENSOR_NUM_MAX] = {
    ID_A,               /* 1 */
    ID_M,               /* 2 */
    ID_O,               /* 3 */
    ID_GY,              /* 4 */
    -1,                 /* 5 */
    ID_PR,              /* 6 */
    -1,                 /* 7 */
    -1,                 /* 8 */
    ID_GR,              /* 9 */
    ID_LA,              /* 10 */
    ID_RV,              /* 11 */
    -1,                 /* 12 */
    ID_GEST_FLIP,        /* 13 */
    ID_SGM,
#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
    ID_GRV,              /* 9: game rotation vector */
    ID_GYU,              /* 10: gyro uncalibrated */
    ID_MU,              /* 11: magnetic uncalibrated */
#endif

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    ID_GEOM_RV,
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
    ID_STC,
    ID_STD,
#endif
};

int8_t handles[BST_SENSOR_NUM_MAX];
int numOfHandles = 0;

int BstSensor::initStorage() {
    char *path = NULL;
    int err = 0;
    struct stat st;
    int retry = 0;

    path = (char *) PATH_DIR_SENSOR_STORAGE;
    err = access(path, F_OK);
    if (!err) {
        st.st_mode = ~S_IFDIR;
        err = stat(path, &st);
        if (!S_ISDIR(st.st_mode)) {
            LOGE("<BST>" "a file of the same name exists");
            /* mark error explicitly for creating the dir */
            err = -ENOTDIR;
        }
    }

    while (0 != err && retry < 4) {
        LOGE("<BST>" "create path of storage, err: %d, retry count %d", err, retry);
        unlink(path);
        err = mkdir(path, 0775);
        if (err) {
            LOGE("<BST>" "error creating storage dir");
        }
        retry++;
        usleep(1000);
    }

    return err;
}


int BstSensor::initIPC() {
    int err = 0;
    char *filename;

    err = initStorage();
    if (err) {
        LOGE("initStorage return %d", err);
        return err;
    }

    filename = (char *) FIFO_CMD;
    err = access(filename, F_OK | R_OK | W_OK);
    if (err) {
        LOGE("<BST> " "can not access %s", filename);
        return err;
    }

    mCmdFd = open(filename, O_RDWR);
    if (mCmdFd < 0) {
        LOGE("<BST> " "error openning file: %s", filename);
        mCmdFd = -abs(errno);
        err = mCmdFd;
        return err;
    }

    filename = (char *) FIFO_DAT;
    err = access(filename, F_OK | R_OK | W_OK);
    if (err) {
        LOGE("<BST> " "can not access %s", filename);
        return err;
    }

    data_fd = open(filename, O_RDWR);
    if (data_fd < 0) {
        LOGE("<BST> " "error openning file: %s", filename);
        err = data_fd;
        return err;
    }

    data_name = filename;

    return err;
}

BstSensor::BstSensor()
: SensorBase(NULL, NULL),
mEnabled(0) {
    struct exchange cmd;
    int i = 0;
    int ret = 0;
    int fData = -1;

    do {
        if (i++ > GET_HANDLES_TRY_NUM) {
            LOGE("initIPC failed:%d", ret);
            return;
        }
        ret = initIPC();
        if (!ret) {
            break;
        }
        sleep(1);
    } while (ret);

    /* if handles already initialized */
    if (numOfHandles > 0) {
        return;
    }

    fData = open(FIFO_DAT, O_RDONLY | O_NONBLOCK);
    if (fData == -1) {
        LOGE("open fifo data failed:%d", fData);
        return;
    }

    memset(&cmd, 0, sizeof(cmd));
    cmd.magic = CHANNEL_PKT_MAGIC_LIST;
    while (numOfHandles == 0) {
        int ret = 0;
        ret = write(mCmdFd, &cmd, sizeof(cmd));
        sleep(1);
        numOfHandles = read(fData, handles,
                            BST_SENSOR_NUM_MAX);
        LOGD("BstSensor write ret %d read num = %d", ret, numOfHandles);
        if (i++ > GET_HANDLES_TRY_NUM) {
            LOGE("reading num of handles time out\n");
            break;
        }
    }
    close(fData);
}

BstSensor::~BstSensor() {
    /* mCmdFd is added by BstSensor, while
     * data_fd is added by SensorBase
     * so here, only close mCmdFd */
    if (mCmdFd >= 0) {
        close(mCmdFd);
    }
}


int BstSensor::id2handle(int32_t id) {
    if ((id <= ID_SENSOR_BASE_BST) ||
            (id >= ID_SENSOR_END_BST)) {
        return -1;
    } else {
        return BstSensor::s_tab_id2handle[id - 1 - ID_SENSOR_BASE_BST];
    }
}


int BstSensor::handle2id(int32_t handle) {
    if ((handle - 1 < 0) || (handle - 1 >= BST_SENSOR_NUM_MAX)) {
        LOGE("<BST> " "Invalid handle %d", handle);
        return -1;
    } else {
        return BstSensor::s_tab_handle2id[handle - 1];
    }
}


int BstSensor::enable(int32_t id, int enable) {
    int err = 0;
    int handle;
    struct exchange cmd;
    int pos = (int) id;

    const struct sensor_t *s;

    handle = BstSensor::id2handle(id);
    if (-1 == handle) {
        return -EINVAL;
    }

    if (mCmdFd < 0) {
        LOGE("<BST> " "cannot tx cmd: %s",
             (char *) strerror(-mCmdFd));
        return mCmdFd;
    }

    enable = !!enable;


    s = BstSensorInfo::getSensor(id);
    LOGI("<BST> " "%s sensor <%s>",
         enable ? "enable" : "disable",
         (s != NULL) ? s->name : "unknown");

    cmd.magic = CHANNEL_PKT_MAGIC_CMD;
    cmd.command.cmd = SET_SENSOR_ACTIVE;
    cmd.command.code = handle;
    cmd.command.value = enable;
    cmd.ts = 0;
    err = write(mCmdFd, &cmd, sizeof(cmd));
    err = err < (int) sizeof(cmd) ? -1 : 0;

    if (!err) {
        if (enable) {
            mEnabled |= (1 << pos);
        } else {
            mEnabled &= ~(1 << pos);
        }
    }

    return err;
}


int BstSensor::setDelay(int32_t id, int64_t ns) {
    int err = 0;
    int handle;
    struct exchange cmd;
    const struct sensor_t *s;

    handle = BstSensor::id2handle(id);
    if (-1 == handle) {
        return -EINVAL;
    }

    if (mCmdFd < 0) {
        LOGE("<BST> " "cannot tx cmd: %s",
             (char *) strerror(-mCmdFd));
        return mCmdFd;
    }

    cmd.magic = CHANNEL_PKT_MAGIC_CMD;

    cmd.command.cmd = SET_SENSOR_DELAY;
    cmd.command.code = handle;
    cmd.command.value = ns / SCALE_TIME_MS2NS;
    cmd.ts = 0;

    s = BstSensorInfo::getSensor(id);
    LOGI("<BST> " "set delay of <%s> to %jdms",
         (s != NULL) ? s->name : "unknown",
         ns / SCALE_TIME_MS2NS);

    err = write(mCmdFd, &cmd, sizeof(cmd));
    err = err < (int) sizeof(cmd) ? -1 : 0;

    if (!err) {
        mDelays[id - ID_SENSOR_BASE_BST] = ns / SCALE_TIME_MS2NS;
    }

    return err;
}


int BstSensor::readEvents(sensors_event_t *pdata, int count) {
    int rslt;
    int err;
    struct exchange sensor_data;
    int sensor;
    sensors_event_t *pdata_cur;
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
    sensors_meta_data_event_t *p_flush_finish_event;
#endif

    if (count > 1) {
        count = 1;
    }
    rslt = 0;
    pdata_cur = pdata;
    while (rslt < count) {
#if 0
        fds.revents = 0;
        err = poll(&fds, 1, BST_DATA_POLL_TIMEOUT);
        if (err <= 0) {
            return rslt;
        }
#endif

        err = read(data_fd, &sensor_data, sizeof(sensor_data));
        if (err < (int) sizeof(sensor_data)) {
            LOGE("<BST> " "bad condition, stream needs sync");
            return rslt;
        }

        if (CHANNEL_PKT_MAGIC_DAT != sensor_data.magic) {
            LOGE("<BST> " "discard invalid data packet from stream");
            return rslt;
        }

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        if (SENSOR_TYPE_META_DATA == sensor_data.data.type) {
            sensor = sensor_data.data.sensor;
            p_flush_finish_event = pdata_cur;
            p_flush_finish_event->version = META_DATA_VERSION;
            p_flush_finish_event->type = SENSOR_TYPE_META_DATA;
            p_flush_finish_event->meta_data.what = META_DATA_FLUSH_COMPLETE;
            p_flush_finish_event->meta_data.sensor = BstSensor::handle2id(sensor);
            PINFO("<BST> " "report flush finish event for sensor id: %d",
                  p_flush_finish_event->meta_data.sensor);
            goto FLUSH_FINISH_EVENT_HANDLED;
        }
#endif
        sensor = sensor_data.data.sensor;
        pdata_cur->version = sizeof(*pdata_cur);
        pdata_cur->sensor = BstSensor::handle2id(sensor);
        pdata_cur->timestamp = sensor_data.ts;

        switch (pdata_cur->sensor) {
        case SENSORS_ACCELERATION_HANDLE:
            pdata_cur->acceleration.x = sensor_data.data.acceleration.x;
            pdata_cur->acceleration.y = sensor_data.data.acceleration.y;
            pdata_cur->acceleration.z = sensor_data.data.acceleration.z;
            pdata_cur->acceleration.status =
                sensor_data.data.status;
            pdata_cur->type = SENSOR_TYPE_ACCELEROMETER;
            break;
        case SENSORS_GYROSCOPE_HANDLE:
            pdata_cur->gyro.x = -sensor_data.data.gyro.x;
            pdata_cur->gyro.y = sensor_data.data.gyro.y;
            pdata_cur->gyro.z = sensor_data.data.gyro.z;
            pdata_cur->gyro.status = sensor_data.data.status;
            pdata_cur->type = SENSOR_TYPE_GYROSCOPE;
            break;
        case SENSORS_MAGNETIC_FIELD_HANDLE:
            pdata_cur->magnetic.x = sensor_data.data.magnetic.x;
            pdata_cur->magnetic.y = sensor_data.data.magnetic.y;
            pdata_cur->magnetic.z = sensor_data.data.magnetic.z;
            pdata_cur->magnetic.status = sensor_data.data.status;
            pdata_cur->type = SENSOR_TYPE_MAGNETIC_FIELD;
            break;
        case SENSORS_ORIENTATION_HANDLE:
            pdata_cur->orientation.azimuth = 360.0f - sensor_data.data.orientation.azimuth;
            pdata_cur->orientation.pitch = sensor_data.data.orientation.pitch;
            pdata_cur->orientation.roll = sensor_data.data.orientation.roll;
            pdata_cur->orientation.status = sensor_data.data.status;
            pdata_cur->type = SENSOR_TYPE_ORIENTATION;
            break;
        case SENSORS_PRESSURE_HANDLE:
            pdata_cur->pressure = sensor_data.data.pressure;
            pdata_cur->type = SENSOR_TYPE_PRESSURE;
            break;
        case SENSORS_GRAVITY_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->type = SENSOR_TYPE_GRAVITY;
            break;
        case SENSORS_LINEAR_ACCEL_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->type = SENSOR_TYPE_LINEAR_ACCELERATION;
            break;
        case SENSORS_ROTATION_VECTOR_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
            pdata_cur->data[3] = sensor_data.data.data[3];
            pdata_cur->data[4] = sensor_data.data.data[4];
#endif
            pdata_cur->type = SENSOR_TYPE_ROTATION_VECTOR;
            break;
        case SENSORS_GEST_FLIP_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->type = BST_SENSOR_TYPE_GEST_FLIP;
            break;

#ifdef __UNCALIBRATED_VIRTUAL_SENSOR_SUPPORT__
        case SENSORS_GAME_ROTATION_VECTOR_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->data[3] = sensor_data.data.data[3];
            pdata_cur->data[4] = sensor_data.data.data[4];
            pdata_cur->type = SENSOR_TYPE_GAME_ROTATION_VECTOR;
            break;
        case SENSORS_GYROSCOPE_UNCALIBRATED_HANDLE:
            pdata_cur->uncalibrated_gyro.x_uncalib = -sensor_data.data.uncalibrated_gyro.x_uncalib;
            pdata_cur->uncalibrated_gyro.y_uncalib = sensor_data.data.uncalibrated_gyro.y_uncalib;
            pdata_cur->uncalibrated_gyro.z_uncalib = sensor_data.data.uncalibrated_gyro.z_uncalib;
            pdata_cur->uncalibrated_gyro.x_bias = -sensor_data.data.uncalibrated_gyro.x_bias;
            pdata_cur->uncalibrated_gyro.y_bias = sensor_data.data.uncalibrated_gyro.y_bias;
            pdata_cur->uncalibrated_gyro.z_bias = sensor_data.data.uncalibrated_gyro.z_bias;
            pdata_cur->type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
            break;
        case SENSORS_MAGNETIC_UNCALIBRATED_HANDLE:
            pdata_cur->uncalibrated_magnetic.x_uncalib = sensor_data.data.uncalibrated_magnetic.x_uncalib;
            pdata_cur->uncalibrated_magnetic.y_uncalib = sensor_data.data.uncalibrated_magnetic.y_uncalib;
            pdata_cur->uncalibrated_magnetic.z_uncalib = sensor_data.data.uncalibrated_magnetic.z_uncalib;
            pdata_cur->uncalibrated_magnetic.x_bias = sensor_data.data.uncalibrated_magnetic.x_bias;
            pdata_cur->uncalibrated_magnetic.y_bias = sensor_data.data.uncalibrated_magnetic.y_bias;
            pdata_cur->uncalibrated_magnetic.z_bias = sensor_data.data.uncalibrated_magnetic.z_bias;
            pdata_cur->type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
            break;
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        case SENSORS_GEOMAGNETIC_ROTATION_VECTOR_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->data[3] = sensor_data.data.data[3];
            pdata_cur->data[4] = sensor_data.data.data[4];
            pdata_cur->type = SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR;
            break;
#endif
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_0__
        case SENSORS_STEP_COUNTER_HANDLE:
#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
            pdata_cur->u64.step_counter = sensor_data.data.step_counter;
            pdata_cur->u64.data[0] = sensor_data.data.step_counter;
            pdata_cur->u64.data[1] = 0;
            pdata_cur->u64.data[2] = 0;
#else
            pdata_cur->step_counter = sensor_data.data.step_counter;
            pdata_cur->data[0] = sensor_data.data.step_counter;
            pdata_cur->data[1] = 0;
            pdata_cur->data[2] = 0;
#endif
            pdata_cur->type = SENSOR_TYPE_STEP_COUNTER;
            break;
        case SENSORS_STEP_DETECTOR_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = 0;
            pdata_cur->data[2] = 0;
            pdata_cur->type = SENSOR_TYPE_STEP_DETECTOR;
            break;
#endif
        case SENSORS_SW_SIGNIFICANT_MOTION_HANDLE:
            pdata_cur->data[0] = sensor_data.data.data[0];
            pdata_cur->data[1] = sensor_data.data.data[1];
            pdata_cur->data[2] = sensor_data.data.data[2];
            pdata_cur->type = BSTEXT_SENSOR_TYPE_SW_SGM;
            break;


        default:
            LOGE("<BST> " "Invalid data pkt");
            return rslt;
        }

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__
        FLUSH_FINISH_EVENT_HANDLED:
#endif
        rslt++;
        pdata_cur++;
    }

    return rslt;
}

int BstSensor::getSensorList(struct sensor_t *list, int len) {
    int num = 0;
    int i, j;
    int handlerID = 0;

    UNUSED_PARAM(len);
    for (i = 0; i < numOfHandles; i++) {
        for (j = 0; j < NUM_SENSORS; j++) {
            handlerID = id2handle(BstSensorInfo::g_bst_sensor_list[j].handle);
            if (handles[i] == handlerID) {
                memcpy(list++,
                       &BstSensorInfo::g_bst_sensor_list[j],
                       sizeof(struct sensor_t));
                num++;
            }
        }
    }

    LOGI("<BST> " "BST HAL version: %s",
         get_hal_version());
    LOGI("build date : %s", get_build_date());

    return num;
}

#if __HAL_VER__ >= __SENSORS_DEVICE_API_VERSION_1_1__

int BstSensor::flush(int id) {
    int err = 0;
    int ret = 0;
    struct exchange cmd;
    const struct sensor_t *s;
    int handle;
    if (mCmdFd < 0) {
        PERR("<BST> " "cannot tx cmd: %s",
             (char *) strerror(-mCmdFd));
        return mCmdFd;
    }
    handle = BstSensor::id2handle(id);
    if (-1 == handle) {
        PERR("<BST> " "flush: cannot get handle for id %d", id);
        return 0;
    }
    cmd.magic = CHANNEL_PKT_MAGIC_CMD;
    cmd.command.cmd = SET_SENSOR_FLUSH;
    cmd.command.code = handle;
    s = BstSensorInfo::getSensor(id);
    PINFO("<BST> " "flush <%s>, id: %d",
          (s != NULL) ? s->name : "unknown", id);
    err = write(mCmdFd, &cmd, sizeof(cmd));
    ret = err < (int) sizeof(cmd) ? -1 : 0;
    return ret;
}

int BstSensor::batch(int id, int flags, int64_t period_ns, int64_t timeout) {
    int ret = 0;

    UNUSED_PARAM(flags);
    UNUSED_PARAM(timeout);
    ret = BstSensor::setDelay(id, period_ns);
    return ret;
}

#endif
