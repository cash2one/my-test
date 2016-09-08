#!/bin/sh
storage_path=/gms/storage/conf/storage.conf
web_xml=/gms/conf/gmsdeploy.xml
kill_tomcat_cmd=/gms/tomcat/bin/shutdown.sh
if [ "$1" = "start" ];then
	sed -i '/F04/d' $web_xml
	sed -i "/F03/a \    <function id=\"F04\" deploy=\"1\" desc=\"上网行为审计\"\/>" $web_xml
	sed -i '/flow_audit_switch/d' $storage_path
	sed -i '/#上网行为/a flow_audit_switch = yes' $storage_path
	$kill_tomcat_cmd 1>/dev/null 2>/dev/null
	/gms/bin/flow_config
	echo "start finish web restart"

elif [ "$1" = "stop" ];then

	sed -i '/F04/d' $web_xml
	sed -i "/F03/a \    <function id=\"F04\" deploy=\"0\" desc=\"上网行为审计\"\/>" $web_xml
	sed -i '/flow_audit_switch/d' $storage_path
	sed -i '/#上网行为/a flow_audit_switch = no' $storage_path
	$kill_tomcat_cmd 1>/dev/null 2>/dev/null
	/gms/bin/flow_config
	echo "stop finish web restart"
else
	echo "input:|start|stop"
fi
