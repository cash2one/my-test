#!/bin/bash 

#GetMemUsedPercent()
#{
#	free | grep "Mem" | awk '{
#			printf("mem:%d\n", ($3-$6-$7)/$2*100);
#		}'
#}
GetMemUsedPercent()
{
    cat /proc/meminfo | awk 'NR <= 4{print $2}'| awk ' 
        BEGIN{
            FS = "\n"
            RS = ""
        }
        { 
            printf("mem:%d\n", ($1-$2-$3-$4)/$1*100);
        }'
}

GetDiskUsedPercent()
{
	df | awk '
	BEGIN {
		available = 0;
		tot = 0;
	}

	{
		if (NR==1) {
			next
		}
		if (NF == 6) {
			available += $4;
			tot += $2;
		} else if (NF == 5){
			available += $3;
			tot += $1;
		} else {
			# skip
		}
	}

	END {
		printf("disk:%d\n", 100 - available/tot*100);
	}'
}

#GetCpuPercent()
#{
#	top -d 1 -n 3 | grep -i 'c''p''u' | awk 'NR == 3' | awk -F '[ %]+' '
#            {printf("cpu:%d\n", ($2+$4+$6)/($2+$4+$6+$8) *100)}' 
#}
GetCpuPercent()
{
    Log1=$(cat /proc/stat | grep 'cpu ' | awk '{print $2" "$3" "$4" "$5" "$6" "$7" "$8}')
    idle1=$(echo $Log1 | awk '{print $4}')
    Total1=$(echo $Log1 | awk '{print $1+$2+$3+$4+$5+$6+$7}')

    sleep 0.5

    Log2=$(cat /proc/stat | grep 'cpu ' | awk '{print $2" "$3" "$4" "$5" "$6" "$7" "$8}')
    idle2=$(echo $Log2 | awk '{print $4}')
    Total2=$(echo $Log2 | awk '{print $1+$2+$3+$4+$5+$6+$7}')

    Total=`expr $Total2 - $Total1`
    idle=`expr $idle2 - $idle1`
    sub=`expr $Total - $idle`
    n_sub=`expr $sub \* 100`
    cpu_percent=`expr $n_sub / $Total`
    echo "cpu:"$cpu_percent
}

GetSystemRunTime()
{
	
	cat /proc/uptime | awk '{printf("runtime:%d\n", $1/60)}'

}

Getdatapercent()
{     
	df -h | grep /data | awk -F '[ %]+' '{print "data:" $5}' 
}

Getdbpercent()
{     
	df -h | grep /db | awk -F '[ %]+' '{print "db:" $5}' 
}
GetDiskUsedPercent
GetCpuPercent 
GetMemUsedPercent
GetSystemRunTime
Getdatapercent
Getdbpercent
