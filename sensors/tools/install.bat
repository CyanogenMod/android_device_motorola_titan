echo off
set acc_name=bma255
set gyro_name=bmg160
set mag_name=bmm050
set usecase=ndof
set REAL_LOCAL_PATH=.
set axis_config=samsung_note

rem specify the suffix name of sensord,
rem ex. if want to use "sensord.ld"
rem set suffix as ld will install the sensord.ld instead of "sensord"
set sensord_suffix=



echo =====================================================
echo installing for %acc_name% %mag_name% %gyro_name%...
adb remount
adb shell rm /system/etc/sensor/*
adb shell rm /data/misc/sensor/*
adb shell rm /system/lib/hw/sensors.*
echo ==================================
echo installing spec...
adb push %REAL_LOCAL_PATH%/spec/accel_spec_%acc_name%.txt /system/etc/sensor/accel_spec.txt
adb push %REAL_LOCAL_PATH%/spec/mag_spec_%mag_name%.txt /system/etc/sensor/mag_spec.txt
adb push %REAL_LOCAL_PATH%/spec/gyro_spec_%gyro_name%.txt /system/etc/sensor/gyro_spec.txt
adb push %REAL_LOCAL_PATH%/spec/usecase_%usecase%.txt /system/etc/sensor/
adb push %REAL_LOCAL_PATH%/spec/softiron_matrix.txt /system/etc/sensor/
adb push %REAL_LOCAL_PATH%/spec/axis_cfg_%axis_config%.txt /system/etc/sensor/sensord_cfg_axis
echo ==================================
echo installing sensord and sensors.so
if exist "libsensors.default.so" (
move /Y libsensors.default.so sensors.default.so
)
if defined sensord_suffix (
echo installing sensord.%sensord_suffix%
adb push %REAL_LOCAL_PATH%/sensord.%sensord_suffix% /system/bin/sensord
) else (
adb push %REAL_LOCAL_PATH%/sensord /system/bin/sensord
)
adb shell chmod 0777 system/bin/sensord
adb push %REAL_LOCAL_PATH%/sensors.default.so system/lib/hw/
adb push %REAL_LOCAL_PATH%/sensors.default.so system/lib/hw/sensors.exynos4.so
echo ==================================
echo rebooting system
adb reboot
pause
