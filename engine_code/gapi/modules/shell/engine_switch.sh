#!/bin/sh
GMSXML=/gms/conf/gmsconfig.xml
APT_TMP_PATH=/gms/apt/online/aptbak
APT_PATH=/gms/apt/online/apt

apt_fork=`ps -ef|grep apt |grep -v grep`
tcad_fork=`ps -ef|grep tcad_gms |grep -v grep`
XGE_MONITOR=`cat $GMSXML |awk -F [=\"] '/if_is_monitor ret/{print $3}'|awk 'NF>0'`
XGE_DEVICE=`cat $GMSXML  |awk -F ["<>"] '/if_is_xge_device/{print $3}'|awk 'NF>0'`
flow_engine=`cat $GMSXML |awk -F ["<>"] '/flow_engine_name/{print $3}'|awk 'NF>0'`
SSH_LINE_CMD=/gms/bin/ssh_set_monitor.sh
cmd_name=/gms/bin/engine_switch.sh
engine_conf_file=/gms/gapi/modules/engine/conf_path.json
ethinfo_file=/gms/gapi/modules/ethinfo/conf_path.json
if [ "$1" = "apt" ];then
	if [ "apt" = $flow_engine  -a  "$apt_fork" != "" ];then
		echo "apt useing ..."
		exit 1
	fi
	cd /gms/tcad
	./dp_killall.sh 1>/dev/null 2>&1
	echo "killall tcad"
	cp /gms/storage/flow_proc_apt /gms/storage/flow_proc
	if [ "apt" != $flow_engine ];then
		cp -f pag.conf /gms/apt/online/
	fi
		mv $APT_TMP_PATH $APT_PATH
#mv /gms/apt/offline/offline /gms/apt/offline/offlinebak
		killall -9 flow_proc
		sed -i "s/\/gms\/tcad\/pag.conf/\/gms\/apt\/online\/pag.conf/g" $ethinfo_file
#sed -i "s/\/gms\/aptbak\//\/gms\/apt\//g" $engine_conf_file
		sed -i "s/\/gms\/tcad\/pag.conf/\/gms\/apt\/online\/pag.conf/g" $engine_conf_file
		sed -i "s/<flow_engine_name>tcad<\/flow_engine_name>/<flow_engine_name>apt<\/flow_engine_name>/g" $GMSXML
	
fi
if [ "$1" = "tcad" ];then
	if [ "tcad" = $flow_engine  -a  "$tcad_fork" != "" ];then
		echo "tcad useing ..."
		exit 1
	fi
	if [ -f $APT_PATH ];then 
		mv $APT_PATH $APT_TMP_PATH
	fi
	if [ "tcad" != $flow_engine ];then
		cp -f /gms/apt/online/pag.conf /gms/tcad/
	fi
#mv /gms/apt/offline/offlinebak /gms/apt/offline/offline
		#修改引擎和捕包的gapi的配置文件，让修改的文件变为tcad下的配置文件
		sed -i "s/\/gms\/apt\/online\/pag.conf/\/gms\/tcad\/pag.conf/g" $engine_conf_file
#sed -i "s/\/gms\/apt\//\/gms\/aptbak\//g" $engine_conf_file
		sed -i "s/\/gms\/apt\/online\/pag.conf/\/gms\/tcad\/pag.conf/g" $ethinfo_file
		killall -9 apt
#mv /gms/storage/flow_proc /gms/storage/flow_proc_apt
		cp /gms/storage/flow_proc_tcad /gms/storage/flow_proc
		killall -9 flow_proc
		cd /gms/tcad
		./dp_sudo_run.sh
		sed -i "s/<flow_engine_name>apt<\/flow_engine_name>/<flow_engine_name>tcad<\/flow_engine_name>/g" $GMSXML


fi
if [ "$XGE_DEVICE" = "1" -a "$XGE_MONITOR" = "0" ];then
	chmod +x $SSH_LINE_CMD
	cat $GMSXML |awk -F ["<>"] '/<device/'|sed 's/.*\(ip\)/\1/'|while read line;
	do
		ipval=`echo $line|awk -F [=' '\"] '{print $3}'`
		$SSH_LINE_CMD  $ipval "$cmd_name $1" < /dev/null
		#$SSH_LINE_CMD  $ipval "touch a" < /dev/null 
	done
fi
