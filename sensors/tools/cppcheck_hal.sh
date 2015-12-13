if [ "$1" == "" ] ;  then
echo "usage: $0 hal_path"
exit 1
fi

cd $1

cppcheck -v --error-exitcode=255 --enable=all --inconclusive --std=posix --suppress=variableScope --quiet \
	-i sensors.cpp\
	-i SensorBase.cpp\
	-i LightSensor.cpp\
	-i ProximitySensor.cpp\
	-i InputEventReader.cpp\
	-i PressureSensor.cpp\
	--template='{file}:{line},{severity},[{id}],{message}'\
	-D BST_SENSOR_PLACE_A=0\
	.

ret=$?
if [ $ret != 0 ]; then
    echo "cppcheck detected errors: $ret"
    exit $ret
fi

echo "$1 passed static analysis by cppcheck!"
