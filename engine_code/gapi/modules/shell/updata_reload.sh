#!/bin/sh
####set standard_conforming_strings=on;
csv_path="/tmp/rule_data.csv"
mtx_path=/gms/mtx/conf/rule.bak
mtx_dst_path=/gms/mtx/conf/rule.conf
rule_table=t_rule_data
store_flag="\`"
maxnum=0
#crond_args
cron_path=/var/spool/cron/crontabs/root
update_rule_shell=/gms/bin/while_hour_rule.sh
rule_time=/gms/conf/rule_tmp_time.txt

postgres_alter_handle()
{
/db/postgresql/bin/psql -U postgres -d  gms_db <<E2
copy  $rule_table (rid,rname,protocol,rmatch,raction,reverse,response,reportloc,choose) from '$csv_path' with delimiter '$store_flag' ;
update t_rule_data set response = replace(response,'null','');
delete from  $rule_table where id <=$maxnum;
vacuum full t_rule_data;
E2
}
postgres_find_data()
{
/db/postgresql/bin/psql -U postgres -d  gms_db <<E2
select count(id) from t_rule_data;
E2
}
postgres_find_max_id()
{
/db/postgresql/bin/psql -U postgres -d  gms_db <<E2
select max(id) from t_rule_data;
E2
}
save_file()
{
crontab -e <<EOF
wq

EOF
}
crond_update_rule()
{
	hour_id=`echo -n $1|cut -f 1 -d :`
	min_id=`echo -n $1|cut -f 2 -d :`
	task_stat=`cat $cron_path |grep rule`
	if [ -n "$task_stat" ];then
		sed -i '/rule/d' $cron_path
	fi
	sed -i  "1a $min_id $hour_id */1 * * /bin/sh $update_rule_shell"  $cron_path
	save_file 1>/dev/null  2>/dev/null
}

if [ "$1" == "0" ];then
	sed -i 's///g' $mtx_path 
	python /gms/bin/form.py > $csv_path
	sed -i 's/``/`/g' $csv_path 
	sed -i 's/\\/\\\\/g' $csv_path
	if [ "`cat $csv_path`" == "" ];then
		exit 1
	fi
	buf=`postgres_find_data`
	#echo $buf
	num=`echo $buf|cut -f 3 -d " "`
	#echo $num
	if [ $num != 0 ];then
	buf=`postgres_find_max_id`
	maxnum=`echo $buf|cut -f 3 -d " "`
	fi
	buf=`postgres_alter_handle 2>&1`
	echo $buf
	buf=${buf:0:5}
	if [ "$buf" == "ERROR" ];then
		exit 1
	fi
	cp -rf $mtx_path $mtx_dst_path
	killall -9 mtx
#rm -rf $mtx_path
	rm -rf $csv_path
	#python /gms/bin/parse_rule.py 1>/dev/null 2>&1 
elif [ "$1" == "1" ];then
	if [ -z "$2" ];then
		echo "please input eg:$0: 1 00:00"
		exit 1
	fi
	echo -n "$2" >$rule_time
	crond_update_rule $2
else
	if [ ! -f "$rule_time" ];then
		echo -n "00:00" >$rule_time 
	fi	
		time_val=`cat $rule_time`
	crond_update_rule "$time_val"
fi
exit 0
