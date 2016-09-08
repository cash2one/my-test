#!/bin/sh
SW_ADDR=192.0.10.250
conf_device()
{
if [ "192.0.10.252" = "$1" ];then
	eth_num=b/3-4
fi
if [ "192.0.10.253" = "$1" ];then
	eth_num=b/5-6
fi
if [ "192.0.10.254" = "$1" ];then
	eth_num=b/7-8
fi
if [ "1" = "$2" ];then
	cmd_str="trunk 2 add interface $eth_num"
else
	cmd_str="trunk 2 no add interface $eth_num"
fi
cd /gms/gapi/modules/device/conf
./ssh_logon.sh $SW_ADDR "$cmd_str"
}
if [ "$1" != "n" ];then
        echo "*******************"
	num=`echo -n $1 | awk -F':' '{print NF}'`
	for i in $(seq $num);do
	ipval=`echo -n $1 | cut -f $i -d ":"`
	conf_device $ipval 1 < /dev/null 
	done
fi
if [ "$2" != "n" ];then
	echo "***********************************************"
	num=`echo -n $2 | awk -F':' '{print NF}'`
	for i in $(seq $num);do
	ipval=`echo -n $2 | cut -f $i -d ":"`
	conf_device $ipval 0 < /dev/null
	done
fi
exit 0
