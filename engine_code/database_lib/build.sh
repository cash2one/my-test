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

RECREATE_DIR() {
	rm -rf $1
	mkdir $1
}

TOP_DIR=`pwd`
ARG=$1
SRC_DIR=$TOP_DIR/src

LIB=$TOP_DIR/lib
SO=$TOP_DIR/so
INCLUDE=$TOP_DIR/include
RET=

RECREATE_DIR $LIB
RECREATE_DIR $SO
RECREATE_DIR $INCLUDE

cd $SRC_DIR
#list="common adt config shm trace timer vcrd software_update"
#for dirname in $list
find . -maxdepth 1 -mindepth 1 -type d | grep -v svn | sort | while read dirname
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

cd $TOP_DIR
if [ "$1" = "-c" ] || [ "$1" = "-C" ]; then
	rm -rf $LIB $INCLUDE $SO
	exit 0
fi

find $SRC_DIR -maxdepth 3 -mindepth 3 -name "lib*\.a" -type f -exec cp -f {} $LIB \;
find $SRC_DIR -maxdepth 3 -mindepth 3 -name "lib*\.so" -type f -exec cp -f {} $SO \;

PSQL_HEAD="$SRC_DIR/postgres/include/gms_psql.h $SRC_DIR/postgres/include/store_common.h $SRC_DIR/postgres/include/gms_tables.h" 
REDIS_HEAD="$SRC_DIR/redis/include/gms_redis.h $SRC_DIR/redis/include/hiredis.h $SRC_DIR/redis/include/gms_keys.h"

HEAD="$PSQL_HEAD $REDIS_HEAD"
cp -f $HEAD $INCLUDE

echo "Database lib build DONE!"
exit $RET

