echo off
echo geting HAL version...
adb remount
adb logcat -s bsthal:I
