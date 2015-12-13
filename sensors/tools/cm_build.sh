#!/bin/bash

ANDROID_ROOT=/share/cm/cm9
ANDROID_VENDOR_BOSCH=$ANDROID_ROOT/vendor/bosch
BIN_OUTPUT=bin_cm
PRODUCT=n7000

rm $ANDROID_ROOT/out/target/product/$PRODUCT/system/bin/sensord*
rm $ANDROID_ROOT/out/target/product/$PRODUCT/symbols/system/lib/hw/sensors*\

rm $ANDROID_VENDOR_BOSCH -rf
rm $BIN_OUTPUT -rf
mkdir -p $BIN_OUTPUT

mkdir $ANDROID_VENDOR_BOSCH
cp daemon -fr $ANDROID_VENDOR_BOSCH/
cp hal -fr $ANDROID_VENDOR_BOSCH/
cp tools -rf $ANDROID_VENDOR_BOSCH/
cp Android.mk -fr $ANDROID_VENDOR_BOSCH/

pushd $ANDROID_ROOT
. build/envsetup.sh
breakfast $PRODUCT
popd

pushd $ANDROID_VENDOR_BOSCH

mm -B bmc=bmc056 bmg=bmg160
ret=$?
if [ $ret != 0 ]; then
    exit $ret
fi

mm -B bmc=bmc056 bmg=bmg160 debug_trace_level=information sensord_suffix=debug
ret=$?
if [ $ret != 0 ]; then
    exit $ret
fi

mm -B bmc=bmc056 bmg=bmg160 debug_data_log=true sensord_suffix=ld
ret=$?
if [ $ret != 0 ]; then
    exit $ret
fi

mm -B bmc=bmc056 bmg=bmg160 debug_data_log=true scheduling_timestamp_calibrated=true sensor_timestamp_scheduling=true sensord_suffix=ts_cali.ld
ret=$?
if [ $ret != 0 ]; then
    exit $ret
fi

mm -B bmc=bmc056 bmg=bmg160 debug_data_log=true sensor_timestamp_scheduling=true sensord_suffix=ts_sched.ld
ret=$?
if [ $ret != 0 ]; then
    exit $ret
fi

popd

cp $ANDROID_ROOT/out/target/product/$PRODUCT/system/bin/sensord* \
	$BIN_OUTPUT/

cp $ANDROID_ROOT/out/target/product/$PRODUCT/symbols/system/lib/hw/sensors.exynos4.so\
	$BIN_OUTPUT/sensors.default.so

cp ./tools/install.bat $BIN_OUTPUT/
cp ./tools/get_sensord_log.bat $BIN_OUTPUT/
cp ./tools/get_sensord_data.bat $BIN_OUTPUT/
cp ./tools/get_hal_version.bat $BIN_OUTPUT/
cp ./tools/get_sensord_version.bat $BIN_OUTPUT/
cp ./daemon/spec $BIN_OUTPUT/ -rf

echo =================================================
echo static analysis
./tools/cppcheck_daemon.sh $ANDROID_VENDOR_BOSCH/daemon
if [ $? != 0 ] ; then
	exit $?
fi

./tools/cppcheck_hal.sh $ANDROID_VENDOR_BOSCH/hal
if [ $? != 0 ] ; then
	exit $?
fi

echo build success!
