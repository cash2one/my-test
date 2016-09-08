#!/bin/bash -f

function GetMemUsedPercent()
{
	free | grep "Mem" | awk '{
			printf("mem:%d\n", $3/$2*100);
		}'
}


function GetDiskUsedPercent()
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

function GetCpuIdle()
{
	top -b -n 1 | grep -w Cpu | awk '{print"cpu:"100 - $5}' | awk -F '%' '{ print $1}'
}


function GetSystemRunTime()
{
	
	cat /proc/uptime | awk '{printf("runtime:%d\n", $1/60)}'

}

function Getdatapercent()
{
	df -h | grep /data | awk -F '[ %]+' '{print "data:" $5}'

}

Getdatapercent
GetDiskUsedPercent
GetCpuIdle
GetMemUsedPercent
GetSystemRunTime
