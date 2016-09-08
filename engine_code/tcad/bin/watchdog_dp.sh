#!/bin/bash

filepath=$(cd "$(dirname "$0")"; pwd)

#monitoring interval(second)
INTERVAL=1

#restart interval(second)
INTERVALRESTART=10

#==================================================================
PROGRAM=tcad_gms
#"-restore" for restoring from cache file
PARAMETER=$1
PROCESSNUM=1
MAX_USED_DISK=90
LOG=${filepath}/$PROGRAM.log
OUT=/dev/null
#OUT=${filepath}/$PROGRAM.out

killall -9 $PROGRAM 1>/dev/null 2>&1 
#rm -f $OUT

ipcrm -M 0x0001079b

ulimit -n 1048576
ulimit -c unlimited

manual_start=1
while true
do
    #"**********Now begin to monitor program's status ************"
	pscmd=`ps -e | grep $PROGRAM | grep -v grep | wc -l`
	currentnum=`expr $pscmd`
	if  [  $currentnum -lt $PROCESSNUM ]; then
        SYSTIME=$(date)
		if [ $manual_start -eq 1 ]; then
			echo "======================================================" >> $LOG
			echo "Restart time(manual start):" $SYSTIME >> $LOG
			echo "Program name(manual start):" $PROGRAM $PARAMETER >> $LOG
			echo "======================================================" >> $LOG
			manual_start=0
		else
			echo "======================================================" >> $LOG
			echo "Restart time(new process):" $SYSTIME >> $LOG
			echo "Program name(new process):" $PROGRAM $PARAMETER >> $LOG
			echo "======================================================" >> $LOG
		fi
		${filepath}/$PROGRAM $PARAMETER 1>$OUT 2>&1 &
    fi
	#"**********Now end to monitor program's status !**********"
	#"Check free disk space"
	useddisk=`df |grep -o "[0-9]*[0-9]%"|head -n 1|grep -o "[0-9]*[0-9]"`
    if [ $useddisk -gt $MAX_USED_DISK ]; then
        echo "" > $OUT
    fi
	sleep $INTERVAL
done
	exit 0

