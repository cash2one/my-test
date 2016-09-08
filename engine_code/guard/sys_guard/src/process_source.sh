#!/bin/bash -f

function get_mtx_source()
{
	top -n 1 | grep "mtx" | awk '{
			if(NF == 14) {
				printf("mtx_cpu:%.2f;mtx_mem:%.2f;\n",$10,$11);
			} else if(NF == 13) {
				printf("mtx_cpu:%.2f;mtx_mem:%.2f;\n",$9,$10);
			} else {
				#skip
			}
	}'
}


function get_vds_source()
{
	top -n 1 | grep "vds" | awk '{
			if(NF == 14) {
				printf("vds_cpu:%.2f;vds_mem:%.2f\n",$10,$11);
			} else if(NF == 13) {
				printf("vds_cpu:%.2f;vds_mem:%.2f\n",$9,$10);
			} else {
				#skip
			}
	}'
}


function get_apt_source()
{
	top -n 1 | grep "apt" | awk '{
			if(NF == 14) {
				printf("apt_cpu:%.2f;apt_mem:%.2f\n",$10,$11);
			} else if(NF == 13) {
				printf("apt_cpu:%.2f;apt_mem:%.2f\n",$9,$10);
			}
	}'
}









get_mtx_source
get_vds_source
get_apt_source
