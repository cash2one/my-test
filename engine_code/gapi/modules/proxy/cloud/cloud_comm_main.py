#!/usr/bin/env python
#-*- coding:utf-8 -*-

from xml.etree import ElementTree
import os
import sys
import time
import threading
import shutil
import signal
import xml.dom.minidom
import print_color
import cloud_log
import struct 
import socket
import traceback
import getopt
import parse
import subprocess

#sys.path.append("../")
import file_up_down_tran
import comm_data
import comm_common
import comm_init
import xmltodict
import cloudid_topo
import json
from comm_backup_DNS import *
from oss.oss_api import *
from oss.osscloudapi import *

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
gmsconfig = '/gms/conf/gmsconfig.xml'
uuid = uplimit = downlimit = g_compare_time0 = commpare_time=uuid_bucket_tag = ''
cloudcon = isroot = ismonitor = ismanager = "0"
comm_cloud_logger=None
retn = -2 
totalmtx = totalvds = refresh_sec = 0
commpare_time = uplimit = downlimit = ''
REFRESH_SEC = 120
thread_list = list()

def create_dir():
    YEAR = time.strftime('%Y',time.localtime(time.time()))
    report_dst_year_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'year'
    report_dst_month_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'month'
    report_dst_week_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'week'
    report_dst_day_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'day'
    
    comm_common.creat_if_dir_notexist(report_dst_year_dir)
    comm_common.creat_if_dir_notexist(report_dst_month_dir)
    comm_common.creat_if_dir_notexist(report_dst_week_dir)
    comm_common.creat_if_dir_notexist(report_dst_day_dir)
    comm_common.creat_if_dir_notexist(comm_data.comm_ue_down_path)
    comm_common.creat_if_dir_notexist(comm_data.host_update_src_path)
    comm_common.creat_if_dir_notexist(comm_data.whitelist_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.whitelist_src_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_re_down_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_conf_down_path)
    comm_common.creat_if_dir_notexist(comm_data.host_conf_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_reg_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_reg_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_flow_down_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_flow_ali_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_dns_down_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_dns_ali_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_url_down_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_query_url_ali_path)
    comm_common.creat_if_dir_notexist(comm_data.comm_topo_up_src_path)
    

#------------------------------.
# @brief 主程序捕获函数        |
#------------------------------*
def sig_exit(sig, stack):
    global comm_cloud_logger
    print("recv signal[%d]. exit. record mtx count[%s] vds count[%s]" %(sig,totalmtx,totalvds))
    comm_cloud_logger.debug("recv signal[%d]. exit. record mtx count[%s] vds count[%s]" %(sig,totalmtx,totalvds))

    write_buf_to_file(totalmtx,'./conf/mtxcount.txt')
    write_buf_to_file(totalvds,'./conf/vdscount.txt')
    
    sys.exit()

#------------------------------.
# @brief 主程序捕获函数        |
# @param buf        源数据     |
# @param file_path  保存的文件 |
#------------------------------*
def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        print e
        comm_cloud_logger.debug("[%s]" %e)
        tmp_file.close()
    else:
        if type(buf) == int:
            tmp_file.write(str(buf))
        else:
            tmp_file.write(buf)
        tmp_file.close()

#--------------------------------.
# @brief 根据云下载的xml文件     |
#        得到 对应事件的文件列表 |
# @param xml_path    xml文件路径 |
# @param prefix_path 事件目录缀  |
# @return list                   |
#--------------------------------*
def get_cloud_xml_fname(xml_path, prefix_path):

    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        if (dir_names[1] == prefix_path):
            list.append(keynode.childNodes[0].nodeValue)
    return list
#--------------------------------.
# @brief 限速模式云下载的xml文件 |
#        得到对应事件的文件列表  |
# @param xml_path    xml文件路径 |
# @param prefix_path 事件目录缀  |
# @return list                   |
#--------------------------------*
def get_cloud_xml_fname_limit(xml_path, prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    list1 = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        try:
            if (dir_names[1] == prefix_path):
                list.append(keynode.childNodes[0].nodeValue)
                sizenode = file_info.getElementsByTagName("Size")[0]
                list1.append(sizenode.childNodes[0].nodeValue)
        except IndexError:
            continue
    return list,list1
#------------------------------------------
# @ bref 在线正常流量查询				  |
#------------------------------------------
def get_cloud_query_xml_fname(xml_path, prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        try:
            dir_name = dir_names[1]+"/"+dir_names[2]
            if (dir_name == prefix_path):
                list.append(keynode.childNodes[0].nodeValue)
        except IndexError:
            continue
    return list
#------------------------------------------------
# @ bref 离线,规则mtx,apt,vds..在线正常流量查询 |
#-------------------------------------------------
def get_cloud_query_xml_fname_limit(xml_path, prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    list1 = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        try:
            dir_name = dir_names[1]+"/"+dir_names[2]
            if (dir_name == prefix_path):
                list.append(keynode.childNodes[0].nodeValue)
                sizenode = file_info.getElementsByTagName("Size")[0]
                list1.append(sizenode.childNodes[0].nodeValue)
        except IndexError:
            continue
    return list,list1

#-------------------------------------------------
# @bref 在线报表正常模式统计                         |
#-------------------------------------------------
def get_cloud_report_xml_fname(xml_path, prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        #report/2014/month  report/2014/week  report/2014/day 为了把阿里云相应目录下的对象放到本地对应目录下(month---->本地month)
        #print 'DEBUG 191 lines : %s' %(dir_names)
        try:
            dir_name_list=dir_names[1]+'/'+dir_names[2]+'/'+dir_names[3]+'/'+dir_names[4]
        #print '[dir_name++++++++++++++++++++%s]'%dir_name_list
            if (dir_name_list == prefix_path and dir_names[-1] != ""):
                list.append(keynode.childNodes[0].nodeValue)
        except IndexError:
            continue
    return list
#-------------------------------------------------------
#	@bref 报表限速模式因为报表的字段比较长，所以和其他公用模块分开了 |
#--------------------------------------------------------
def get_cloud_report_xml_fname_limit(xml_path, prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    list1 = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_names = fname.split("/")
        #print dir_names
        #report/2014/month  report/2014/week  report/2014/day 为了把阿里云相应目录下的对象放到本地对应目录下(month---->本地month)
        try:
            dir_name_list = dir_names[1]+'/'+dir_names[2]+'/'+dir_names[3]+'/'+dir_names[4]
            if (dir_name_list == prefix_path and dir_names[-1] != ""):
                list.append(keynode.childNodes[0].nodeValue)
                sizenode = file_info.getElementsByTagName("Size")[0]
                list1.append(sizenode.childNodes[0].nodeValue)
        except IndexError:
            continue
    return list,list1

def read_interface_xml():
    global uuid,isroot,cloudcon,comm_cloud_logger,interface_conf_file,ismonitor,ismanager

    root = ElementTree.parse(interface_conf_file)
    lst_node = root.getiterator("cominfo")
     
    p = subprocess.Popen(['python /gms/gapi/modules/auth/genkey.pyc'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    stdout , stderr = p.communicate()
    uuid = stdout.strip()

    cloud_node = root.find("selfinfo/devtype/cloudcon")
    cloudcon = cloud_node.text
    comm_cloud_logger.debug("uuid is %s  , cloudcon is %s" %(uuid , cloudcon))
    
    devtype_node = root.find("selfinfo/devtype/root")
    isroot = devtype_node.text

    devtype_node = root.find("selfinfo/devtype/monitor")
    ismonitor = devtype_node.text

    devtype_node = root.find("selfinfo/devtype/manager")
    ismanager = devtype_node.text 

#-----------------------------------------.
# @brief 向云端上传本地文件      |
# @param oss          远端连接句柄  |
# @param bucket       云端数据集合  |
# @param local_fname    文件目录      |
# @param remote_fname     后缀          |
#-----------------------------------------*
def cloud_up_file(oss, bucket, local_fname, remote_fname):
    t = time.time()
    try:
        res = oss.put_object_from_file(bucket, remote_fname, local_fname)
        res.read()
    except Exception as ex:
        print ex
        return -1
    if res.status != 200:
        print_color.print_upload(local_fname, remote_fname, "FAIL")
        comm_cloud_logger.debug("UP FILE[%s] -> CLOUD[%s] FAIL" %(local_fname,remote_fname))
        comm_cloud_logger.warn("[%s] ,status : [%s]" %(res.read(),res.status))
        return -1
    else:
        print_color.print_upload(local_fname, remote_fname, "SUCC")
        comm_cloud_logger.debug("[%s] UP FILE[%s] -> CLOUD[%s] SUCC" %(bucket , local_fname,remote_fname))
        print "-->spend [%f] second(s). file size[%d]." %(time.time()-t, os.path.getsize(local_fname))
        comm_cloud_logger.debug("-->spend [%f] second(s). file size[%d]."\
                     %(time.time()-t, os.path.getsize(local_fname)))
        retn = 0
    return retn
#------------------------------------------
# @brief 向云端上传本地文件,并限速        |
# @param oss   远端连接句柄               | 
# @param bucket             云端数据集合  |
# @param local_fname        文件目录      |
# @param remote_fname,      后缀          |
#-----------------------------------------
def cloud_up_file_limit(oss, bucket, local_fname, remote_fname,limit_speed):
    t = time.time()
    try:
        res = oss.put_object_from_file2(bucket,remote_fname,local_fname,limit_speed)
    except Exception, ex:
        print ex
        #comm_cloud_logger.debug("[%s] status : %s" %( ex , res.status))
        return -1
    if res.status != 200:
        print_color.print_upload(local_fname, remote_fname, "FAIL")
        comm_cloud_logger.debug("Lspeed[%s] Limit Speed Model UP FILE[%s] -> Limit Speed Model CLOUD[%s] FAIL" %(limit_speed,local_fname,remote_fname))
        print res.read()
        comm_cloud_logger.debug("[%s]" %(res.read()))
        retn = -1
    else:
        res.read()
        print_color.print_upload(local_fname, remote_fname, "SUCC")
        comm_cloud_logger.debug("[%s] Lspeed[%s] LIMIT Speed Model UP FILE[%s] -> LIMIT Speed Model CLOUD[%s] SUCC" %(bucket ,limit_speed,local_fname,remote_fname))
        comm_cloud_logger.debug("-->spend [%f] second(s). file size[%d]."\
                     %(time.time()-t, os.path.getsize(local_fname)))
        retn = 0
    return retn

#----------------------------------
#    @bref  创建用户放入报表的目录|
#----------------------------------
def creat_cloud_file(oss, bucket,cloud_name):
    try:
        res = oss.put_object_from_string(bucket,cloud_name+'/',"")
        res.read()
    except Exception, ex:
        return -1
    if res.status != 200:
        print_color.print_upload(cloud_name, "FAIL")
        comm_cloud_logger.debug("CREATE_ALI_DIR [%s]FAIL" %(cloud_name))
        retn = -1
    else:
        retn = 0
    return retn
#-----------------------------------------.
# @brief 向云端下载传本地文件         |
# @param oss                远端连接句柄  |
# @param bucket             云端数据集合  |
# @param flist              文件列           |
# @param path               本地目标目录  |
#-----------------------------------------*
def cloud_down_file(oss, bucket, flist, path):
    global totalmtx
    global g_compare_time0
    for fname in flist:
        if not fname:
            continue
        down_fname = ''
        final_name = path + os.path.basename(fname)
        down_fname = final_name + ".tmp"
        t = time.time()
        try:
            res = oss.get_object_to_file(bucket, fname, down_fname)
            res.read()
        except Exception, ex:
            print ex
            #comm_cloud_logger.debug("[%s]" %ex)
            continue
        if res.status != 200:
            print_color.print_download(fname, final_name, "FAIL")
            comm_cloud_logger.debug("CLOUD[%s] ->FILE[%s]FILE FAIL" %(fname,final_name))
            comm_cloud_logger.debug("[%s]" %(res.read()))
            break
        else:
            compare_time = time.strftime('%Y%m%d') 
            if path == comm_data.comm_me_ali_down_path :        
                if g_compare_time0 != compare_time:
                    totalmtx = 0
                    g_compare_time0 = compare_time
                else:
                    totalmtx = int(totalmtx) + 1
                print " NEW MTX COUNT[%s] ." %(totalmtx)
                comm_cloud_logger.debug(" NEW MTX COUNT[%s] ." %(totalmtx))

            print_color.print_download(fname, final_name, "SUCC")
            comm_cloud_logger.debug("[%s]  CLOUD[%s] ->FILE[%s]SUCC ." %(bucket,fname,final_name))
            comm_cloud_logger.debug("-->spend [%f] second(s). file size[%d] ."\
                     %(time.time()-t, os.path.getsize(down_fname)))
            #先将下载完成的文件进行改名
            try:
                os.rename(down_fname, final_name)
            except Exception, ex:
                print ex
                comm_cloud_logger.debug("[%s]" %ex)
                continue
            #  下载成功 删除云端对应的文件
            try:
                res = oss.delete_object(bucket, fname)
                res.read()
            except Exception, ex:
                print ex
                comm_cloud_logger.debug("[%s]" %ex)
                continue
            if (res.status != 204):
                print_color.print_delete(fname, "FAIL")
                comm_cloud_logger.debug("CLOUD[%s] FIAL  " %(fname))
                print res.read()
                comm_cloud_logger.warn("[%s]" %(res.read()))
            else:
                print_color.print_delete(fname, "SUCC")
                comm_cloud_logger.debug("CLOUD[%s] SUCC " %(fname))
#-----------------------------------------.
# @brief 限速模式向云端下载传本地文件     |
# @param oss                远端连接句柄  |
# @param bucket             云端数据集合  |
# @param flist              文件列        |
# @param path               本地目标目录  |
#-----------------------------------------*
def cloud_down_file_limit(oss, bucket,fname_list,fname_size,path,down_lim):
    global totalmtx
    global g_compare_time0
    compare_time = ''
    for fname in fname_list: 
        if not fname:
            continue
        for fsize in fname_size:
            if int(fsize) != 0 and fname.endswith('.ok'):
                down_fname = ''
                final_name = path + os.path.basename(fname)
                down_fname = final_name + ".tmp"
                t = time.time()
                try:
                    #print_color.print_downloading(fname, down_fname)
                    #comm_cloud_logger.debug("Lspeed[%s] (Limit Speed Model) CLOUD[%s] -> FILE[%s]..." %(down_lim,fname,down_fname))
                    res = oss.get_object_to_file2(bucket,fname,int(fsize),down_fname,down_lim)
                    res.read()
                except Exception, ex:
                    print ex
                    comm_cloud_logger.debug("[%s]" %ex)
                    continue
                print '^^^^^^^^^^^  read status  : ' , res.status
                if res.status not in [200, 206 ]:
                    print_color.print_download(fname, final_name, "FAIL")
                    comm_cloud_logger.debug("Lspeed[%s] (Limit Speed Model) CLOUD[%s] -> FILE[%s]FILE" %(down_lim,fname,final_name))
                    comm_cloud_logger.debug("[%s]" %(res.read()))
                    continue
                else:
                    compare_time = time.strftime('%Y%m%d') 
                    if path == comm_data.comm_me_ali_down_path :        
                        if g_compare_time0 != commpare_time:
                            totalmtx = 0
                            g_compare_time0 = commpare_time
                        else:
                            totalmtx = int(totalmtx) + 1
                        print " NEW MTX COUNT[%s] ." %(totalmtx)
                        comm_cloud_logger.debug(" NEW MTX COUNT[%s] ." %(totalmtx))

                    #print_color.print_download(fname, final_name, "SUCC")
                    #comm_cloud_logger.debug("[%s] Lspeed[%s] (Limit Speed Model) CLOUD[%s] ->FILE[%s]SUCC" %(bucket,down_lim,fname,final_name))
                    print "-->spend [%f] second(s). file size[%d]." %(time.time()-t, os.path.getsize(down_fname))
                    comm_cloud_logger.debug("-->spend [%f] second(s). file size[%d]."\
                             %(time.time()-t, os.path.getsize(down_fname)))
                    #先将下载完成的文件进行改名
                    try:
                        os.rename(down_fname, final_name)
                    except Exception, ex:
                        print ex
                        comm_cloud_logger.debug("[%s]" %ex)
                        continue
                    #  下载成功 删除云端对应的文件
                    try:
                        res = oss.delete_object(bucket, fname)
                    except Exception, ex:
                        print ex
                        comm_cloud_logger.debug("[%s]" %ex)
                        continue
                    if (res.status != 204):
                        print_color.print_delete(fname, "FAIL")
                        comm_cloud_logger.debug("Lspeed[%s] Limit Speed Model CLOUD[%s] FIAL  " %(down_lim,fname))
                        print res.read()
                        comm_cloud_logger.debug("[%s]" %(res.read()))
                    else:
                        print_color.print_delete(fname, "Limit Speed Model SUCC")
                        comm_cloud_logger.debug("Lspeed[%s] Limit Speed Model CLOUD[%s] SUCC " %(down_lim,fname))
#-----------------------------------------.
# @brief MTX离线上行处理业务              |
#-----------------------------------------*
def mtx_offline_down_busi():
    print "-------------------- Begin MTX offline DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin MTX offline DOWN BUSINESS -------------")

    while True:
        for down_file in file_up_down_tran.file_filter_bysuffix([comm_data.host_mtx_src_path], ['.ok']):
            #1. 根据拓扑进行文件名的修正及封装
            #文件名 oldname = 2014010203123301_cloudid.ok
            oldname = os.path.basename(down_file)
            #print "oldname = {%s}" %(oldname)
            name = os.path.splitext(oldname)[0]
            item = name.split("_")
            cloudid = item[1]
            topuuid_uuid = cloudid_topo.get_cloud_topo(cloudid)
            if (topuuid_uuid == "no_value"):
                print "************ file[%s]'s cloudid[%s] is not in topo. Can not find. DELETE IT!!!*********"\
                     %(down_file, cloudid)
                comm_cloud_logger.debug("************ file[%s]'s cloudid[%s] is not in topo."\
                    " Can not find. DELETE IT!!!*********" %(down_file, cloudid))
                os.remove(down_file)
                continue
            uuid = topuuid_uuid.split("_")[1]
            mtx_file = item[0] + "_" + uuid + ".ok"
            dst_file = comm_data.comm_me_ali_down_path + mtx_file
            zip_mtx_file = os.path.join(comm_data.host_mtx_src_path, mtx_file)
            os.rename(down_file, zip_mtx_file)
            #2. 对文件进行压缩，直接到ali/me/down/下面
            try:
                comm_common.createzip(dst_file, zip_mtx_file,passwd='9527')
            except Exception, ex:
                print "############creatzip Excepiton==>file[%s]-> file[%s]" %(zip_mtx_file, dst_file)
                comm_cloud_logger.debug("############creatzip Excepiton==>file[%s]-> file[%s]" %(zip_mtx_file, dst_file))
                print ex
                comm_cloud_logger.debug("[%s]" %ex)
            else:
                print "== DOWN === Create file %s SUCC!!! ======" %(dst_file)
                comm_cloud_logger.debug("== DOWN === Create file %s SUCC!!! ======" %(dst_file))
                #3. 删除该文件
                os.remove(zip_mtx_file)

        time.sleep(0.1)



#-----------------------------------------.
# @brief MTX离线上传处理业务              |
#-----------------------------------------*
def mtx_offline_up_busi():
    print "-------------------- Begin MTX offline UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin MTX offline UP BUSINESS -------------")
    while True:
        # --> mtx 上传事件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_me_ali_up_path], ['.ok']):

            #1. 得到文件 直接解压到北大博雅目录下
            try:
                comm_common.extractzip(up_file, comm_data.host_mtx_dst_path,passwd='9527')
            except Exception, ex:
                print "==extractzip Excepiton==>file[%s]" %(up_file)
                comm_cloud_logger.debug("==extractzip Excepiton==>file[%s]" %(up_file))
                print ex
                comm_cloud_logger.debug("[%s]" %ex)
                os.remove(up_file)
                continue
            else:
                print "==== UP === move file [%s] -> file[%s] SUCC!" %(up_file, comm_data.host_mtx_dst_path)
                comm_cloud_logger.debug("==== UP === move file [%s] -> file[%s] SUCC!"\
                     %(up_file, comm_data.host_mtx_dst_path))
                os.remove(up_file)

        time.sleep(0.1)

#-----------------------------------------.
# @brief 设备注册离线上行处理业务         |
#-----------------------------------------*
def reg_offline_up_busi():
    print "-------------------- Begin REG offline UP BUSINESS -------------"
    comm_cloud_logger.debug("---------- Begin REG offline UP BUSINESS -------------")
    while True:
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_reg_up_path], ['.ok']):
            #1. 将reg 入库格式 直接转换为 json 格式
            file = open(up_file, "r")
            read_str = file.readline()
            item = read_str.split('|')
            i = 0
            for i in range(1,8):
                if item[i] == '\N':
                    item[i] = ''
            json_context = "[{"+'"deviceName"'+":"+'"'+item[0]+'"'+\
                    ',"pid"'+":"+'"'+item[1]+'"'+',"uuid"'+":"+'"'\
                    +item[2]+'"'+',"ip"'+":"+'"'+item[3]+'"'+\
                    ',"customName"'+":"+'"'+item[4]+'"'+',"deviceType"'\
                    +":"+'"'+item[5]+'"'+',"ParentId"'+":"+'"'+item[6]+\
                    '"'+',"id"'+":"+'"'+item[7]+'"'+"}]"
            file.close()

            file_name = os.path.basename(up_file)
            tmp_name = file_name + ".tmp"
            final_tmp_name = os.path.join(comm_data.host_reg_dst_path, tmp_name)
            final_name = os.path.join(comm_data.host_reg_dst_path, file_name)

            #2. 将转换后的文件直接放入 comm_data.host_reg_dst_path 中去
            file = open(final_tmp_name, 'w')
            file.write(json_context)
            file.close()
            os.rename(final_tmp_name, final_name)

            #3. 删除该文件
            os.remove(up_file)
            print "===== reg UP ===== move (dbase)[%s] -> (json)[%s]" %(up_file, final_name)
            comm_cloud_logger.debug("===== reg UP ===== move (dbase)[%s] -> (json)[%s]" %(up_file, final_name))
        time.sleep(1)


def reg_json_null_to_N(file):
    if file["deviceName"]=='':
        file["deviceName"] += "\N"
    if file["pid"]=='':
        file["pid"] += "\N"
    if file["uuid"]=='':
        file["uuid"] += "\N"
    if file["ip"]=='':
        file["ip"] += "\N"
    if file["customName"]=='':
        file["customName"] += "\N"
    if file["deviceType"]=='':
        file["deviceType"] += "\N"
    if file["ParentId"]=='':
        file["ParentId"] += "\N"
    if file["id"]=='':
        file["id"] += "\N"

#-----------------------------------------.
# @brief 设备注册离线下行处理业务         |
#-----------------------------------------*
def reg_offline_down_busi():
    print "-------------------- Begin REG offline DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin REG offline DOWN BUSINESS -------------")
    while True:
        for down_file in file_up_down_tran.file_filter_bysuffix([comm_data.host_reg_src_path], ['.ok']):
            #1.将json 格式的注册文件 转换成 数据库格式
            file = open(down_file, 'r')
            json_context = json.load(file)
            file.close()

            i = 0
            # 可能会有多条注册信息，所以用for遍历
            for json_context[i] in json_context:
                reg_json_null_to_N(json_context[i])
                dbase_context = json_context[i]["deviceName"]+"|"+json_context[i]["pid"]+"|"+json_context[i]["uuid"]+"|"+json_context[i]["ip"]+"|"+json_context[i]["customName"]+"|"+json_context[i]["deviceType"]+"|"+json_context[i]["ParentId"]+"|"+str(json_context[i]["id"])

                #分配的注册ID返回码
                cloudid = json_context[i]["id"]
                #原设备UUID
                uuid = json_context[i]["uuid"]
                #原设备父节点UUID
                pid = json_context[i]["pid"]
            #2.进行顶级云拓扑建立

            #建立拓扑不支持\N模式，所以应该把\N转换为''空字符串
            if (cloudid == "\N"):
                #注册失败
                print "===== reg DOWN ====== reg fail null cloudid!"
                comm_cloud_logger.debug("===== reg DOWN ====== reg fail null cloudid!")
            else:
                #注册成功
                #根据cloudid, uuid, pid 建立拓扑结构
                if (pid == "\N"):
                    pid = ''
                cloudid_topo.creat_cloud_topo(cloudid, uuid, pid)

            reg_file_name = "reg_" + uuid + ".ok"
            final_file = os.path.join(comm_data.comm_reg_down_path, reg_file_name)
            final_tmp_file = final_file + ".tmp"

            #3.将转换成数据库格式的文件 直接拷贝到 comm_data.comm_reg_down_path中
            file = open(final_tmp_file, 'w')
            file.write(dbase_context)
            file.close()
            os.rename(final_tmp_file, final_file)
            #4.删除该文件
            os.remove(down_file)
            print "======= reg DOWN ======= move (json)[%s] -> (dbase)[%s]" %(down_file, final_file)
            comm_cloud_logger.debug("======= reg DOWN ======= move (json)[%s] -> (dbase)[%s]" %(down_file, final_file))
        time.sleep(1)

#-----------------------------------------.
# @brief 自动注册    离线处理业务         |
#-----------------------------------------*
def reg_offline_autoreg_busi():
    print "-------------------- Begin REG offline AUTO BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin REG offline AUTO BUSINESS -------------")
    while True:
        for reg_file in file_up_down_tran.file_filter_bysuffix([comm_data.host_reg_dst_path], ['.ok']):
            dst_file = os.path.join(comm_data.host_reg_src_path, os.path.basename(reg_file))
            readed = json.load(open(reg_file, 'r'))

            i = 0
            # 根据IP 分配 ID
            for readed[i] in readed:
                ip= str(readed[i]["ip"])
                #m = id.split('-')[4]
                readed[i]["id"]= str(socket.ntohl(struct.unpack('I', socket.inet_aton(str(ip)))[0]))[0:8]
                #if readed[i]["ip"] == "192.168.1.180":
                #    readed[i]["id"] = "20000180"
                #elif readed[i]["ip"] == "192.168.1.166":
                #    readed[i]["id"] = "20000166"
                print "==== Auto Reg ===== file[%s],IP[%s] --> Creat ID[%s]" %(reg_file,\
                     readed[i]["ip"], readed[i]["id"])
                comm_cloud_logger.debug("==== Auto Reg ===== file[%s],IP[%s] --> Creat ID[%s]"\
                     %(reg_file, readed[i]["ip"], readed[i]["id"]))

            json.dump(readed, open(dst_file, 'w'))
            print "==== Auto Reg ===== move file[%s] -> file[%s]" %(reg_file, dst_file)
            comm_cloud_logger.debug("==== Auto Reg ===== move file[%s] -> file[%s]" %(reg_file, dst_file))
            os.remove(reg_file)

        time.sleep(1)

#------------------------------------------------------.
# @brief 规则库，白名单，升级包，配置 离线业务         |
#------------------------------------------------------*
def conf_offline_down_busi():
    print "-------------------- Begin CONF offline DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin CONF offline DOWN BUSINESS -------------")
    dir_list = ((comm_data.host_rule_src_path, comm_data.comm_re_down_path),                # 规则库
                (comm_data.host_conf_src_path, comm_data.comm_conf_down_path),             # 配置
                (comm_data.host_update_src_path, comm_data.comm_ue_down_path),     # 升级包
                (comm_data.whitelist_src_path, comm_data.whitelist_dst_path),   # 白名单
                )
    while True:
        try:
            conf_down_file_parser(dir_list)
        except Exception:
            print("**** ERROR ****")
            comm_cloud_logger.debug("*****ERROR*****")
            print(traceback.format_exc())
            comm_cloud_logger.debug("[%s]" %(traceback.format_exc()))
        finally:
            time.sleep(1)

#------------------------------------------------------.
# @brief 规则库，白名单，升级包，配置 处理流程         |
#------------------------------------------------------*
def conf_down_file_parser(dir_list):
    import json
    import traceback
    FTYPE_UNKNOWN = 0
    FTYPE_DEVID_BROADCAST = 1
    FTYPE_DEVID_MULTICAST = 2

    def sumfile(fobj):
        m = md5.new()
        while True:
            d = fobj.read(8096)
            if not d:
                break
            m.update(d)
        return m.hexdigest()

    def checksum(fname):
        ''' 从文件名中获得 md5 值校验文件内容 '''
        try:
            fname_md5 = fname.split('-')[2].upper()
        except IndexError:
            # 文件名异常暂时在这里重命名备份
            print("**** WARNING **** file[%s] name illegal. backup.")
            comm_cloud_logger.debug("**** WARNING **** file[%s] name illegal. backup.")
            os.rename(fname, fname + '.bak')
            return False
        else:
            fdata_md5 = None
            with open(fname, 'r') as fp:
                fdata_md5 = sumfile(fp).upper()
            if fname_md5 == fdata_md5:
                return True
            else:
                return False

    def file_type(fname):
        ftype = fname.split('_')[-1]
        if ftype == 'all.ok':
            return FTYPE_DEVID_BROADCAST
        elif ftype == 'devidlist.ok':
            return FTYPE_DEVID_MULTICAST
        else:
            return FTYPE_UNKNOWN

    def multicast_src_file_list(src_dir):
        for fname in os.listdir(src_dir):
            fname = os.path.join(src_dir, fname)
            if os.path.isfile(fname) and file_type(fname) == FTYPE_DEVID_MULTICAST:
                # 切掉结尾的 _devidlist.ok 就是对应的数据文件的名称
                devid_file = fname
                src_file = fname[:-13] + '.ok'
                if os.path.exists(src_file) and os.path.isfile(src_file):
                    if src_dir == comm_data.host_update_src_path:
                        # 对升级包作 md5 校验
                        # 约定先拷贝 _devlist.ok 文件，后拷贝升级包文件
                        # 升级包格式如下:
                        #GMS-1.1.1.2-b227c5683aecd4c0a21f1c32468c3f1e-normal_devlist.ok
                        #GMS-1.1.1.2-b227c5683aecd4c0a21f1c32468c3f1e-normal.ok
                        if checksum(src_file) == True:
                            yield src_file, devid_file
                    else:
                        yield src_file, devid_file

    def broadcast_src_file_list(src_dir):
        for fname in os.listdir(src_dir):
            fname = os.path.join(src_dir, fname)
            if os.path.isfile(fname) and file_type(fname) == FTYPE_DEVID_BROADCAST:
                if src_dir == comm_data.host_update_src_path:
                    # 对升级包作 md5 校验
                    # 升级包格式如下:
                    #GMS-1.1.1.2-b227c5683aecd4c0a21f1c32468c3f1e-normal_all.ok
                    if checksum(fname) == True:
                        yield fname
                else:
                    yield fname

    def multi_file_copy(src_file, dst_file_list):
        for dst_file in dst_file_list:
            dst_tmp_file = dst_file + ".tmp"
            comm_common.createzip(dst_tmp_file,src_file,passwd='9527')
            os.rename(dst_tmp_file, dst_file)

    def make_dst_uuid_list(devid_file):
        uuid_list = []
        with open(devid_file) as fp:
            data = fp.read()
            try:
                # 暂定 json 文件格式为 {"devidlist": ["devid1", "devid2", ...]}
                uuid_dict = json.loads(data)
                uuid_list = uuid_dict['devidlist']
            except Exception:
                # json 文件解析出错时。会返回空列表。最后在主流程里备份出错的文件。
                print("**** ERROR ****")
                comm_cloud_logger.debug("******ERROR*****")
                print(traceback.format_exc())
                comm_cloud_logger.debug("[%s]" %(traceback.format_exc()))
                uuid_list = []
        return uuid_list

    def multicast_dst_file_list(src_file, devid_file, dst_dir):
        uuid_list = make_dst_uuid_list(devid_file)
        file_list = []
        fbase_name = os.path.split(src_file)[-1][:-3] # 切掉尾部的 .ok
        print fbase_name
        for uuid in uuid_list:
            dst_file = os.path.join(dst_dir, fbase_name + '_' + uuid + '.ok')
            file_list.append(dst_file)
        return file_list

    def broadcast_dst_file_list(src_file, dst_dir):
        uuid_list = cloudid_topo.get_all_uuid_list()
        file_list = []
        fbase_name = os.path.split(src_file)[-1][:-7] # 切掉尾部的 _all.ok
        for uuid in uuid_list:
            dst_file = os.path.join(dst_dir, fbase_name + '_' + uuid + '.ok')
            file_list.append(dst_file)
        return file_list


    for src_dir, dst_dir in dir_list:
        for src_file, devid_file in multicast_src_file_list(src_dir):
            dst_file_list = multicast_dst_file_list(src_file, devid_file, dst_dir)
            print("src file[%s] devid file[%s]" % (src_file, devid_file))
            comm_cloud_logger.debug("src file[%s] devid file[%s]" % (src_file, devid_file))
            print("dst file list[%s]" % dst_file_list)
            comm_cloud_logger.debug("dst file list[%s]" % dst_file_list)
            if dst_file_list == []:
                print("**** WARNING **** multicast dst uuid list == null")
                comm_cloud_logger.debug("**** WARNING **** multicast dst uuid list == null")
                os.rename(src_file, src_file + '.bak')
                os.rename(devid_file, devid_file + '.bak')
            else:
                multi_file_copy(src_file, dst_file_list)
                os.remove(src_file)
                os.remove(devid_file)

        for src_file in broadcast_src_file_list(src_dir):
            dst_file_list = broadcast_dst_file_list(src_file, dst_dir)
            print("src file[%s]" % src_file)
            comm_cloud_logger.debug("src file[%s]" % src_file)
            print("dst file list[%s]" % dst_file_list)
            comm_cloud_logger.debug("dst file list[%s]" % dst_file_list)
            if dst_file_list == []:
                print("**** WARNING **** broadcast dst uuid list == null")
                comm_cloud_logger.debug("**** WARNING **** broadcast dst uuid list == null")
                os.rename(src_file, src_file + '.bak')
            else:
                multi_file_copy(src_file, dst_file_list)
                os.remove(src_file)

#-----------------------------------------.
# @brief 设备注册在线上行处理业务         |
#-----------------------------------------*
def reg_online_up_busi():
    print "-------------------- Begin REG online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin REG online UP BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> reg 注册文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_reg_up_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_data.REG_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief 限速-设备注册在线上行处理业务    |
#-----------------------------------------*
def reg_online_up_limit_busi():
    print "-------------------- Begin Limit Speed Model REG online UP BUSINESS -------------"
    comm_cloud_logger.debug("-----------Limit Speed Model Begin REG online UP BUSINESS ------")
    global uuid
    global limit_speed
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> reg 注册文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_reg_up_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_data.REG_FILE, os.path.basename(up_file))
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief 限速模式设备注册在线下行处理业务 |
#-----------------------------------------*
def reg_online_down_limit_busi():
    print "---------- Begin Limit Speed Model REG online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin Limit Speed Model REG online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_reg_down_limit_filelist.xml'
    uuidtmp = uuid +'/'+comm_data.REG_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.warn("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
            continue

        # --> reg 注册文件下载
        try:
            reg_fname_list,reg_fname_size=get_cloud_xml_fname_limit(tmp_file, comm_data.REG_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.debug("%s,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,reg_fname_list,reg_fname_size,comm_data.comm_reg_down_path,down_lim)

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)

#-----------------------------------------.
# @brief 设备注册在线下行处理业务         |
#-----------------------------------------*
def reg_online_down_busi():
    print "-------------------- Begin REG online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin REG online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_reg_down_filelist.xml'
    uuidtmp = uuid +'/'+comm_data.REG_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.debug("[%s]" %ex)
            continue

        # --> reg 注册文件下载
        try:
            reg_fname_list = get_cloud_xml_fname(tmp_file, comm_data.REG_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] and ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file(oss, bucket, reg_fname_list, comm_data.comm_reg_down_path)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

#-----------------------------------------.
# @brief MTX 限速模式 在线上传处理业务    |
#-----------------------------------------*
def mtx_online_up_limit_busi():
    print "-------------------- Begin Limit Speed Model MTX online UP BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin Limit Speed Model MTX nline UP BUSINESS --------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> mtx 上传事件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_me_ali_up_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_data.MTX_FILE, os.path.basename(up_file))
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if retn == 0:
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief MTX在线上传处理业务              |
#-----------------------------------------*
def mtx_online_up_busi():
    print "-------------------- Begin MTX  online UP BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin MTX  online UP BUSINESS --------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> mtx 上传事件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_me_ali_up_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_data.MTX_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if retn == 0:
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief REPORT 在线上传处理业务          |
#-----------------------------------------
def report_online_up_busi():
    print "-------------------- Begin REPORT online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin REPORT online UP BUSINESS -------------")
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        YEAR = time.strftime('%Y',time.localtime(time.time()))
         # 报表产生的源目录
        report_src_year_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'year'
        report_src_month_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'month'
        report_src_week_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'week'
        report_src_day_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'day'

		# add by zdw 
        report_src_year_dir_last = comm_data.comm_report_up_path+'/'+str(int(YEAR)-1)+'/'+'year'
        report_src_month_dir_last = comm_data.comm_report_up_path+'/'+str(int(YEAR)-1)+'/'+'month'
        report_src_week_dir_last = comm_data.comm_report_up_path+'/'+str(int(YEAR)-1)+'/'+'week'
        report_src_day_dir_last  = comm_data.comm_report_up_path+'/'+str(int(YEAR)-1)+'/'+'day'


        #报表需要拷贝的目录，避免重复长传处理完的文件
        report_dst_year_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'year'
        report_dst_month_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'month'
        report_dst_week_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'week'
        report_dst_day_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'day'

		#add by zdw
        report_dst_year_dir_last = comm_data.comm_report_down_path+'/'+str(int(YEAR)-1)+'/'+'year'
        report_dst_month_dir_last = comm_data.comm_report_down_path+'/'+str(int(YEAR)-1)+'/'+'month'
        report_dst_week_dir_last = comm_data.comm_report_down_path+'/'+str(int(YEAR)-1)+'/'+'week'
        report_dst_day_dir_last = comm_data.comm_report_down_path+'/'+str(int(YEAR)-1)+'/'+'day'
        #上传阿里云的目录
        report_up_year_src_path = comm_data.REPORT_UP_FILE + '/' + YEAR + '/'+'year'
        report_up_month_src_path = comm_data.REPORT_UP_FILE + '/' + YEAR + '/'+'month'
        report_up_week_src_path = comm_data.REPORT_UP_FILE +'/'+YEAR + '/'+'week'
        report_up_day_src_path = comm_data.REPORT_UP_FILE +'/' + YEAR +'/'+'day'
		
        report_up_year_src_path_last = comm_data.REPORT_UP_FILE + '/' + str(int(YEAR)-1) + '/'+'year'
        report_up_month_src_path_last = comm_data.REPORT_UP_FILE + '/' + str(int(YEAR)-1) + '/'+'month'
        report_up_week_src_path_last = comm_data.REPORT_UP_FILE +'/'+ str(int(YEAR)-1) + '/'+'week'
        report_up_day_src_path_last = comm_data.REPORT_UP_FILE +'/' + str(int(YEAR)-1) +'/'+'day'

        #跨年，动态建立目录 
        compare = "/data/tmpdata/comm/report/2015"
        com_data = compare.split("/")[-1]
        if com_data != YEAR:
            comm_common.creat_if_dir_notexist(report_dst_year_dir)
            comm_common.creat_if_dir_notexist(report_dst_month_dir)
            comm_common.creat_if_dir_notexist(report_dst_week_dir)
            comm_common.creat_if_dir_notexist(report_dst_day_dir)

        #copy /data/permdata/report/-------->/data/tmpdata/comm/report 
        cp_src_to_dst=((report_src_year_dir,report_dst_year_dir),
            (report_src_month_dir,report_dst_month_dir),
            (report_src_week_dir,report_dst_week_dir),
            (report_src_day_dir,report_dst_day_dir),
            (report_src_year_dir_last,report_dst_year_dir_last),
            (report_src_month_dir_last,report_dst_month_dir_last),
            (report_src_week_dir_last,report_dst_week_dir_last),
            (report_src_day_dir_last,report_dst_day_dir_last))
        
        #up /data/permdata/report/--------->/cloud/
        dir_list=((report_src_year_dir, report_up_year_src_path),
            (report_src_month_dir, report_up_month_src_path),
            (report_src_week_dir, report_up_week_src_path),
            (report_src_day_dir, report_up_day_src_path),
            (report_src_year_dir_last, report_up_year_src_path_last),
            (report_src_month_dir_last, report_up_month_src_path_last),
            (report_src_week_dir_last, report_up_week_src_path_last),
            (report_src_day_dir_last, report_up_day_src_path_last))
        # --> report 上传事件
        for src_dir,dst_dir in dir_list:
            #print '1.[%s] --->[%s]'%(src_dir,dst_dir)
            if not os.path.exists(src_dir) or not os.path.isdir(src_dir):
                pass
            else:
                for src,dst in cp_src_to_dst:
                    if src != src_dir:
                        continue
                    else:
                        for up_file in file_up_down_tran.file_filter_bysuffix([src_dir], ['.doc']):
                            cloud_dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                            #上传成功,拷贝一份到/data/tmpdata/comm/report/
                            retn = cloud_up_file(oss, bucket, up_file, cloud_dst_file)
                            if retn == 0:
                                other_dst_file = os.path.join(dst,os.path.basename(up_file))
                                shutil.copy(up_file,other_dst_file) 
                                print_color.print_copy(up_file,other_dst_file)
                                comm_cloud_logger.debug("==REPORT CP [%s]--->[%s]======" %(up_file,other_dst_file))
                                #删除源文件
                                os.remove(up_file)
                                print_color.print_delete_local(up_file)
                            else:
                                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief REPORT 限速-在线上传处理业务     |
#-----------------------------------------
def report_online_up_limit_busi():
    print "-------------------- Begin REPORT Limit Speed Model online UP BUSINESS -------------"
    comm_cloud_logger.debug("----------- Begin REPORT Limit Speed Model online UP BUSINESS -------")
    global uuid

    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    while True:
        YEAR = time.strftime('%Y',time.localtime(time.time()))
         # 报表产生的源目录
        report_src_year_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'year'
        report_src_month_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'month'
        report_src_week_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'week'
        report_src_day_dir = comm_data.comm_report_up_path+'/'+YEAR+'/'+'day'
        #报表需要拷贝的目录，避免重复长传处理完的文件
        report_dst_year_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'year'
        report_dst_month_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'month'
        report_dst_week_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'week'
        report_dst_day_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'day'
        #上传阿里云的目录
        report_up_year_src_path = comm_data.REPORT_UP_FILE + '/' + YEAR + '/'+'year'
        report_up_month_src_path = comm_data.REPORT_UP_FILE + '/' + YEAR + '/'+'month'
        report_up_week_src_path = comm_data.REPORT_UP_FILE +'/'+YEAR + '/'+'week'
        report_up_day_src_path = comm_data.REPORT_UP_FILE +'/' + YEAR +'/'+'day'
        #跨年，动态建立目录 
        compare = "/data/tmpdata/comm/report/2014"
        com_data = compare.split("/")[-1]
        if com_data != YEAR:
            comm_common.creat_if_dir_notexist(report_dst_year_dir)
            comm_common.creat_if_dir_notexist(report_dst_month_dir)
            comm_common.creat_if_dir_notexist(report_dst_week_dir)
            comm_common.creat_if_dir_notexist(report_dst_day_dir)

        #copy /data/permdata/report/-------->/data/tmpdata/comm/report 
        cp_src_to_dst=((report_src_year_dir,report_dst_year_dir),
            (report_src_month_dir,report_dst_month_dir),
            (report_src_week_dir,report_dst_week_dir),
            (report_src_day_dir,report_dst_day_dir))
        
        #up /data/permdata/report/--------->/cloud/
        dir_list=((report_src_year_dir, report_up_year_src_path),
            (report_src_month_dir, report_up_month_src_path),
            (report_src_week_dir, report_up_week_src_path),
            (report_src_day_dir, report_up_day_src_path))
        # --> report 上传事件
        for src_dir,dst_dir in dir_list:
            #print '1.[%s] --->[%s]'%(src_dir,dst_dir)
            if not os.path.exists(src_dir) or not os.path.isdir(src_dir):
                pass
            else:
                for src,dst in cp_src_to_dst:
                    if src != src_dir:
                        continue
                    else:
                        #print '2.[%s] --->[%s]'%(src_dir,dst)
                        for up_file in file_up_down_tran.file_filter_bysuffix([src_dir], ['.doc']):
                            print up_file
                            cloud_dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                            #上传成功,拷贝一份到/data/tmpdata/comm/report/
                            #retn = cloud_up_file(oss, bucket, up_file, cloud_dst_file)
                            print cloud_dst_file
                            retn = cloud_up_file_limit(oss, bucket,up_file,cloud_dst_file,up_lim)
                            if retn == 0:
                                print '@@@@@@@[%s]@@@@@@@'%dst
                                other_dst_file = os.path.join(dst,os.path.basename(up_file))
                                shutil.copy(up_file,other_dst_file) 
                                print_color.print_copy(up_file,other_dst_file)
                                comm_cloud_logger.debug("==REPORT CP [%s]--->[%s]======" %(up_file,other_dst_file))
                                #删除源文件
                                os.remove(up_file)
                                print_color.print_delete_local(up_file)
                            else:
                                oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)

#-----------------------------------------.
# @brief REPORT 在线下传处理业务           |
#-----------------------------------------*
def report_online_down_busi():
    print "-------------------- Begin QUERY online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin QUERY online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_report_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.REPORT_FILE

    while True:
    # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue
    # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.debug("[%s]" %ex)
            continue

        YEAR = time.strftime('%Y',time.localtime(time.time())) #动态获取日期年
        #阿里云上提供用户的下载目录
        report_down_year_src_path = comm_data.REPORT_DOWN_FILE + '/' + YEAR + '/'+'year'
        report_down_month_src_path = comm_data.REPORT_DOWN_FILE + '/' + YEAR + '/'+'month'
        report_down_week_src_path = comm_data.REPORT_DOWN_FILE +'/'+YEAR + '/'+'week'
        report_down_day_src_path = comm_data.REPORT_DOWN_FILE +'/' + YEAR +'/'+'day'
        #下载到本地目录中
        report_dst_year_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'year'+'/'
        report_dst_month_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'month'+'/'
        report_dst_week_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'week'+'/'
        report_dst_day_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'day'+'/'
        #整理一个映射列表
        dir_list=((report_dst_year_dir,report_down_year_src_path),
            (report_dst_month_dir,report_down_month_src_path),
            (report_dst_week_dir,report_down_week_src_path),
            (report_dst_day_dir,report_down_day_src_path))

        # --> report 下载事件
        for dst_dir,src_dir in dir_list:
            dst_file = os.path.join(str(uuid), src_dir)
            try:
                retn = creat_cloud_file(oss, bucket,dst_file) #创建提供用户放入报表的目录
                if retn == 0:
                    pass
                report_fname_list = get_cloud_report_xml_fname(tmp_file, src_dir) #返回桶内的列表
                cloud_down_file(oss, bucket, report_fname_list,dst_dir) #根据列表下载对应的列表
            except Exception,ex:
                #print '%s' %(traceback.format_exc())
                comm_cloud_logger.warn("%s" %(ex))
                continue

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception ,ex:
            pass

        time.sleep(1)
#-----------------------------------------.
# @brief REPORT 限速模式-在线下传处理业务 |
#-----------------------------------------*
def report_online_down_limit_busi():
    print "-------------------- Begin Limit Speed Model QUERY online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("----------- Begin Limit Speed Model QUERY online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_report_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.REPORT_FILE

    while True:
    # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue
    # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
            continue

        YEAR = time.strftime('%Y',time.localtime(time.time())) #动态获取日期年
        #阿里云上提供用户的下载目录
        report_down_year_src_path = comm_data.REPORT_DOWN_FILE + '/' + YEAR + '/'+'year'
        report_down_month_src_path = comm_data.REPORT_DOWN_FILE + '/' + YEAR + '/'+'month'
        report_down_week_src_path = comm_data.REPORT_DOWN_FILE +'/'+YEAR + '/'+'week'
        report_down_day_src_path = comm_data.REPORT_DOWN_FILE +'/' + YEAR +'/'+'day'
        #下载到本地目录中
        report_dst_year_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'year'+'/'
        report_dst_month_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'month'+'/'
        report_dst_week_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'week'+'/'
        report_dst_day_dir = comm_data.comm_report_down_path+'/'+YEAR+'/'+'day'+'/'
        #整理一个映射列表
        dir_list=((report_dst_year_dir,report_down_year_src_path),
            (report_dst_month_dir,report_down_month_src_path),
            (report_dst_week_dir,report_down_week_src_path),
            (report_dst_day_dir,report_down_day_src_path))

        # --> report 下载事件
        for dst_dir,src_dir in dir_list:
            dst_file = os.path.join(str(uuid), src_dir)
            retn = creat_cloud_file(oss, bucket,dst_file) #创建提供用户放入报表的目录
            if retn == 0:
               pass
            #else:
            #    print 'creat dir [%s] fail!!!!' %dst_file
            #    print_color.print_creat(dst_file)
            #    comm_cloud_logger.debug("----Limit Speed Model CREAT REPORT CLOUD DIR[%s] FAIL!!--------"%dst_file)
            try:
                fname_list,fname_size = get_cloud_report_xml_fname_limit(tmp_file, src_dir) #返回桶内的列表
            except Exception,ex:
                os.remove(tmp_file)
                comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
                continue
            cloud_down_file_limit(oss,bucket,fname_list,fname_size,dst_dir,down_lim) #根据列表下载对应的列表

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception ,ex:
            pass

        time.sleep(1)

#-----------------------------------------.
# @brief QUERY在线上传处理业务              |
#-----------------------------------------*
def query_online_up_busi():
    print "-------------------- Begin QUERY online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin QUERY online UP BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_ali_path , query_flow_src_path),     
                    (comm_data.comm_query_url_ali_path, query_url_src_path),       
                    (comm_data.comm_query_dns_ali_path, query_dns_src_path),    
                    )
        for src_dir,dst_dir in dir_list:
            for up_file in file_up_down_tran.file_filter_bysuffix([src_dir], ['.ok']):
                dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                retn = cloud_up_file(oss, bucket, up_file, dst_file)
                if retn == 0:
                    #上传成功 删除本地文件
                    os.remove(up_file)
                else:
                    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief QUERY 限速模块在线上传处理业务              |
#-----------------------------------------*
def query_online_up_limit_busi():
    print "-------------- Begin QUERY Limit Speed Model online UP BUSINESS -------------"
    comm_cloud_logger.debug("---------- Begin QUERY Limit Speed Model online UP BUSINESS --------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_ali_path , query_flow_src_path),     
                    (comm_data.comm_query_url_ali_path, query_url_src_path),       
                    (comm_data.comm_query_dns_ali_path, query_dns_src_path),    
                    )
        for src_dir,dst_dir in dir_list:
            for up_file in file_up_down_tran.file_filter_bysuffix([src_dir], ['.ok']):
                dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
                if retn == 0:
                    #上传成功 删除本地文件
                    os.remove(up_file)
                else:
                    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief MTX在 在线下传处理业务           |
#-----------------------------------------*
def mtx_online_down_busi():
    print "-------------------- Begin MTX online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin MTX online DOWN BUSINESS -------------")
    global uuid , g_compare_time0
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_mtx_down_filelist.xml'
    g_compare_time0 = time.strftime('%Y%m%d')

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
            continue

        # --> mtx 下载事件
        try:
            mtx_fname_list = get_cloud_xml_fname(tmp_file, comm_data.MTX_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] and ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_me_ali_down_path)

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)

#-----------------------------------------.
# @brief 限速MTX在 在线下传处理业务       |
#-----------------------------------------*
def mtx_online_down_limit_busi():
    print "---------------- Begin Limit Speed Model MTX online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("------------ Begin Limit Speed Model MTX online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_mtx_down_limit_filelist.xml'
    uuidtmp = uuid + '/'+comm_data.MTX_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue
        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
            continue
        # --> mtx 下载事件
        try:
            mtx_fname_list,mtx_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.MTX_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] and ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,mtx_fname_list,mtx_fname_size,comm_data.comm_me_ali_down_path,down_lim)

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)

def delete_event_busi(del_dir):
    while True:
        for del_file in file_up_down_tran.file_filter_bysuffix([del_dir], ['.ok']):
            try:
                os.remove(del_file)
                print "delete [%s] is success!!!!!!!!!!!!!" %del_file
                comm_cloud_logger.debug("delete [%s] is success!!!!!!!!!!!!!!" %del_file)
            except Exception,ex:
                print ex
                comm_cloud_logger.debug(ex)
                print "delete [%s] is fail!!!!!!!!!!!!!" %del_file
                comm_cloud_logger.debug("delete [%s] is fail!!!!!!!!!!!!!!" %del_file)
                continue
        time.sleep(2)
    
# 离线模式下删除事件
def offline_delete_event_busi():
    print "-------------------- Begin offline DELETE  BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin offline DELETE BUSINESS -------------")
    del_list = (comm_data.comm_flow_ali_path,
                comm_data.comm_devstat_ali_path,
                comm_data.comm_ve_ali_path,
                comm_data.comm_surl_ali_path,
                comm_data.comm_ae_ali_path,) 
    while True:
        for del_dir in del_list:
            for del_file in file_up_down_tran.file_filter_bysuffix([del_dir], ['.ok']):
                try:
                    os.remove(del_file)
                    print "delete [%s] is success!!!!!!!!!!!!!" %del_file
                    comm_cloud_logger.debug("delete [%s] is success!!!!!!!!!!!!!!" %del_file)
                except Exception,ex:
                    print ex
                    comm_cloud_logger.debug(ex)
                    print "delete [%s] is fail!!!!!!!!!!!!!" %del_file
                    comm_cloud_logger.debug("delete [%s] is fail!!!!!!!!!!!!!!" %del_file)
                    continue
        time.sleep(2)
    
#------------------------------------
# @bref 正常模式在线正常流量查询    |
#-----------------------------------
def query_online_down_busi():
    print "---------------- Begin  QUERY online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin  QUERY online DOWN BUSINESS ---------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_query_down_filelist.xml'
    uuidtmp = uuid +'/'+'query'

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]" %ex)
            continue
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_down_path , query_flow_src_path),     
                    (comm_data.comm_query_url_down_path, query_url_src_path),       
                    (comm_data.comm_query_dns_down_path, query_dns_src_path),    
                    )
        # --> query 下载事件
        for dst_dir,src_dir in dir_list:
            try:
                query_fname_list = get_cloud_query_xml_fname(tmp_file, src_dir)
            except Exception,ex:
                os.remove(tmp_file)
                comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
                continue
            cloud_down_file(oss, bucket, query_fname_list, dst_dir)

        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)
    
#------------------------------------
# @bref 限速模式在线正常流量查询    |
#-----------------------------------
def query_online_down_limit_busi():
    print "---------------- Begin Limit Speed Model QUERY online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin Limit Speed Model QUERY online DOWN BUSINESS ---------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_query_down_limit_filelist.xml'
    uuidtmp = uuid +'/'+'query'

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue
        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.debug("[%s]" %ex)
            continue
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_down_path , query_flow_src_path),     
                    (comm_data.comm_query_url_down_path, query_url_src_path),       
                    (comm_data.comm_query_dns_down_path, query_dns_src_path),    
                    )
        # --> query 下载事件
        for dst_dir,src_dir in dir_list:
            try:
                query_fname_list,query_fname_size = get_cloud_query_xml_fname_limit(tmp_file, src_dir)
            except Exception,ex:
                os.remove(tmp_file)
                comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
                continue
            cloud_down_file_limit(oss,bucket,query_fname_list,query_fname_size, dst_dir,down_lim)
        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)

# 
def rule_mtx_online_down_limit_busi():
    #if comm_data.if_rule_flag == 'no':
    #    print "----------------  MTX CONF FLAG {close} BUSINESS -------------"
    #    comm_cloud_logger.debug("------ MTX CONF FLAG {close} BUSINESS -------------")
    #    return
    print "------------- Begin Limit Speed Model rule_mtx conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model MTX CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/mtx_rule_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.MTX_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # --> mtx规则库 下载事件
        try:
            rule_fname_list,rule_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.MTX_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,rule_fname_list,rule_fname_size,comm_data.comm_mtx_rule_down_path,down_lim)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
# update
def update_online_down_limit_busi():
    print "------------- Begin Limit Speed Model update conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model UPDATE CONF online DOWN BUSINESS -------")
    global uuid,down_lim
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/update_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.UPDATE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # --> 升级包 下载事件
        try:
            up_fname_list,up_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.UPDATE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,up_fname_list,up_fname_size,comm_data.comm_ue_down_path,down_lim)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)

# rule_vds_limit_down
def rule_vds_online_down_limit_busi():
    print "------------- Begin Limit Speed Model rule_vds conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model VDS CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/update_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.VDS_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.warn("[%s]"%ex)
            continue

        # --> vds规则库 下载事件
        try:
            vds_fname_list,vds_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.VDS_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
       
        cloud_down_file_limit(oss, bucket,vds_fname_list,vds_fname_size,comm_data.comm_vds_rule_down_path,down_lim)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)

# rule_surl_limit_down
def rule_surl_online_down_limit_busi():
    print "------------- Begin Limit Speed Model rule_surl conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model SURL CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/rule_surl_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.SURL_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.warn("[%s]"%ex)
            continue

        # --> surl规则库 下载事件
        try:
            surl_fname_list,surl_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.SURL_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,surl_fname_list,surl_fname_list,comm_data.comm_surl_rule_down_path,down_lim)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
# rule_apt_limit_down
def rule_apt_online_down_limit_busi():
    print "------------- Begin Limit Speed Model rule_apt conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model APT CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/update_down_limit_filelist.xml'

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            comm_cloud_logger.warn("[%s]"%ex)
            continue

        # --> apt规则库 下载事件
        try:
            apt_fname_list,apt_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.APT_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss, bucket,apt_fname_list,apt_fname_size,comm_data.comm_apt_rule_down_path,down_lim)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)
# config_online_list_limit_down
def conf_online_down_limit_busi():
    print "------------- Begin Limit Speed Model  conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model  CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/conf_online_down_limit_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.CONF_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue

        # --> 配置 下载事件
        try:
            conf_fname_list,conf_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.CONF_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("[%s] ,delete %s" %(ex,tmp_file))
            continue
        cloud_down_file_limit(oss,bucket,conf_fname_list,conf_fname_size,comm_data.comm_conf_down_path,down_lim)

        # --> 白名单 不需要进行从阿里云下载， 而是需要解析顶级设备目录
        dir_list = ((comm_data.whitelist_src_path, comm_data.whitelist_dst_path),)
        try:
            conf_down_file_parser(dir_list)
        except Exception:
            print("**** ERROR ****")
            comm_cloud_logger.debug("**** ERROR ****")
            print(traceback.format_exc())
            comm_cloud_logger.debug("[%s]" %(traceback.format_exc()))
        # 4. 删除xml临时文件
        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)
#------------------------------------------------------.
# @brief 开启MTX规则下发 在线业务                      |
#------------------------------------------------------*
def rule_mtx_online_down_busi():
    #if comm_data.if_rule_flag == 'no':
    #   print "----------------  MTX CONF FLAG {close} BUSINESS -------------"
    #    comm_cloud_logger.debug("------ MTX CONF FLAG {close} BUSINESS -------------")
    #    return
    print "-------------------- Begin MTX CONF online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin MTX CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_mtx_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.MTX_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue

        # --> mtx规则库 下载事件
        try:
            mtx_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.MTX_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_mtx_rule_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        time.sleep(2)
#------------------------------------------------------.
# @brief 开启VDS规则下发 在线业务                      |
#------------------------------------------------------*
def rule_vds_online_down_busi():
    print "-------------------- Begin VDS CONF online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin VDS CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_vds_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.VDS_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try:
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # --> vds规则库 下载事件
        try:
            vds_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.VDS_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket, vds_fname_list, comm_data.comm_vds_rule_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
#------------------------------------------------------.
# @brief 开启升级包规则下发 在线业务                      |
#------------------------------------------------------*
def update_online_down_busi():
    print "-------------------- Begin UPDATE online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin UPDATE CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_update_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.UPDATE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # --> 升级包 下载
        try:
            update_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.UPDATE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket,update_fname_list, comm_data.comm_ue_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
#------------------------------------------------------.
# @brief 开启apt规则下发 在线业务                      |
#------------------------------------------------------*
def rule_apt_online_down_busi():
    print "-------------------- Begin apt conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin APT CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_apt_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.APT_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # -->  apt 规则下载
        try:
            apt_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.APT_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket, apt_fname_list, comm_data.comm_apt_rule_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
#------------------------------------------------------.
# @brief 开启surl规则下发 在线业务                      |
#------------------------------------------------------*
def rule_surl_online_down_busi():
    print "-------------------- Begin surl online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin surl CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_surl_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.SURL_RULE_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # -->  下载
        try:
            surl_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.SURL_RULE_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket, surl_fname_list, comm_data.comm_surl_rule_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass
        time.sleep(2)
#------------------------------------------------------.
# @brief 开启配置下发 在线业务                      |
#------------------------------------------------------*
def conf_online_down_busi():
    print "-------------------- Begin conf online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin conf CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_down_filelist.xml'
    uuidtmp = uuid+'/'+comm_data.CONF_FILE

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                continue

        # 2. 将得到的xml文件存在本地
        try :
            write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            comm_cloud_logger.warn("[%s]"%ex)
            continue
        # -->  配置文件 下载事件
        try:
            conf_fname_list = get_cloud_xml_fname(tmp_file, comm_data.CONF_FILE)
        except Exception,ex:
            os.remove(tmp_file)
            comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            continue
        cloud_down_file(oss, bucket, conf_fname_list, comm_data.comm_conf_down_path)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

        # --> 白名单 不需要进行从阿里云下载， 而是需要解析顶级设备目录
        dir_list = ((comm_data.whitelist_src_path, comm_data.whitelist_dst_path),)
        try:
           conf_down_file_parser(dir_list)
        except Exception:
            print("**** ERROR ****")
            comm_cloud_logger.debug("**** ERROR ****")
            print(traceback.format_exc())
            comm_cloud_logger.debug("[%s]" %(traceback.format_exc()))

        time.sleep(2)
#-----------------------------------------.
# @brief 设备状态在线上行处理业?|
#-----------------------------------------*
def online_up_busi(comm_ali_path,comm_file):
    global uuid , totalvds ,g_compare_time1
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    BUCKETNEW = comm_data.OSS_UP_BUCKET_NEW
     
    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_ali_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_file, os.path.basename(up_file))
            if comm_ali_path == comm_data.comm_ve_ali_path :
                compare_time = time.strftime('%Y%m%d')
                if g_compare_time1 == compare_time:
                    totalvds = int(totalvds) + 1
                else:
                    totalvds = 0 
                retn = cloud_up_file(oss, BUCKETNEW, up_file, dst_file)
            else:
                retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#-----------------------------------------.
# @brief 限速模块设备状态在线上行处理业务 |
#-----------------------------------------*
def online_up_limit_busi(comm_ali_path,comm_file):
    global uuid ,totalvds,g_compare_time1
    global up_lim,down_lim
    print '*!!!!!!!!!!! %s , %s !!!!!!!!!!'%(up_lim,down_lim)
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    BUCKETNEW = comm_data.OSS_UP_BUCKET_NEW
    
    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_ali_path], ['.ok']):
            dst_file = os.path.join(str(uuid), comm_file, os.path.basename(up_file))
            if comm_ali_path == comm_data.comm_ve_ali_path :
                compare_time = time.strftime('%Y%m%d')
                if g_compare_time1 == compare_time:
                    totalvds = int(totalvds) + 1
                else:
                    totalvds = 0 
                retn = cloud_up_file_limit(oss, BUCKETNEW,up_file,dst_file,up_lim)
            else:
                retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#----------------------------------
# @ bref topo 正常上传模块	      |
#----------------------------------
def topo_online_up_busi():
    print "-------------------- Begin topo online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin topo online UP BUSINESS -------------")
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_topo_up_src_path], ['.xml']):
            dst_file = os.path.join(str(uuid), comm_data.TOPO_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
            else:
                oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
          
        time.sleep(2)

#---------------------------------
# @ bref topo 限速上传模块		|
#--------------------------------
def topo_online_up_limit_busi():
    print "---------------- Begin topo Limit Speed Model online UP BUSINESS -------------"
    comm_cloud_logger.debug("---------------- Begin topo Limit Speed Model online UP BUSINESS ----------")
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_topo_up_src_path], ['.xml']):
            dst_file = os.path.join(str(uuid), comm_data.TOPO_FILE, os.path.basename(up_file))
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                os.remove(up_file)
            else:
                oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        time.sleep(2)
#---------------------------------------
# @bref 配置，规则上传模块             |
#---------------------------------------
def conf_online_up_busi():
    print "-------------------- Begin CONF online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin CONF online UP BUSINESS -------------")
    comm_ali_path = comm_data.comm_devstat_ali_path
    comm_file = comm_data.STAT_FILE
    if comm_data.stat_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_busi(comm_ali_path,comm_file)
#---------------------------------------
# @bref 限速模块配置，规则上传         |
#---------------------------------------
def conf_online_up_limit_busi():
    print "-------------------- Begin Limit Speed Model CONF online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------- Begin Limit Speed Model CONF online UP BUSINESS --------")
    comm_ali_path = comm_data.comm_devstat_ali_path
    comm_file = comm_data.STAT_FILE
    if comm_data.stat_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_limit_busi(comm_ali_path,comm_file)
        
#-------------------------------------
# @bref abb 正常模式上传             |
#------------------------------------
def abb_online_up_busi():
    print "-------------------- Begin ABB online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin ABB online UP BUSINESS -------------")
    comm_ali_path = comm_data.comm_ae_ali_path
    comm_file = comm_data.ABB_FILE
    if comm_data.abb_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_busi(comm_ali_path,comm_file)
#--------------------------------
# @bref abb 限速在线上传模式			|
#--------------------------------
def abb_online_up_limit_busi():
    print "-------------------- Begin Limit Speed Model ABB online UP BUSINESS -------------"
    comm_cloud_logger.debug("------------------ Begin Limit Speed Model ABB online UP BUSINESS -------------")
    comm_ali_path = comm_data.comm_ae_ali_path
    comm_file = comm_data.ABB_FILE
    if comm_data.abb_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_limit_busi(comm_ali_path,comm_file)
#-----------------------------------
# @bref vds 正常上传模式			|
#-----------------------------------
def virus_online_up_busi():
    print "-------------------- Begin VIRUS online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin VIRUS online UP BUSINESS -------------")
    global totalvds , g_compare_time1
    comm_ali_path = comm_data.comm_ve_ali_path
    comm_file = comm_data.VIRUS_FILE
    g_compare_time1 = time.strftime('%Y%m%d')

    if comm_data.virus_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_busi(comm_ali_path,comm_file)

#-------------------------------------
# @bref vds 限速上传模式			 |
#------------------------------------
def virus_online_up_limit_busi():
    print "----------------- Begin Limit Speed Model VIRUS online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------Begin Limit Speed Model VIRUS online UP BUSINESS --------")
    comm_ali_path = comm_data.comm_ve_ali_path
    comm_file = comm_data.VIRUS_FILE
    if comm_data.virus_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_limit_busi(comm_ali_path,comm_file)
#---------------------------------------
# @ bref surl 在线上传模式				|
#---------------------------------------
def surl_online_up_busi():
    print "--------------- Begin  SURL online UP BUSINESS -------------"
    comm_cloud_logger.debug("------------- Begin  SURL online UP BUSINESS -------------")
    comm_ali_path = comm_data.comm_surl_ali_path
    comm_file = comm_data.SURL_FILE
    if comm_data.surl_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_busi(comm_ali_path,comm_file)
#---------------------------------------
#surl 限速在线上传模式					|
#---------------------------------------
def surl_online_up_limit_busi():
    print "-------------------- Begin Limit Speed Model SURL online UP BUSINESS -------------"
    comm_cloud_logger.debug("---------------- Begin Limit Speed Model SURL online UP BUSINESS ---------")
    comm_ali_path = comm_data.comm_surl_ali_path
    comm_file = comm_data.SURL_FILE
    if comm_data.surl_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_limit_busi(comm_ali_path,comm_file)
#--------------------------------------.
#sysflow 正常在线模式					|
#--------------------------------------
def sysflow_online_up_busi():
    print "-------------------- Begin SYSFLOW online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin FLOW online UP BUSINESS -------------")
    comm_ali_path = comm_data.comm_flow_ali_path
    comm_file = comm_data.FLOW_FILE
    if comm_data.flow_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_busi(comm_ali_path,comm_file) 
#--------------------------------------.
#sysflow 限速在线上传模式			   |
#--------------------------------------.
def sysflow_online_up_limit_busi():
    print "-------------- Begin SYSFLOW Limit Speed Model online UP BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin FLOW Limit Speed Model online UP BUSINESS --------")
    comm_ali_path = comm_data.comm_flow_ali_path
    comm_file = comm_data.FLOW_FILE
    if comm_data.flow_isupload == 'no':
        delete_event_busi(comm_ali_path)
    else:
        online_up_limit_busi(comm_ali_path,comm_file) 

#------------------------------------------------------------
#  @bref 往record_uuid下放入uuid_time_mtx_status_uptime信息 |
#------------------------------------------------------------
def uptime():
    p = subprocess.Popen(['/gms/gapi/modules/proxy/cloud/GenStat.sh'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    stdout , stderr = p.communicate()
    return stdout.split()[3].split(':')[1]

def engines_status(engine_name):
    def check_if_tcad_or_apt():
        tree = parse.read_xml(gmsconfig)
        nodes = parse.find_nodes(tree, "flow_engine/flow_engine_name")
        for node in nodes:
            return node.text

    def juge_engnie_status(engine_name):
        p = subprocess.Popen(["pidof %s" %(engine_name)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
        out = p.stdout.readlines()
        if out == []:
            return 0
        else:
            return 1
        
    if engine_name == "vds":
        detect_dd = juge_engnie_status('dd') 
        detect_dm = juge_engnie_status('dm') 
        detect_apc = juge_engnie_status('apc') 
        if detect_dd == 1 and detect_dm == 1 and detect_apc == 1:
            return 1
        else:
            return 0
    elif engine_name == "mtx": 
        return (juge_engnie_status(engine_name))
    else:
        ret_engine = check_if_tcad_or_apt()
        if ret_engine == 'apt':
            return (juge_engnie_status(ret_engine))
        if ret_engine == 'tcad':
            return (juge_engnie_status('tcad_gms'))
        if ret_engine == None:
            return (juge_engnie_status('apt'))

def version():
    tree = ElementTree.parse("/gms/conf/version.xml")    
    tags = tree.findall("SysVersion")
    for node in tags:
        version = node.text
    return version

def get_ip():
    tree = ElementTree.parse("/gms/conf/gmsconfig.xml")
    nodes = tree.findall("gms_device/attr/")
    for node in nodes:
        ip = node.attrib['ip'] 
    return ip

def get_disk():
    p = subprocess.Popen(['/gms/gapi/modules/proxy/cloud/GenStat.sh'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    stdout , stderr = p.communicate()
    return stdout.split()[0].split(':')[1]
def get_cpu():
    p = subprocess.Popen(['/gms/gapi/modules/proxy/cloud/GenStat.sh'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    stdout , stderr = p.communicate()
    return stdout.split()[1].split(':')[1]
def get_mem():
    p = subprocess.Popen(['/gms/gapi/modules/proxy/cloud/GenStat.sh'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    stdout , stderr = p.communicate()
    return stdout.split()[2].split(':')[1]

#def rule_version():
    #p=subprocess.Popen(['cat /gms/mtx/conf/rule.conf |grep version'],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    #stdout , stderr = p.communicate()
    #return stdout.split(':')[1].strip()
    #Rule_Path = '/gms/mtx/conf/rule.conf'
def rule_version():
    import re
    with open('/gms/mtx/conf/rule.conf','r') as fp:
        datas = fp.read()

    r = re.compile(r'^#[a-zA-Z]+:\d+')
    ret = r.findall(datas)
    return ret[0].split(':')[1]


def dev_name():
    tree = parse.read_xml(gmsconfig)                                                                                     
    subnode = parse.find_nodes(tree,"gms_device/attr")                                                                     
    for node in subnode:                                                                                                 
        dev_name = node.attrib['name'].encode('utf8')
    return dev_name

def machine_type():
    path_type = '/cfcard/chanct_conf/machine_type.conf'
    with open(path_type,'r') as fp:
        data = fp.read()
    return data.strip()


def make_uuid_bucket_tag_name():
    global uuid,totalmtx
    print '%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s'%(uuid,dev_name(),machine_type(),uptime(),str(engines_status("mtx")),str(engines_status("apt")),str(engines_status("vds")),version(),str(get_ip()),get_disk(),get_cpu(),get_mem(),rule_version(),totalmtx,totalvds,time.time())
    return '%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s_%s'%(uuid,dev_name(),machine_type(),uptime(),str(engines_status("mtx")),str(engines_status("apt")),str(engines_status("vds")),version(),str(get_ip()),get_disk(),get_cpu(),get_mem(),rule_version(),totalmtx,totalvds,time.time())

def create_uuid_bucket_tag():
    global uuid_bucket_tag,uuid
    try:
        uuid_bucket_tag = make_uuid_bucket_tag_name()
        #print '+++++++ %s ++++++++++++' %(uuid_bucket_tag)
        oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        res = oss.put_object_from_string(comm_data.OSS_UUID_BUCKET,uuid_bucket_tag, "record")
        res.read()
        #print '++++++++ %s +++++++++' %res.status
        if res.status in [200, 204]:
            return True
        else:
            return False
    except Exception,ex:
        comm_cloud_logger.debug("%s" %(traceback.format_exc()))                        
        print '-------------------------',ex
        return False

def delete_uuid_bucket_tag():
    global uuid_bucket_tag
    try:
        oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        res = oss.delete_object(comm_data.OSS_UUID_BUCKET,uuid_bucket_tag)
        res.read()
        if res.status in [200, 204]:
            return True
        else:
            return False
    except:
        comm_cloud_logger.debug("%s" %(traceback.format_exc()))                        
        return False

def refresh_uuid_bucket_tag():
    if delete_uuid_bucket_tag():
        return create_uuid_bucket_tag()
    else:
        return False

def refresh():
    print "-------------- Begin DEVICE_MESSAGE online UP BUSINESS -------------"                              
    comm_cloud_logger.debug("--------- Begin DEVICE_MESSAGE online UP BUSINESS --------")                        

    global refresh_sec , timeout
    if create_uuid_bucket_tag():
        comm_cloud_logger.debug("add uuid succeed ." )                        
    else:
        comm_cloud_logger.warn("add uuid fail ." )                        

    while True:
        print_flag = False
        try:
            refresh_sec = (refresh_sec + 1) % REFRESH_SEC
            if refresh_sec == 0:
                if refresh_uuid_bucket_tag():
                    print('refresh uuid [%s] success.' % uuid)
                    print_flag = True

            if print_flag:
                comm_cloud_logger.debug("[%s] Update device status [%s] succeed." %(comm_data.OSS_UUID_BUCKET,uuid_bucket_tag))                        
        except Exception,ex:
            comm_cloud_logger.warn("%s" %(ex))                        
            continue
        time.sleep(1)

def clear_bucket_devstatus():
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UUID_BUCKET
    try:
        res = oss.get_bucket(bucket)                                         
        buf = res.read()                                                     
        dom = xml.dom.minidom.parseString(buf)                                              
        root = dom.documentElement                                           
        reg_list = root.getElementsByTagName('Contents')                     
        for file_info in reg_list:                                           
            keynode = file_info.getElementsByTagName("Key")[0]               
            fname = keynode.childNodes[0].nodeValue                          
            if fname.split()[0].split('_')[0] == uuid:
                try:
                    res = oss.delete_object(comm_data.OSS_UUID_BUCKET,fname)
                    res.read()
                except Exception,ex:
                    print ex
                    comm_cloud_logger.debug("clear up device status [%s] fail." %(fanme))                        
                if res in [200 ,204]:
                    print 'clear devid status succeed .'
                    comm_cloud_logger.debug("clear up device status [%s] succeed." %(fanme))                        
    except Exception,ex:
        comm_cloud_logger.debug("%s" %(ex))
        pass

def xml2dict(xmlfile):
    xmldict =None
    with file(xmlfile) as fp:
        xmldata = fp.read()
        xmldict = xmltodict.parse(xmldata)
    return xmldict

def config_reload_thread():
    comm_cloud_logger.debug("==== config reload thread start ====")
    xmlfile = comm_init.interface_conf_file
    xmldict_org = xml2dict(xmlfile)
    # 检查配置文件是否更新。如果更新，退出程序
    while True:
        try:
            xmldict_cur = xml2dict(xmlfile)
            if xmldict_cur != xmldict_org:
                comm_cloud_logger.debug("==== config file was modified. restart. ====")
                os.kill(os.getpid(), signal.SIGKILL)
        except Exception:
            comm_cloud.logger.info(traceback.format_exc())
        finally:
            time.sleep(1)

def host_cloud_log_init():
    global comm_cloud_logger
    comm_cloud_logger = cloud_log.CloudLogger(path = comm_data.cloud_log_path,level = '',format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')

#各种事件开始线程处理
#def try_catch_busi(event_busi,t_name):
#    try:
#        thread_list.append(threading.Thread(target=event_busi,name=t_name,args=()))
#        thread.setDaemon(True)
#        thread.start()
#    except Exception, ex:
#        print ex
#        comm_cloud_logger.debug("[%s]" %ex)

def join_thread(t_name):
    thread.join(t_name)

def judge_limit_conf(uplimit,downlimit):
    if uplimit == '-1' and downlimit == '-1':
        uplimit = '0'
        downlimit = '0'
        proxy_limit_xml(uplimit,downlimit) 
        return 0
    elif uplimit == '0' and downlimit == '0':
        proxy_limit_xml(uplimit,downlimit) 
        return 0
    else:
        proxy_limit_xml(uplimit,downlimit) 
        return (uplimit,downlimit) 

#记录proxy_limit信息到xml中
def proxy_limit_xml(uplimit,downlimit):
    tree = parse.read_xml(gmsconfig)
    tnode = parse.find_nodes(tree,"gmsconf")
    nodes = parse.find_nodes(tree,"gms_limit")
    #如果没有<gms_limit>节点，则创建。
    if not nodes:
        newnode = parse.create_node("gms_limit",{"uplimit":uplimit,"downlimit":downlimit},"")  
        parse.add_child_node(tnode,newnode)
    else:
    #如果有相应的节点，则修改它的属性。
        parse.change_node_properties(nodes,{"uplimit":uplimit,"downlimit":downlimit},"")
    parse.write_xml(tree,gmsconfig)    
    comm_cloud_logger.debug("write uplimit %s and downlimit %s into %s"%(uplimit,downlimit,gmsconfig))

def init_read_mtx_count():
    global totalmtx ,totalvds
    def read_conf(file):
        try:
            if not os.path.exists(file):
                totalcount = 0 
            else:
                fp = open(file,'r')
                totalcount = fp.read().strip()
                fp.close()
                if totalcount == '':
                    totalcount = 0
            return totalcount
        except Exception,ex:
            comm_cloud_logger.debug("%s" %(ex))

    totalmtx = read_conf('./conf/mtxcount.txt')
    totalvds = read_conf('./conf/vdscount.txt')

def read_link_status():
    from xml.etree.ElementTree import ElementTree,Element
    root = ElementTree()
    tree = root.parse('/gms/conf/gmsconfig.xml')

    ret = map(lambda x:elem.attrib  , [elem for elem in tree.findall('comm_stat')])
    if ret[0].has_key('link'):
        return ret[0]['link']
    return False


def create_thread(fun,name,args=()):
    retThread = threading.Thread(target=fun,name=name ,args=())
    retThread.setDaemon(True)
    retThread.start()
    thread_list.append(retThread)
    time.sleep(0.1)

def offline_busi():
    # 离线业务模式
    comm_cloud_logger.debug("Beacuse  cloudcon == 0,so limit speed model is not aviliable,xml not opration")
    print "Begin offline busi!"
    comm_cloud_logger.debug("Begin offline busi!")

    cloudid_topo.load_topo_dist()
    if (comm_data.intranet_mode == "1"):
        comm_cloud_logger.debug("intranet mode, do not start offline busi...")
        return 

    switch_offline_busi = {
        'offline_reg_up': reg_offline_up_busi,
        'offline_reg_down': reg_offline_down_busi,
        'offline_reg_auto': reg_offline_autoreg_busi,
        'offline_delete_event': offline_delete_event_busi,
        'offline_mtx_up': mtx_offline_up_busi,
        'offline_mtx_down': mtx_offline_down_busi,
        'offline_conf_down': conf_offline_down_busi 
    } 
    
    for key , value in switch_offline_busi.items():
        create_thread(switch_offline_busi.get(key),name=value,args=())

def online_busi():
    global up_lim,down_lim
    # 在线模式且不限速 
    clear_bucket_devstatus() #设备启动时先清空云上之前的设备状态

    if judge_limit_conf(uplimit,downlimit) == 0:
        print "Begin Online Busi!"
        comm_cloud_logger.debug("Begin Online Busi!")

        switch_online_busi = {
            'reg_up': reg_online_up_busi,
            'reg_down': reg_online_down_busi,
            'refresh': refresh,
            'mtx_up': mtx_online_up_busi,
            'mtx_down': mtx_online_down_busi,
            'query_down': query_online_down_busi,
            'query_up': query_online_up_busi,
            'vds_rule': rule_vds_online_down_busi,
            'apt_rule': rule_apt_online_down_busi,
            'update_down': update_online_down_busi,
            'surl_rule': rule_surl_online_down_busi,
            'conf_down': conf_online_down_busi,
            'conf_up': conf_online_up_busi,
            'abb_up': abb_online_up_busi,
            'vds_up': virus_online_up_busi,
            'surl_up': surl_online_up_busi,
            'sysflow_up': sysflow_online_up_busi,
            'topo_up': topo_online_up_busi,
            'report_up': report_online_up_busi,
            'report_down': report_online_down_busi
        }
        for key , value in switch_online_busi.items():
            create_thread(switch_online_busi.get(key),name=value,args=())
    else:
        (up_lim , down_lim) = judge_limit_conf(uplimit,downlimit)
        up_lim = int(up_lim)/8 * 1000
        down_lim = int(down_lim)/8 * 1000
        comm_cloud_logger.debug("up_lim is %s ,down_lim is %s"%(up_lim,down_lim))

        clear_bucket_devstatus() #设备启动时先清空云上之前的设备状态

        switch_limit_busi = {
            'reg_up': reg_online_up_limit_busi,
            'reg_down': reg_online_down_limit_busi,
            'refresh': refresh,
            'mtx_up': mtx_online_up_limit_busi,
            'mtx_down': mtx_online_down_limit_busi,
            'query_down': query_online_down_limit_busi,
            'query_up': query_online_up_limit_busi,
            'vds_rule': rule_vds_online_down_limit_busi,
            'apt_rule': rule_apt_online_down_limit_busi,
            'update_down': update_online_down_limit_busi,
            'surl_rule': rule_surl_online_down_limit_busi,
            'conf_down': conf_online_down_limit_busi,
            'conf_up': conf_online_up_limit_busi,
            'abb_up': abb_online_up_limit_busi,
            'vds_up': virus_online_up_limit_busi,
            'surl_up': surl_online_up_limit_busi,
            'sysflow_up': sysflow_online_up_limit_busi,
            'topo_up': topo_online_up_limit_busi,
            'report_up': report_online_up_limit_busi,
            'report_down': report_online_down_limit_busi
        }

        for key , value in switch_limit_busi.items():
            create_thread(switch_limit_busi.get(key),name=value,args=())

def main():
    global comm_cloud_logger,up_lim,down_lim,uplimit,downlimit
    opts,args = getopt.getopt(sys.argv[1:],"h:l:","") 
    for opt,arg in opts:
        if opt == '-h':
            uplimit = arg 
        if opt == '-l':
            downlimit = arg
    # 读取目录信息
    comm_init.read_pre_json()

    check_DNS(eval(comm_data.backup_dns))
    #初始化日志
    host_cloud_log_init()
    # 读取配置文件
    read_interface_xml()
    # 创建目录
    create_dir()
    # init mtx count
    init_read_mtx_count()
    # 读取层级设备间的状态
    #read_link_status()
    create_thread(config_reload_thread,name='config_reload_thread',args=())

    if (ismonitor == "1"):   #监测点,单点或层级关系,层级关系若与上层管理点断连了,则直接连接阿里云
        if (isroot == "1" and cloudcon == "1"):    #单点关系,与云交互,监测点接通阿里云,在线模式
            online_busi()
        elif (isroot == "1" and cloudcon == "0"):  #单点关系,与云交互,监测点离线模式
            offline_busi()
        elif (isroot == "0"):                      #层级关系,下层监测点,走的是通讯
            if (read_link_status() == "0"):        #此部分不做任何处理
                comm_cloud_logger.debug(" ** From Nolink status switch to link Aliyun . ** ")
                online_busi()
            if (read_link_status() == "1"):
                pass

    if (ismanager == "1"):   #管理点,判断是否设备间断开,与此部分没关系
        if (isroot == "1" and cloudcon == "1"):
            online_busi() 
        elif (isroot == "1" and cloudcon == "0"):
            offline_busi()
        elif (isroot == "0"):
            if (read_link_status() == "0"):
                comm_cloud_logger.debug(" ** From Nolink status switch to link Aliyun . ** ")
                online_busi()
            if (read_link_status() == "1"):
                pass

    for thread in thread_list:
        thread.join()
    
if __name__ == "__main__":
    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)
    signal.signal(signal.SIGTERM, sig_exit)
    reload(sys)
    sys.setdefaultencoding('utf-8')
    main()
    while (True):
        time.sleep(3)

###################### file end ####################
