#!/bin/sh
num=3
update_rule_python=/gms/gapi/modules/proxy/cloud/online_down_rule.pyc
rule_update_stat=`ps -ef|grep online_down_rule|grep -v grep`
for i in `seq $num`
do

if [ -z "$rule_update_stat" ];then
	nohup python $update_rule_python 1>/dev/null 2>&1 &	
fi

sleep 3600s
done
