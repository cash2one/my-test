#!/bin/bash

get_proc_info(){
	pidnum=`ps -ef|grep "$1"|wc -l`
#	echo $pidnum
	if [ "$pidnum" -ge "1" ]; then

	pid=`ps axu|grep "$1"|grep -v grep|awk '{print $1}'`
#	echo $pid
	tmp1=`cat /proc/$pid/stat | awk '{print $24}'`
	tmp2=`expr $tmp1 \* 4`
	mem=`expr $tmp2 / 1024`

#	cpu=`top -d 1 -n1 -b| grep $pid | awk '{print $8}'`
	line=`top -d 1 -n1 -b|awk '{print $1}'|grep -n $pid |awk -F":" '{print $1}'`
	if [ "$line" -gt "0" ]; then
	cpu=`top -d 1 -n1 -b|sed -n "$line p"|awk '{print $8}'`
	else
	cpu=0
	fi
	echo $1 "                       "$mem "                    " $cpu
	else
	echo $1 "                       ""0"  "                    " "0" 
	fi
}
while true
do
echo `date`   >> /gms/guard/conf/tmp    
#get_proc_info tomcat  >> /gms/guard/conf/tmp 
#get_proc_info mtx    >> /gms/guard/conf/tmp
#get_proc_info "tcad_gms"   >> /gms/guard/conf/tmp  
#get_proc_info /vds/dd  >> /gms/guard/conf/tmp    
#get_proc_info /vds/dm  >> /gms/guard/conf/tmp   
get_proc_info /vds/apc >> /gms/guard/conf/tmp    
get_proc_info "python comm_main.pyc"  >> /gms/guard/conf/tmp     
get_proc_info cloud_comm_main.pyc     >> /gms/guard/conf/tmp
get_proc_info /gms/tcad/tcad_gms   >> /gms/guard/conf/tmp   
get_proc_info "url_detect.pyc"  >> /gms/guard/conf/tmp   
get_proc_info store_proc   >> /gms/guard/conf/tmp   
get_proc_info merge_proc   >> /gms/guard/conf/tmp   
get_proc_info flow_proc   >> /gms/guard/conf/tmp   
get_proc_info sys_guard   >> /gms/guard/conf/tmp   

sleep 30
done
