#!/bin/sh

LOGMON="logmon"
REDIS="redis"                                          
POSTGRES="postgres"                                    
TOMCAT="tomcat"                                        
AUTH_FLAG=/gms/conf/0.ret
REG_PATH=/gms/conf/reg.dat
WTL_PATH=/gms/mtx/conf/wtl.conf
PAG_PATH=/gms/mtx/pag.conf
MTX_PATH=/gms/mtx/
APT_PATH=/gms/apt/online/
VDS_PATH=/vds/script/monitor.sh
STORE_PATH=/gms/storage/store_proc
FLOW_PATH=/gms/storage/flow_proc
MERGE_PATH=/gms/storage/merge_proc
comm_path=/gms/conf/comm_interface_conf.xml
gmsconf_path=/gms/conf/gmsconfig.xml
monitor=`cat $comm_path|grep -v "xml" |sed 's/monitor//g'|awk -F '<>' '{print $2}'|cut -c1|awk 'NF>0'`
root=`cat $comm_path|grep -v "xml" |sed 's/root//g'|awk -F '<>' '{print $2}'|cut -c1|awk 'NF>0'`
cloudcon=`cat $comm_path|grep -v "xml" |sed 's/cloudcon//g'|awk -F '<>' '{print $2}'|cut -c1|awk 'NF>0'`
VDS_FLAG=`cat $gmsconf_path |grep vds|awk -F [=\"] '/dd/{print $3}'|awk 'NF>0'`
APT_FLAG=`cat $gmsconf_path |awk -F [=\"] '/apt status/{print $3}'|awk 'NF>0'`
MTX_FLAG=`cat $gmsconf_path |awk -F [=\"] '/mtx status/{print $3}'|awk 'NF>0'`
XGE_FLAG=`cat $gmsconf_path |awk -F [=\"] '/if_is_monitor ret/{print $3}'|awk 'NF>0'`
STORE_STAT=`ps x|grep -v 'grep'|grep "store_proc"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
COMM_STAT=`ps x|grep -v 'grep'|grep -w "comm_main.pyc"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
CLOUD_STAT=`ps x|grep -v 'grep'|grep "cloud_comm_main.py"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
FLOW_STAT=`ps x|grep -v 'grep'|grep "flow_proc"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
FLOW_DEF_STAT=`ps x|grep -v 'grep'|grep "flow_def"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
MERGE_STAT=`ps x|grep -v 'grep'|grep "merge_proc"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
GUARD_STAT=`ps x|grep -v 'grep'|grep "sys_guard"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
MTX_STAT=`ps x|grep -v 'grep'|grep "mtx -d"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
APT_STAT=`ps x|grep -v 'grep'|grep "apt"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
OFFLINE_STAT=`ps x|grep -v 'grep'|grep "offline"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
URL_STAT=`ps x|grep -v 'grep'|grep "url_detect.py"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
VDS_STAT=`ps x|grep -v 'grep'|grep "vds/dd"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
EVENT_STAT=`ps x|grep -v 'grep'|grep "event_export.py"| awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`

comm_link_stat=`cat $gmsconf_path|awk -F [=\"] '/comm_stat link/{print $3}'|awk 'NF>0'`

stream_num=`grep -r 'stream_num =' $PAG_PATH |awk -F ' ' '{printf $3}'`
time_path=`date +"%Y/%m"`
now_time=`date +"%Y-%m-%d %H:%M:%S"`
last_path="/data/log/logmon/"$time_path
mkdir -p $last_path
log_file=$last_path/logmon-`date +%d`.log
echo  "$now_time **********************OOOOOOOO-------------------|  START  |-------------OOOOOOOOO *****************">>$log_file
echo  "$now_time create log file :$log_file">>$log_file
echo  "$now_time create log file :$log_file"
s=$(du -k $GMS_LOG | awk '{print $1}')
if [ "$s" -ge 1000 ]; then #1M limit
rm -rf $GMS_LOG && touch $GMS_LOG
chmod 777 $GMS_LOG
fi

monitor() {        
	name=$1                                           
		p=`ps -ef|grep redis-server |grep -v grep|wc -l` #full path
		if [ "$p" == "0" ]; then # 1 is grep self process 
#echo "crond: (`date`) start $name from dead" >> $GMS_LOG
			service $name stop                            
				service $name start                           
				fi                                                
}                                                     
monitor_bad(){                                        
	name=$1                                           
		p=`ps -ef| grep  /db/postgresql/data|grep -v grep|wc -l` #full path
		if [ "$p" == "0" ]; then # 1 is grep self process 
#echo "crond: (`date`) start $name from dead" >> $GMS_LOG
			service $name stop                            
				service $name start                           
				fi                                                
}                                                     
monitor_tomcat(){                                        
	name=$1                                           
		p=`ps -ef| grep  tomcat|grep -v grep|wc -l` #full path
		if [ "$p" == "0" ]; then # 1 is grep self process 
#echo "crond: (`date`) start $name from dead" >> $GMS_LOG
			service $name stop                            
				service $name start                           
				fi                                                
}                                                     

#monitor logmon                                        
monitor redis                    
monitor_bad postgres       
monitor_tomcat tomcat
#if [ $XGE_FLAG != "1" ];then
#	cd /gms/gapi/modules/auth;python sys_auth.pyc 1>/dev/null
#	ret=`echo $?`
#if [ $ret != 16 ];then
#	echo  "$now_time *******************auth fail or no auth***************">>$log_file
#	echo "auth fail"
#	echo  "$now_time *******************auth fail kill dd apc dm***************">>$log_file
#	if [ -n "$VDS_STAT" ];then
#		killall -9 dd
#		killall -9 apc
#		killall -9 dm
#	fi
#	echo  "$now_time *******************auth fail kill url_delect.py***************">>$log_file
#	if [ -n "$URL_STAT" ];then
#		kill -9 $URL_STAT
#	fi
#	echo  "$now_time *******************auth fail kill mtx***************">>$log_file
#	if [ -n "$MTX_STAT" ];then
#		kill -9 $MTX_STAT
#	fi
#	echo  "$now_time *******************auth fail kill apt***************">>$log_file
#	if [ -n "$APT_STAT" ];then
#		kill -9 $APT_STAT
#	fi
#	echo  "$now_time *******************auth fail kill online***************">>$log_file
#	if [ -n "$OFFLINE_STAT" ];then
#		kill -9 $OFFLINE_STAT
#	fi
#	echo  "$now_time *******************auth fail kill comm_main.pyc***************">>$log_file
#	if [ -n "$COMM_STAT" ];then
#		kill -9 $COMM_STAT
#	fi
#	echo  "$now_time *******************auth fail kill comm_main_cloud.pyc***************">>$log_file
#	if [ -n "$CLOUD_STAT" ];then
#		kill -9 $CLOUD_STAT
#	fi
#	echo  "$now_time *******************auth fail kill store_proc***************">>$log_file
#	if [ -n "$STORE_STAT" ];then
#		kill -9 $STORE_STAT
#	fi
#	echo  "$now_time *******************auth fail kill flow_proc***************">>$log_file
#	if [ -n "$FLOW_STAT" ];then
#		kill -9 $FLOW_STAT
#	fi
#	echo  "$now_time *******************auth fail kill merge_proc***************">>$log_file
#	if [ -n "$MERGE_STAT" ];then
#		kill -9 $MERGE_STAT
#	fi
#exit -1
#fi
#fi
#if [ ! -f $AUTH_FLAG ]; then
#	echo  "$now_time Not auth flag:$AUTH_FLAG">>$log_file
#	echo  "$now_time Not auth flag:$AUTH_FLAG"
#	exit -1
#fi
cd /gms/storage
if [ $XGE_FLAG != "1" ];then
	if [ -z "$STORE_STAT" ];then
		echo  "$now_time ***********************store_proc no start ,restart store_proc********************">>$log_file
		echo  "$now_time ***********************store_proc no start ,restart store_proc********************"
		./store_proc 1>/dev/null 2>&1 &
	else
		echo  "$now_time ***********************store_proc have started ,no store_proc********************">>$log_file
		echo  "$now_time ***********************store_proc have started ,no store_proc********************"
	fi
	if [ -z "$MERGE_STAT" ];then
		echo  "$now_time ***********************merge_proc no start ,restart merge_proc********************">>$log_file
		echo  "$now_time ***********************merge_proc no start ,restart merge_proc********************"
		./merge_proc 1>/dev/null 2>&1 &
	else
		echo  "$now_time ***********************merge_proc have started ,no merge_proc********************">>$log_file
		echo  "$now_time ***********************merge_proc have started ,no merge_proc********************"
	fi
fi
if [ -z "$FLOW_STAT" ];then
	echo  "$now_time ***********************flow_proc no start ,restart flow_proc********************">>$log_file
	echo  "$now_time ***********************flow_proc no start ,restart flow_proc********************"
	./flow_proc 1>/dev/null 2>&1 &
else
	echo  "$now_time ***********************flow_proc have started ,no flow_proc********************">>$log_file
	echo  "$now_time ***********************flow_proc have started ,no flow_proc********************"
fi
if [ -z "$FLOW_DEF_STAT" ];then
	echo  "$now_time ***********************flow_def no start ,restart flow_def********************">>$log_file
	echo  "$now_time ***********************flow_def no start ,restart flow_def********************"
	./flow_def 1>/dev/null 2>&1 &
else
	echo  "$now_time ***********************flow_def have started ,no flow_def********************">>$log_file
	echo  "$now_time ***********************flow_def have started ,no flow_def********************"
fi
if [ -z "$GUARD_STAT" ];then
	echo  "$now_time ***********************sys_guard no start ,restart sys_guard********************">>$log_file
	echo  "$now_time ***********************sys_guard no start ,restart sys_guard********************"
	cd /gms/guard/;./sys_guard 1>/dev/null 2>&1 &
else
	echo  "$now_time ***********************sys_guard have started ,no sys_guard********************">>$log_file
	echo  "$now_time ***********************sys_guard have started ,no sys_guard********************"
fi
if [ -z "$COMM_STAT" ];then
	echo  "$now_time ***********************comm_main.py no start ,restart comm_main.py***************">>$log_file
	echo  "$now_time ********************comm_main.py no start ,restart comm_main.py*****************"
	cd /gms/comm;nohup python comm_main.pyc 1>/dev/null 2>&1 &
else
	echo  "$now_time **********************comm_main.py have started ,no comm_main.py*****************">>$log_file
	echo  "$now_time ********************comm_main.py have started ,no comm_main.py*******************"
fi
if [ $root == "1" ];then
	if [ -z "$CLOUD_STAT" ];then
		echo  "$now_time ***********************comm_cloud.py no start ,restart comm_cloud.py***************">>$log_file
		echo  "$now_time ********************comm_cloud.py no start ,restart comm_cloud.py*****************"
		cd /gms/gapi/init/;python cloud_start.pyc >>$log_file
	else
		echo  "$now_time **********************comm_cloud.py have started ,no comm_cloud.py*****************">>$log_file
		echo  "$now_time ********************comm_cloud.py have started ,no comm_cloud.py*******************"
	fi
	if [ $cloudcon == "1" ];then
		if [ -n "$EVENT_STAT" ];then
			kill -9 $EVENT_STAT
			echo  "$now_time **********************because root 1**cloudcn 1*so killall (/nmsmw/python-2.6.8/bin/python /nmsmw/daemon/mtx_export/event_export.py)*************">>$log_file
		fi
	else
		echo  "$now_time **********************because root 1**cloudcn 0** so start (/nmsmw/python-2.6.8/bin/python /nmsmw/daemon/mtx_export/event_export.py)*****************">>$log_file
		if [ -z "$EVENT_STAT" ];then
		nohup python /nmsmw/daemon/mtx_export/event_export.py 1>/dev/null 2>&1 &
		fi

	fi
else
	if [ "$comm_link_stat" == "0" ];then
		if [ -z "$CLOUD_STAT" ];then
			echo  "$now_time **********************comm link fail ,so start cloud_comm_main.py*******************************">>$log_file
			echo  "$now_time **********************comm link fail ,so start cloud_comm_main.py*******************************"
		cd /gms/gapi/init/;python cloud_start.pyc >>$log_file	
		fi	
	else
		if [ -n "$CLOUD_STAT" ];then
			echo  "$now_time **********************because root 0**cloudcn 0** so kill -9 $CLOUD_STAT(cloud_comm_main.py)*****************">>$log_file
			kill -9 $CLOUD_STAT
		fi	
	fi
	if [ -n "$EVENT_STAT" ];then
		echo  "$now_time **********************because root 0**cloudcn 0** so kill -9 $EVENT_STAT(/nmsmw......event_export.py)*****************">>$log_file
		echo  "$now_time **********************because root 0**cloudcn 0** so kill -9 $EVENT_STAT(/nmsmw......event_export.py)*****************"
		kill -9 $EVENT_STAT
	fi	

fi

if [ $monitor == "1" ];then
	if [ -f "$REG_PATH" ];then
		DEV_ID=`cat $REG_PATH |cut -f 8 -d '|'`
		echo  "$now_time ***********************read $REG_PATH get dev_id :$DEV_ID********************">>$log_file
		echo  "$now_time ***********************read $REG_PATH get dev_id :$DEV_ID********************"
		if [ "$stream_num" != "\N" ];then 
			if [ -z "$MTX_STAT" ];then
				if [ $MTX_FLAG == "1" ];then
					cd /gms/mtx
					if [ -f $WTL_PATH ];then
						echo  "$now_time **********************************start mtx wtl************************************** ">>$log_file
						echo  "$now_time **********************************start mtx wtl************************************** "
						nohup ./mtx -d special --devid $DEV_ID -t $stream_num --flow_num $stream_num -w /data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-$DEV_ID.mtd -G 30 --dynpool_size 200M --ypool_size 400M --mtdver 4 --back_trace --emergency -I -o 10240  &
					else
						echo  "$now_time **********************************start mtx no_wtl************************************** ">>$log_file
						echo  "$now_time **********************************start mtx no_wtl************************************** "
						nohup ./mtx -d special --devid $DEV_ID -t $stream_num --flow_num $stream_num -w /data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-$DEV_ID.mtd -G 30 --dynpool_size 200M --ypool_size 400M --mtdver 4 --back_trace --emergency -I -o 10240 &
					fi
					echo "./mtx -d special --devid $DEV_ID -t $stream_num --flow_num $stream_num -w /data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-$DEV_ID.mtd -G 30 --dynpool_size 200M --ypool_size 400M --mtdver 4 --back_trace --emergency -I -o 10240 "
				fi

			else
				echo  "$now_time **********************************mtx have started************************************** ">>$log_file
				echo  "$now_time **********************************mtx have started************************************** "
		fi

		if [ -z "$APT_STAT" ]; then
			if [ $APT_FLAG == "1" ];then
			echo  "$now_time **********************************APT not start,restart APT****************************** ">>$log_file
			echo  "$now_time **********************************APT not start,restart APT****************************** "
			cd /gms/apt/online; ./apt 1>/dev/null 2>&1 &
			fi
		else
			echo  "$now_time **********************************APT have started,not restart APT****************************** ">>$log_file
			echo  "$now_time **********************************APT have started,not restart APT****************************** "
		fi
		if [ -z "$OFFLINE_STAT" ]; then
			if [ $APT_FLAG == "1" ];then
				echo  "$now_time ********************************offline not start,restart offline************************* ">>$log_file
				echo  "$now_time ******************************offline not start,restart offline**************************** "
				cd /gms/apt/offline; ./offline 1>/dev/null 2>&1 &
			fi
		else
			echo  "$now_time **********************************APT have started,not restart APT****************************** ">>$log_file
			echo  "$now_time **********************************APT have started,not restart APT****************************** "
		fi

		if [ -z "$VDS_STAT" ]; then
			if [ $VDS_FLAG == "1" ];then
				echo  "$now_time **********************************Vds not start,restart vds****************************** ">>$log_file
				echo  "$now_time **********************************Vds not start,restart vds****************************** "
				cd /vds/script/; ./monitor.sh >>$log_file 
			fi
		else
			echo  "$now_time **********************************Vds have started,not restart vds****************************** ">>$log_file
			echo  "$now_time **********************************Vds have started,not restart vds****************************** "

		fi

		if [ -z "$URL_STAT" ]; then
			echo  "$now_time **********************************url_detect.py not start,restart url_detect.py************************ ">>$log_file
			echo  "$now_time **********************************url_detect.py not start,restart url_detect.py************************ "
			cd /gms/url_detect/;nohup python url_detect.pyc 1>/dev/null 2>&1 &
		else
			echo  "$now_time **********************************url_detect.py have started,not restart url_detect.py****************** ">>$log_file
			echo  "$now_time **********************************url_detect.py have started,not restart url_detect.py*********************** "
		fi
	fi
else
	echo  "$now_time **********************************Not get $REG_PATH ,not start mtx apt vds**************************** ">>$log_file
	if [ -n "$VDS_STAT" ];then
	killall -9 dd
	killall -9 apc
	killall -9 dm
	fi
	if [ -n "$URL_STAT" ];then
	kill -9 $URL_STAT
	fi
	if [ -n "$MTX_STAT" ];then
	kill -9 $MTX_STAT
	fi
	if [ -n "$APT_STAT" ];then
	kill -9 $APT_STAT
	fi
	if [ -n "$OFFLINE_STAT" ];then
	kill -9 $OFFLINE_STAT
	fi
	echo  "$now_time **********************************Not get $REG_PATH ,not start mtx apt vds**************************** "
	fi
	else
	echo  "$now_time **********************because monitor 0** so kill(mtx vds apt url_detect.py offline)*****************">>$log_file
	if [ -n "$VDS_STAT" ];then
	killall -9 dd
	killall -9 apc
	killall -9 dm
	fi
	if [ -n "$URL_STAT" ];then
	kill -9 $URL_STAT
	fi
	if [ -n "$MTX_STAT" ];then
	kill -9 $MTX_STAT
	fi
	if [ -n "$APT_STAT" ];then
	kill -9 $APT_STAT
	fi
	if [ -n "$OFFLINE_STAT" ];then
	kill -9 $OFFLINE_STAT
	fi
	echo  "$now_time **********************OOOOOOOO-------------------|  END  |-------------OOOOOOOOO *****************">>$log_file
fi
