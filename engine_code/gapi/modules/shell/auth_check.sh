#!/bin/sh
gmsconf_path=/gms/conf/gmsconfig.xml
gms_cloud=/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json
CLOUD_STAT=`ps x|grep -v 'grep'|grep "cloud_comm_main.py"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r`
XGE_FLAG=`cat $gmsconf_path |awk -F [=\"] '/if_is_monitor ret/{print $3}'|awk 'NF>0'`

if [ $XGE_FLAG != "1" ];then
	cd /gms/gapi/modules/auth;python sys_auth.pyc 1>/dev/null
	ret=`echo $?`
if [ $ret != 16 ];then
	sed -i '/if_rule_flag/d' $gms_cloud
	sed -i '/enable_white_list/a \    \"if_rule_flag\":         \"no\"' $gms_cloud
	if [ -n "$CLOUD_STAT" ];then
		kill -9 $CLOUD_STAT
	fi
else
	sed -i '/if_rule_flag/d' $gms_cloud
	sed -i '/enable_white_list/a \    \"if_rule_flag\":         \"yes\"' $gms_cloud
	if [ -n "$CLOUD_STAT" ];then
		kill -9 $CLOUD_STAT
	fi
fi
fi
