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

list="query_flow"
for dirname in $list
#find . -maxdepth 1 -mindepth 1 -type d | grep -v svn | sort | while read dirname
do
	echo $dirname
	build_dir $dirname
	ret=$?
	if [ $ret -ne 0 ]; then
        	exit 1
		if [ $ret -ne 2 ]; then
			exit 1
		fi
	fi
done
RET=$?

exit $RET

