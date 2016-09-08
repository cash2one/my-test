#!/bin/sh

ARG=""

execute() {
	echo $1
	$1
}

build_dir() {
	cd $1
	echo "in $1"
	./vbuild.sh $ARG
	ret=$?

	cd ..
	return $ret
}

TOP_DIR=`pwd`
ARG=$1
SRC_DIR=$TOP_DIR/src

LIB=$TOP_DIR/lib
SO=$TOP_DIR/so
INCLUDE=$TOP_DIR/include

RET=

#list="common adt config shm trace timer vcrd software_update"
#for dirname in $list
find . -maxdepth 1 -mindepth 1 -type d | grep -v svn | grep -v psql_test|grep -v flow_def | grep -v redis_test | grep -vw flow_proc|grep -v merge_proc |sort | while read dirname
do
	echo $dirname
	build_dir $dirname
	ret=$?
        echo $ret
	if [ $ret -ne 0 ]; then
		exit 1
		if [ $ret -ne 2 ]; then
			exit 1
		fi
	fi
done
RET=$?

exit $RET

