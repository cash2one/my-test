#!/bin/sh
dev_conf=/cfcard/chanct_conf/dev_list.conf
if [ -f $dev_conf ];then
	vi $dev_conf
	touch /cfcard/.conf.dev
	echo "device is rebooting for config finish "
	sync
	reboot
fi
