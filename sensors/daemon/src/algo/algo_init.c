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
 * @file         algo_init.c
 * @date         "Mon Jul 29 16:02:28 2013 +0800"
 * @commit       "5f0e168"
 *
 * @brief
 *
 * @detail
 *
 */

#define LOG_TAG_MODULE "<algo_init_bst>"
#include "sensord.h"

#define ALGO_SOFTIRON_MATRIX           "softiron_matrix.txt"

extern const ts_sensmatrix softiron_default;
extern const unsigned char bma_spec[];
extern const unsigned char bmg160_spec[];
extern const unsigned char bmm_spec[];
extern const unsigned char usecase_spec[];

void algo_check_cust_param_setting(void);

/*configuration setting parameters for customize*/
static int g_compass_heading_sensitivity = -1;
static int g_ndof_ori_correct_speed = -1;
static int g_mag_filt_mode = -1;
static int g_mag_calib_acc_sensitivity = -1;
static int g_mag_calib_speed = -1;
static int g_compass_magcalib_autorecmode = -1;
static float g_process_noise_diag[4] = {0.009,0.009,0.009,0.081};
static float g_acc_filt_param1 = -1.0;
static float g_acc_filt_param2 = -1.0;

#ifdef __RESET_CALIBRATION_PROFILE__
/*
 * @brief This function initialize calibration profile
 *
 * @param none
 *
 * @return none
 */
void algo_sensor_profile_init(void)
{
	remove((char*)SENSOR_CFG_FILE_SYS_PROFILE_CALIB_A);
	remove((char*)SENSOR_CFG_FILE_SYS_PROFILE_CALIB_M);
	remove((char*)SENSOR_CFG_FILE_SYS_PROFILE_CALIB_G);
}
#endif

/*
 * @brief This function returns current use case
 *
 * @param none
 *
 * @return use case id relate with ALGO_FUSION_MODE
 */
int algo_get_current_usecase(void)
{
#ifdef __USECASE_TYPE_NDOF__
	return ALGO_FUSION_MODE_9DOF;
#elif defined __USECASE_TYPE_COMPASS__
	return ALGO_FUSION_MODE_COMPASS;
#elif defined __USECASE_TYPE_M4G__
	return ALGO_FUSION_MODE_M4G;
#elif defined __USECASE_TYPE_IMU__ || defined __USECASE_TYPE_ACCEL_ONLY__
	return ALGO_FUSION_MODE_IMU;
#else
#warning no usecase defined, will use NDOF as default usecase.
	return ALGO_FUSION_MODE_9DOF;
#endif
}


/*!
 * @brief This function reads a text pure numbered spec file,
 * and transfer them to float numbered buffer
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 */
int algo_init_load_f32(const char *filename, BSX_F32 **pp_matrix, int num)
{
	int                 len;
	char               *p_str = NULL, *p_curr = NULL;
	float              *p_data = NULL;
	int                 err = 0;
	char                tmp[64] = {0};
	int                 tmp_idx, data_idx;
	int                 end_parsing = 0;

	len = util_fs_get_filesize(filename);
	if (len <= 0)
	{
		return -1;
	}
	p_str = malloc(len + 2);
	if (p_str == NULL)
	{
		err = -1;
		goto exit;
	}
	len = util_fs_read_file(filename, p_str, len);
	p_str[len] = 0;
	if (len == -1)
	{
		err = -1;
		goto exit;
	}

	p_data = malloc(num * sizeof(float));
	if (NULL == p_data)
	{
		err = -1;
		goto exit;
	}

	data_idx = 0;
	p_curr = p_str;
	while(len >= (p_curr - p_str))
	{
		/* make sure the string start with number or minus symbol*/
		while(((*p_curr > '9') || (*p_curr < '0')) && (*p_curr != '-'))
		{
			if (*p_curr == 0) {
				end_parsing = 1;
				break;
			}
			p_curr ++;
		}
		if (end_parsing) {
			break;
		}
		tmp_idx = 0;
		memset(tmp, 0, sizeof(tmp));
		while(((*p_curr <= '9') && (*p_curr >= '0'))
			|| (*p_curr == '.') || (*p_curr == '-')){
			tmp[tmp_idx ++] = *p_curr++;
		}
		tmp[tmp_idx] = 0;
		p_data[data_idx] = atof(tmp);
		printf("float data%s, is %f\n",tmp, p_data[data_idx]);
		data_idx ++;
		if (data_idx >= num)
		{
			break;
		}
	}

exit:
	if (p_str) {
		free(p_str);
	}
	if (p_data) {
		*pp_matrix = p_data;
	}
	return err;
}

/*!
 * @brief This function initialize paramters for bsx library,
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 */
int algo_init_parameters(void)
{
	BSX_U8 ret = 0;

#ifdef CUST_ORIENTATION_FILT_COEF
#ifdef CUST_ORIENTATION_FILT_DYNA
	ret = bsx_set_compass_filterparam(CUST_ORIENTATION_FILT_COEF,
			CUST_ORIENTATION_FILT_DYNA);
	if (ret)
	{
		PERR("failed to set filter:%d\n", ret);
		return ret;
	}
	PINFO("configure compass filter param:%f,%f\n",
			CUST_ORIENTATION_FILT_COEF,
			CUST_ORIENTATION_FILT_DYNA);
#endif
#endif

#ifdef CUST_ORIENTATION_FILT_MODE
	ret = bsx_set_compass_filtermode(CUST_ORIENTATION_FILT_MODE);
	if (ret)
	{
		PERR("failed to set filter mode:%d\n", ret);
		return ret;
	}
	PINFO("configed compass filter mode as:%d\n", CUST_ORIENTATION_FILT_MODE);
#endif

#ifdef CUST_MAGNETIC_ACCURACY_THRES
	ret = bsx_set_magcalib_accuracythreshold(CUST_MAGNETIC_ACCURACY_THRES);
	if (ret)
	{
		PERR("failed to set magnetic accuracy threshold:%d\n", ret);
		return ret;
	}
	PINFO("configed magnetic accuracy thresshold:%f\n", CUST_MAGNETIC_ACCURACY_THRES);
#endif

	return ret;
}


/*!
 * @brief This function read customize parameters from local config file
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 */
static void algo_load_cust_param(void)
{
	char *filename = SENSOR_CFG_FILE_ALGO;

	int fd;
	char buf[80] = "";
	int ret;
	int p[20];
	float noise[4] = {0.0};
	float acc_filt_param1 = -1.0, acc_filt_param2 = -1.0;

	fd = open(filename, O_RDONLY);
	if (-1 != fd)
	{
		ret = read(fd, buf, sizeof(buf) - 1);
		close(fd);


		if (ret >= 0)
		{

			ret = sscanf(buf, "%10d,%10d,%10d,%10d,%10d,%10d,%10f,%10f,%10f,%10f,%10f,%10f",
					&p[0],&p[1],&p[2],&p[3],&p[4],&p[5],
					&noise[0],&noise[1],&noise[2],&noise[3],
					&acc_filt_param1,&acc_filt_param2);

			if (12 <= ret)
			{
				PINFO("config of param: %d %d %d %d %d %d %f %f %f %f %f %f",
					p[0],p[1],p[2],p[3],p[4],p[5],
					noise[0],noise[1],noise[2],noise[3],
					acc_filt_param1,acc_filt_param2);
				g_compass_heading_sensitivity = p[0];
				g_ndof_ori_correct_speed = p[1];
				g_mag_filt_mode = p[2];
				g_mag_calib_speed = p[3];
				g_mag_calib_acc_sensitivity = p[4];
				g_compass_magcalib_autorecmode = p[5];
				g_process_noise_diag[0] = noise[0];
				g_process_noise_diag[1] = noise[1];
				g_process_noise_diag[2] = noise[2];
				g_process_noise_diag[3] = noise[3];
				g_acc_filt_param1 = acc_filt_param1;
				g_acc_filt_param2 = acc_filt_param2;
			}
			else
			{
				PERR("invalid content: %s", buf);
			}
		}
		else
		{
			PINFO("read error");
		}
	}
	else
	{
		PINFO("no cfg file for algo");
	}
}

/*!
 * @brief This function set cust parameters to bsx library
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 *
*/
static void algo_init_cust_param(void)
{
	int ret = 0;
	algo_load_cust_param();
	PINFO("algo_init_cust_param");

	/*
	* Please be noted that the sequence of calling the API's should be in the same order as listed below
	*/

	if (-1 != g_compass_heading_sensitivity)
	{
		ret = bsx_set_compass_headingsensitivity(g_compass_heading_sensitivity);
		PINFO("set compass heading sensitivity ret %d", ret);
	}
#ifdef __USECASE_TYPE_NDOF__
	if (-1 != g_ndof_ori_correct_speed)
	{
		ret = bsx_set_ndof_orientcorrectionspeed(g_ndof_ori_correct_speed);
		PERR("set ndof orient correction speed ret %d", ret);
	}
#endif
	if (-1 != g_mag_filt_mode)
	{
		ret = bsx_set_magfiltermode(g_mag_filt_mode);
		PINFO("set magfiltermode ret %d", ret);
	}

	if (-1 != g_mag_calib_speed)
	{
		ret = bsx_set_magcalibrationspeed(g_mag_calib_speed);
		PINFO("set magcalib ration speed ret %d", ret);
	}

	if (-1 != g_mag_calib_acc_sensitivity)
	{
		ret = bsx_set_magcalibacc_sensitivity(g_mag_calib_acc_sensitivity);
		PINFO("set magcalibacc sensitivity ret %d", ret);
	}

	if (-1 != g_compass_magcalib_autorecmode)
	{
		ret = bsx_set_compass_magcalibaccautorecmode(g_compass_magcalib_autorecmode);
		PINFO("set compass magcalib acc auto rec mode ret %d", ret);
	}

	if (g_process_noise_diag[0] > -1.0 && g_process_noise_diag[1] > -1.0
			&& g_process_noise_diag[2] > -1.0 && g_process_noise_diag[3] > -1.0)
	{
#if (defined __USECASE_TYPE_NDOF__) || (defined __USECASE_TYPE_COMPASS__) || (defined __USECASE_TYPE_M4G__)
		ret = bsx_set_magcalib_processnoise(g_process_noise_diag);
		PINFO("set magcalib process noise ret %d", ret);
#endif
	}

	if (g_acc_filt_param1 > -1.0 && g_acc_filt_param2 > -1.0)
	{
		ret = bsx_set_compass_accel_filterparam(g_acc_filt_param1, g_acc_filt_param2);
		PINFO("set compass accel filt param ret %d", ret);
	}

}
/*!
 * @brief This function loads spec files from file system and set them into
 *            bsx_init, after that it calls algo_adapter_init to set up
 *            sensor_hw and working modes
 *
 * @param none
 *
 * @return 0 success, < 0 failed
 */
int algo_init(void)
{
	int err = 0;
	const BSX_U8*   p_accelspec;
	const BSX_U8*   p_magspec = NULL;
	const BSX_U8*   p_gyrospec = NULL;
	const BSX_U8*   p_usecaseconfig = NULL;
	initParam_t  algo_param;
	int usecase = 0;

#ifdef __SOFTIRON_SUPPORT__
	BSX_F32*  softiron_matrix = NULL;
#endif

	usecase = algo_get_current_usecase();
	/* read accelerator spec file */
	p_accelspec = bma_spec;

	/* read magnetic spec file */
	if (ALGO_FUSION_MODE_IMU == usecase)
	{
		p_magspec = NULL;
	}
	else
	{
		p_magspec = bmm_spec;
	}

#ifdef __USECASE_TYPE_ACCEL_ONLY__
	p_gyrospec = NULL;
#else
	/* read gyroscope spec file */
	if (ALGO_FUSION_MODE_COMPASS == usecase || ALGO_FUSION_MODE_M4G == usecase)
	{
		p_gyrospec = NULL;
	}
	else
	{
#if defined (__BMI055__) || defined (__BMI058__) || defined (__BMG160__) || defined (__BMI160__)
		p_gyrospec = bmg160_spec;
#else
		PERR("failed to find gyroscope spec array\n");
		goto exit;
#endif

	}
#endif
	/* read use case configure file */
	p_usecaseconfig = usecase_spec;

	memset(&algo_param, 0, sizeof(algo_param));
	algo_param.accelspec = (BSX_U8 *)p_accelspec;
	algo_param.magspec   = (BSX_U8 *)p_magspec;
	algo_param.gyrospec  = (BSX_U8 *)p_gyrospec;
	algo_param.usecase   = (BSX_U8 *)p_usecaseconfig;
	err = bsx_init(&algo_param);
	PDEBUG("bsx_init return %d acc status %d mag status %d gyro status %d usecase status %d",
			err, algo_param.accelspec_status, algo_param.magspec_status,
			algo_param.gyrospec_status, algo_param.usecase_status);
	if (err)
	{
		PERR("failed at bsx_init:%d\n", err);
		goto exit;
	}

#ifdef __SOFTIRON_SUPPORT__
	err = algo_init_load_f32(SENSOR_CFG_FILE_PATH ALGO_SOFTIRON_MATRIX,
			(BSX_F32**)&softiron_matrix, 9);
	if (err)
	{
		PWARN("failed to load softiron matrix:%d, use default\n", err);
		softiron_matrix = (BSX_F32*)&softiron_default;
	}

	err = bsx_set_softiron_correctionmatrix(*((ts_sensmatrix*)softiron_matrix));
	if (err)
	{
		PERR("failed to set softiron matrix:%d\n", err);
		goto exit;
	}
#endif

	err = algo_init_parameters();
	if (err)
	{
		PERR("faild at init parameters:%d\n", err);
		goto exit;
	}
	algo_adapter_init();
	algo_init_cust_param();
#ifdef __RESET_CALIBRATION_PROFILE__
	algo_sensor_profile_init();
#endif
exit:

#ifdef __SOFTIRON_SUPPORT__
		if ((softiron_matrix) &&
			(((BSX_F32*)&softiron_default) != softiron_matrix) )
		{
			free(softiron_matrix);
		}
#endif


	return err;
}


