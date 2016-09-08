#!/usr/bin/python
# _*_coding:utf8 _*_
import json
import signal
import os
import sys
import time
import xml.dom.minidom
import threading
import thread
import print_color
import cloudid_topo


base_cloud_id = 20000043
reg_src_path = "/data/gms_host/reg/up/"
reg_dst_path = "/data/gms_host/reg/down/"

#------------------------------.
# @brief 主程序捕获函数		   |
#------------------------------*
def sig_exit(sig, stack):
    print("recv signal[%d]. exit." %sig)
    exit()

 #-----------------------------------------.
# @brief 遍历制定目录，得到指定后缀的文件 |
# @param path		文件目录			  |
# @param suffix		后缀				  |
#-----------------------------------------*
def file_filter_bysuffix(input, suffix=None):
    def ffilter(list, dirname, fnames):
        if suffix != None:
            fnames = filter(lambda f: os.path.splitext(f)[1] in suffix, fnames)

        list.extend(filter(os.path.isfile, map(lambda f: os.path.join(dirname, f), fnames)))

    def fprocess(path):
        path = os.path.expanduser(path)
        if os.path.isfile(path):
            flist.append(path)
        elif os.path.isdir(path):
            os.path.walk(path, ffilter, flist)

    flist = []
    map(fprocess, input)
    return flist   


def do_cloud_reg(reg_file_name):
    
    src_file = reg_src_path + reg_file_name 
    dst_file = reg_dst_path + reg_file_name

    readed = json.load(open(src_file, 'r'))

    i = 0

    global base_cloud_id

    for readed[i] in readed:
        base_cloud_id += 1
        if readed[i]["ip"] == "192.168.1.158":
            readed[i]["id"] = "20000158"
        elif readed[i]["ip"] == "192.168.1.165":
            readed[i]["id"] = "20000165"
        elif readed[i]["ip"] == "192.168.1.160":
            readed[i]["id"] = "20000160"
        elif readed[i]["ip"] == "192.168.0.180":
            readed[i]["id"] = "20000180"
        elif readed[i]["ip"] == "192.168.0.181":
            readed[i]["id"] = "20000181"
        elif readed[i]["ip"] == "192.168.0.182":
            readed[i]["id"] = "20000182"
        elif readed[i]["ip"] == "192.168.0.183":
            readed[i]["id"] = "20000183"
        elif readed[i]["ip"] == "192.168.0.184":
            readed[i]["id"] = "20000184"
        elif readed[i]["ip"] == "192.168.0.185":
            readed[i]["id"] = "20000185"
        elif readed[i]["ip"] == "192.168.0.186":
            readed[i]["id"] = "20000186"
        else:
            readed[i]["id"] = ""
        
        print readed[i]["id"]

    json.dump(readed, open(dst_file, 'w'))


if __name__=="__main__":
    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)

    while True:
        for reg_file in file_filter_bysuffix([reg_src_path], ['.ok']):
            reg_file_name = os.path.basename(reg_file)
            do_cloud_reg(reg_file_name)
            os.remove(reg_file)
        time.sleep(1)
