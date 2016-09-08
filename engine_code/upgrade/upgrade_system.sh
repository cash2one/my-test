#!/bin/sh

###################################################
########when fail:
########a.only rm /tmp/*
########b.not care $SYSTEM
###################################################

 export TMP_UPNAME=`ls -lt /cfcard/*.up | head -n 1 | awk '{print $9}'`
 export CFCARD=/cfcard
 export BOOTLOADER_PATH=$CFCARD/boot/bootloader
 export SYSTEM=$CFCARD/system
 export GMS_PACKAGE=$CFCARD/gms_package
 export CONF=$CFCARD/chanct_conf
 export CHANCT_LIB=$CFCARD/chanct_lib
 export DB=/db
 export DATA=/data
 
 #gms package
#export LOCAL_PACK_PATH=$CFCARD/gms.tar.gz
 export LOCAL_PACK_PATH=$TMP_UPNAME
 export CHANCT_TOOL=/$CFCARD/boot/chanct_lib_tool.tar.gz
 
 #file related 
 export BOOT_LOG=$CFCARD/boot.log
 export MACHINE_TYPE_FILE=$CONF/machine_type.conf
 export MACHINE_TYPE_9U_FILE=$CONF/machine_type_9u.conf
 export DEVICE_FILE=$CONF/dev.cfg
 export UUID_IP_FILE=$CONF/uuid_ip.cfg
 export VERSION_FILE=$BOOTLOADER_PATH/version
 export ENGINE=/gms 
 export NEW_VERSION=1.0.0.2
 export OLD_VERSION=1.0.0.1
 export disknum=1000000
 export memnum=900000
unzip_ready(){
MD5_STR=`echo $TMP_UPNAME|cut -f 3 -d -|cut -c1-10`
NEW_VERSION=`echo $TMP_UPNAME|cut -f 2 -d -`
OLD_VERSION=`cat /gms/conf/version.xml |grep SysVersion|awk -F'<' '{print $2}'|awk -F'>' '{print $2}'`
NOW_MD5=`md5sum $TMP_UPNAME|awk '{print $1}'|cut -c23-32`
echo $MD5_STR>>/cfcard/upgrade.log
echo $NOW_MD5>>/cfcard/upgrade.log
echo -n `date +"%Y-%m-%d %H:%M:%S"` >>/cfcard/upgrade.log
echo "upgrade..">>/cfcard/upgrade.log
if [ "$MD5_STR" != "$NOW_MD5" ];then
	echo -n "0" >$CFCARD/md5.txt
	rm $TMP_UPNAME
	exit 0
else
###md5验证成功后，验证内存和disk#########
	Gmsdisk=`df |grep gms|awk '{print $4}'`
	Gmsmem=`free |grep Mem |awk '{print $4}'`
	if [ $Gmsdisk -ge $disknum ]; then
		echo "disk sucess.." >>/cfcard/upgrade.log
		if [ $Gmsmem -ge $memnum ]; then
			echo "mem sucess..">>/cfcard/upgrade.log
			echo -n "1">$CFCARD/md5.txt	
		else
			echo -n "1" >$CFCARD/md5.txt
			sync && echo 3 > /proc/sys/vm/drop_caches
		fi
	else
		rm $TMP_UPNAME
		echo -n "0" >$CFCARD/md5.txt
		exit 0
	fi
fi
}

upgrade_start(){
killall -9 crond
killall -9 logmon
killall -9 postgres
killall -9 mtx 
killall -9 apt
killall -9 python
killall -9 store_proc
killall -9 merge_proc
killall -9 flow_proc
cd /gms/tcad/;./dp_killall.sh;cd -
mkdir -p $BOOTLOADER_PATH
GEN_SYSTEM_FILE=generate_system.sh
#PRE_INSTALL_FILE=pre_install.sh
FINAL_INSTALL=final_install.sh
#tar xf $LOCAL_PACK_PATH $GEN_SYSTEM_FILE # only in current directory
#tar xf $LOCAL_PACK_PATH $FINAL_INSTALL # only in current directory
#tar xf $LOCAL_PACK_PATH $PRE_INSTALL_FILE # only in current directory
dd if=$TMP_UPNAME |openssl aes-256-cbc -d -k chanct.gms |tar zxf - $GEN_SYSTEM_FILE 
#dd if=$TMP_UPNAME |openssl aes-256-cbc -d -k chanct.gms |tar zxf - $PRE_INSTALL_FILE 
dd if=$TMP_UPNAME |openssl aes-256-cbc -d -k chanct.gms |tar zxf - $FINAL_INSTALL 
chmod a+x $GEN_SYSTEM_FILE
#chmod a+x $PRE_INSTALL_FILE
chmod a+x $FINAL_INSTALL
mv $GEN_SYSTEM_FILE $BOOTLOADER_PATH/$GEN_SYSTEM_FILE
#mv $PRE_INSTALL_FILE $BOOTLOADER_PATH/$PRE_INSTALL_FILE
mv $FINAL_INSTALL $BOOTLOADER_PATH/$FINAL_INSTALL
$BOOTLOADER_PATH/generate_system.sh 1 >>/cfcard/upgrade.log
$BOOTLOADER_PATH/final_install.sh     >>/cfcard/upgrade.log
}



unzip_ready
if [ "$1" == "1" ]; then
	echo "upgrade start ..." >>/cfcard/upgrade.log
	nowtime=`date +"%Y-%m-%d %H:%M:%S"`
	utype=2
	ucontent_type=1
	new_verinfo=\'系统升级\'
	ustatus=3
	if [ "$2" == "" ];then
		echo -n "'$nowtime',$utype,$ucontent_type,'$OLD_VERSION','$NEW_VERSION',$new_verinfo,$ustatus" >/cfcard/update_log.txt
	else
		echo -n "'$nowtime',$utype,$ucontent_type,'$OLD_VERSION','$NEW_VERSION',$new_verinfo,$ustatus,$2,$3" >/cfcard/update_log.txt
	fi
	upgrade_start
	echo "upgrade sucess reboot..." 
	echo "sync" >>/cfcard/upgrade.log 
	sync
	echo "sync" >>/cfcard/upgrade.log 
	sync
	echo "sync" >>/cfcard/upgrade.log 
	sync
	echo "reboot..." >>/cfcard/upgrade.log
	reboot
fi
