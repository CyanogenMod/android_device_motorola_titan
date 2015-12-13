if [ "$1" == "" ] ;  then
echo "usage: $0 daemon_path"
exit 1
fi

DIR_SCRIPT=$PWD/$(dirname $0)

cd $1

cppcheck -v --error-exitcode=255 --enable=all --inconclusive --std=posix --suppress=variableScope --quiet \
	--suppressions-list="$DIR_SCRIPT/cppcheck_suppress_daemon.lst"\
	-I .\
	-I ../version\
	-I ./algo/inc\
	-I ./inc\
	-I ./src/algo\
	-I ./src/hw\
	-I ./src/hw/a\
	-I ./src/hw/m\
	-I ./src/hw/g\
	-i ./src/hw/g-mpu3050\
	-i ./src/hw/g-itg3200\
	-D USHRT_MAX=0XFFFF\
	-D INT_MAX=0X7FFFFFFF\
	--template='{file}:{line},{severity},[{id}],{message}'\
	.

ret=$?
if [ $ret != 0 ]; then
    echo "cppcheck detected errors: $ret"
    exit $ret
fi

echo "$1 passed static analysis by cppcheck!"
