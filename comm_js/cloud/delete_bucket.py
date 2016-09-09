#!/usr/bin/python
# _*_ coding:utf8 _*_

#----------------------------------------------.
# @file cloud_down.py						   |
# @brief 云存储下载流程						   |
# @author liu_danbing <liudanbing@chanct.com>  |
# @version 1.0								   |
# @date 2013-12-26							   |
#----------------------------------------------*

import signal
import sys
import os
import time
import xml.dom.minidom
import threading
sys.path.append("../")
from oss.oss_api import *

OSS_ID   = "iUh5r3SH2auxvjtl"
OSS_KEY  = "YDdmYYn3Z5HXYYN603XnUgPhguKHje"

#---------------------------------.
# @brief 顶级结点 下行云端数据集合|
#---------------------------------*
dev_down_bucket = "oss-event-up"

#---------------------------------.
# @brief 国家主机 下行云端数据集合|
#---------------------------------*
host_down_bucket = "oss-event-down"

#------------------------------.
# @brief 主程序捕获函数		   |
#------------------------------*
def sig_exit(sig, stack):
    print("recv signal[%d]. exit." %sig)
    exit()

#------------------------------.
# @brief 主程序捕获函数		   |
# @param buf        源数据     |
# @param file_path  保存的文件 |
#------------------------------*
def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        print e
    else:
        tmp_file.write(buf)
        tmp_file.close()

#------------------------------.
# @brief 主程序捕获函数		   |
# @param xml_path xml文件路径  |
# @return list                 |
#------------------------------*
def get_cloud_xml_fname(xml_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        list.append(keynode.childNodes[0].nodeValue)
    return list


def delete_bucket_file(oss, bucket):

    # 1. 读取云库中目前都有哪些文件
    #res = oss.list_all_my_buckets()
    try:
        res = oss.get_bucket(bucket)
    except Exception, ex:
        print "@@@@@@2 get Exceptin @@@@@@@@2"
        print ex
        return (-1)
    else:
        if res.status != 200:
            print "list %s bucket FAIL." %(bucket)
            print res.status, res.read()
            return (-1)

      
    # 2. 将得到的xml文件存在本地
    if (dev_type == 1):
        tmp_file = '/tmp/cloud_tmp_dev.xml'
    elif (dev_type == 2):
        tmp_file = '/tmp/cloud_tmp_host.xml'

    write_buf_to_file(res.read(), tmp_file)

    # 3. 解析xml文件 并将得到的文件名下载
    cloud_fname_list = get_cloud_xml_fname(tmp_file)

    # 4. 遍历文件list 分别进行下载
    for fname in cloud_fname_list: 
        t = time.time()
        res = oss.delete_object(bucket, fname)
        if (res.status != 204):
            print "delete cloud[%s] FAIL. %d %s" %(fname, res.status, res.read())
        else:
            print "delete cloud[%s] SUCC." %(fname)

    # 6. 删除xml临时文件
    os.remove(tmp_file)
        

#----------------------.
# @brief main主入口	   |
#----------------------*
if __name__ == "__main__":


	#注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)

	#与阿里云 oss 服务建立连接 
    oss = OssAPI("oss.aliyuncs.com", OSS_ID, OSS_KEY)

    dev_type = int(sys.argv[1])
    if dev_type == 1:
        #up bucket
        bucket = dev_down_bucket

    elif dev_type == 2:
        #down bucket
        bucket = host_down_bucket
    else:
        print 'wrong type 1-delete up bucket  2-delete down bucket'
        exit()

	#进入上传循环业务
    while True:	
        
        delete_bucket_file(oss, bucket)
        
        time.sleep(2)
    else:
        print("EXIT!");
