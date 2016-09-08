#!/bin/sh

if [ -z $1 ];then
	exit -1
fi
key=`echo -n "$1"|md5sum|cut -f 1 -d ' '`
uname=admin
#echo -n $key
change_key(){
su - postgres <<EOF
/db/postgresql/bin/psql gms_db <<E2
update t_conf_user set passwd ='$key' where uname='$uname';
E2
EOF
}
result=`change_key $1`
result=`echo $result|cut -f 1 -d ' '`
if [ "$result" == "UPDATE" ];then
	exit 0
else
	exit 1
fi
