#!/bin/sh
datase_path=/gms/conf/database.conf
storage_path=/gms/storage/conf/storage.conf
web_xml=/gms/conf/gmsdeploy.xml
kill_tomcat_cmd=/gms/tomcat/bin/shutdown.sh
comm_conf=/gms/comm/conf/comm_pre_conf.json
if [ "$1" = "start" ];then
#sed -i 's/0xDCBC/0xDCBB/' $datase_path
	sed -i '/F01/d' $web_xml
	sed -i "/<gmsdeploy>/a \    <function id=\"F01\" deploy=\"1\" desc=\"流量分析\"\/>" $web_xml
	sed -i '/old_flow/d' $comm_conf
	sed -i "/iscopytodb/a \    \"old_flow\":             \"1\"," $comm_conf
	sed -i '/flow_analyze/d' $storage_path 
	sed -i '/flowevent_store_disenable/a flow_analyze        = yes' $storage_path
	$kill_tomcat_cmd 1>/dev/null 2>/dev/null
	killall -9 flow_proc
	echo "start finish web restart"

elif [ "$1" = "stop" ];then

#sed -i 's/0xDCBB/0xDCBC/' $datase_path
	sed -i '/F01/d' $web_xml
	sed -i "/<gmsdeploy>/a \    <function id=\"F01\" deploy=\"0\" desc=\"流量分析\"\/>" $web_xml
	sed -i '/old_flow/d' $comm_conf
	sed -i "/iscopytodb/a \    \"old_flow\":             \"0\"," $comm_conf
	sed -i '/flow_analyze/d' $storage_path 
	sed -i '/flowevent_store_disenable/a flow_analyze        = no' $storage_path
	$kill_tomcat_cmd 1>/dev/null 2>/dev/null
	killall -9 flow_proc
	echo "stop finish web restart"
else
	echo "input:|start|stop"
fi
