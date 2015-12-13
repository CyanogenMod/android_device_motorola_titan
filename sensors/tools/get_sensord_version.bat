echo off
echo geting sensord version...
adb remount
adb shell head -n 10 data/misc/sensor/sensord.log
pause
