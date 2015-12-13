echo retreiving sensor data log...
adb remount
adb shell mkdir data/misc/sensor/tmp
adb shell mv data/misc/sensor/sensor_data_*.log data/misc/sensor/tmp/
adb shell "cd data/misc/sensor/tmp;for file in *; do mv \"$file\" `echo $file | sed -e 's/  */_/g' -e 's/:/-/g'`; done"
adb pull data/misc/sensor/tmp .
adb shell mv data/misc/sensor/tmp/* data/misc/sensor/
adb shell rmdir data/misc/sensor/tmp
pause
