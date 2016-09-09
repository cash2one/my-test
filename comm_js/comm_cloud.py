#-*- coding:utf-8 -*-


from xml.etree import ElementTree
import os
import sys
import time
import threading
import signal
import xml.dom.minidom
import print_color
import cloud_log
import struct 
import socket
import traceback

sys.path.append("../")
import file_up_down_tran
import comm_data
import comm_common
import comm_init
import xmltodict
import cloudid_topo
import json
from oss.oss_api import *
from oss.osscloudapi import *



interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
uuid = 0
cloudcon = "0"
comm_cloud_logger=None
retn = -2 


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
    print("recv signal[%d]. exit." %sig)
    comm_cloud_logger.debug("recv signal[%d]. exit." %sig)
    exit()

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
        if (dir_names[1] == prefix_path):
            list.append(keynode.childNodes[0].nodeValue)
            sizenode = file_info.getElementsByTagName("Size")[0]
            list1.append(sizenode.childNodes[0].nodeValue)
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
        dir_name = dir_names[1]+"/"+dir_names[2]
        if (dir_name == prefix_path):
            list.append(keynode.childNodes[0].nodeValue)
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
        dir_name = dir_names[1]+"/"+dir_names[2]
        if (dir_name == prefix_path):
            list.append(keynode.childNodes[0].nodeValue)
            sizenode = file_info.getElementsByTagName("Size")[0]
            list1.append(sizenode.childNodes[0].nodeValue)
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
        dir_name_list=dir_names[1]+'/'+dir_names[2]+'/'+dir_names[3]+'/'+dir_names[4]
        #print '[dir_name++++++++++++++++++++%s]'%dir_name_list
        if (dir_name_list == prefix_path and dir_names[-1] != ""):
            list.append(keynode.childNodes[0].nodeValue)
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
        dir_name_list = dir_names[1]+'/'+dir_names[2]+'/'+dir_names[3]+'/'+dir_names[4]
        if (dir_name_list == prefix_path and dir_names[-1] != ""):
            list.append(keynode.childNodes[0].nodeValue)
            sizenode = file_info.getElementsByTagName("Size")[0]
            list1.append(sizenode.childNodes[0].nodeValue)
    return list,list1

def read_interface_xml():
    global interface_conf_file
    global uuid
    global cloudcon
    global comm_cloud_logger
    root = ElementTree.parse(interface_conf_file)
    lst_node = root.getiterator("cominfo")

    devtype_node = root.find("selfinfo/uuid")
    uuid = devtype_node.text
    print "local uuid is %s" %(uuid)
    comm_cloud_logger.debug("local uuid is %s" %(uuid))
    cloud_node = root.find("selfinfo/devtype/cloudcon")
    cloudcon = cloud_node.text
    print "cloudcon is %s" %(cloudcon)
    comm_cloud_logger.debug("cloudcon is %s" %(cloudcon))


#-----------------------------------------.
# @brief 向云端上传本地文件               |
# @param oss                远端连接句柄  |
# @param bucket             云端数据集合  |
# @param local_fname        文件目录      |
# @param remote_fname,      后缀          |
#-----------------------------------------*
def cloud_up_file(oss, bucket, local_fname, remote_fname):
    t = time.time()
    try:
        res = oss.put_object_from_file(bucket, remote_fname, local_fname)
    except Exception, ex:
        print ex
        comm_cloud_logger.debug("[%s]" %ex)
        return -1
    if res.status != 200:
        print_color.print_upload(local_fname, remote_fname, "FAIL")
        comm_cloud_logger.debug("UP FILE[%s] -> CLOUD[%s] FAIL" %(local_fname,remote_fname))
        print res.read()
        comm_cloud_logger.debug("[%s]" %(res.read()))
        retn = -1
    else:
        print_color.print_upload(local_fname, remote_fname, "SUCC")
        comm_cloud_logger.debug("UP FILE[%s] -> CLOUD[%s] SUCC" %(local_fname,remote_fname))
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
    print 'limit_speed [%s]'%limit_speed
    t = time.time()
    try:
        res = oss.put_object_from_file2(bucket,remote_fname,local_fname,limit_speed)
    except Exception, ex:
        print ex
        comm_cloud_logger.debug("[%s]" %ex)
        return -1
    if res.status != 200:
        print_color.print_upload(local_fname, remote_fname, "FAIL")
        comm_cloud_logger.debug("Lspeed[%s] Limit Speed Model UP FILE[%s] -> Limit Speed Model CLOUD[%s] FAIL" %(limit_speed,local_fname,remote_fname))
        print res.read()
        comm_cloud_logger.debug("[%s]" %(res.read()))
        retn = -1
    else:
        print_color.print_upload(local_fname, remote_fname, "SUCC")
        comm_cloud_logger.debug("Lspeed[%s] LIMIT Speed Model UP FILE[%s] -> LIMIT Speed Model CLOUD[%s] SUCC" %(limit_speed,local_fname,remote_fname))
        print "-->spend [%f] second(s). file size[%d]." %(time.time()-t, os.path.getsize(local_fname))
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
    except Exception, ex:
        print(traceback.format_exc())
        print ex
        #comm_cloud_logger.debug("[%s]" %ex)
        return -1
    if res.status != 200:
        print_color.print_upload(cloud_name, "FAIL")
        print '!!!!!!creat cloud dir fail!!!!!'
        comm_cloud_logger.debug("CREATE_ALI_DIR [%s]FAIL" %(cloud_name))
        retn = -1
    else:
        #print '******creat cloud dir [%s]succeed*********'%(cloud_name)
        #comm_cloud_logger.debug("#####CREAT_ALI_DIR [%s]SUCC#####" %(cloud_name))
        retn = 0
    return retn
#-----------------------------------------.
# @brief 向云端下载传本地文件             |
# @param oss                远端连接句柄  |
# @param bucket             云端数据集合  |
# @param flist              文件列        |
# @param path               本地目标目录  |
#-----------------------------------------*
def cloud_down_file(oss, bucket, flist, path):
    for fname in flist:
        down_fname = ''
        final_name = path + os.path.basename(fname)
        down_fname = final_name + ".tmp"
        t = time.time()
        try:
            print_color.print_downloading(fname, down_fname)
            comm_cloud_logger.debug("CLOUD[%s] -> FILE[%s]..." %(fname,down_fname))
            res = oss.get_object_to_file(bucket, fname, down_fname)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
            continue
        if res.status != 200:
            print_color.print_download(fname, final_name, "FAIL")
            comm_cloud_logger.debug("CLOUD[%s] ->FILE[%s]FILE" %(fname,final_name))
            print res.read()
            comm_cloud_logger.debug("[%s]" %(res.read()))
            break
        else:
            print_color.print_download(fname, final_name, "SUCC")
            comm_cloud_logger.debug("CLOUD[%s] ->FILE[%s]SUCC" %(fname,final_name))
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
                comm_cloud_logger.debug("CLOUD[%s] FIAL  " %(fname))
                print res.read()
                comm_cloud_logger.debug("[%s]" %(res.read()))
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
    count = 0
    #print 'down_lim[%s]'%down_lim
    #print fname_list
    for fname in fname_list: 
        for fsize in fname_size:
            if count == 0:
                #print 'fname:[%s],fname_size:[%s]'%(fname,fsize)
                down_fname = ''
                final_name = path + os.path.basename(fname)
                down_fname = final_name + ".tmp"
                t = time.time()
                try:
                    print_color.print_downloading(fname, down_fname)
                    comm_cloud_logger.debug("Lspeed[%s] (Limit Speed Model) CLOUD[%s] -> FILE[%s]..." %(down_lim,fname,down_fname))
                    #res = oss.get_object_to_file(bucket, fname, down_fname)
                    #print 'Will go to get_object_to_file2.......'
                    res = oss.get_object_to_file2(bucket,fname,int(fsize),down_fname,down_lim)
                    print 'oss.get_object_to_file2 end.........'
                    print '-----res.status = %s------'%res.status
                except Exception, ex:
                    print ex
                    comm_cloud_logger.debug("[%s]" %ex)
                    continue
                if res.status != 206:
                    print_color.print_download(fname, final_name, "FAIL")
                    comm_cloud_logger.debug("Lspeed[%s] (Limit Speed Model) CLOUD[%s] -> FILE[%s]FILE" %(down_lim,fname,final_name))
                    print res.read()
                    comm_cloud_logger.debug("[%s]" %(res.read()))
                    break
                else:
                    print_color.print_download(fname, final_name, "SUCC")
                    comm_cloud_logger.debug("Lspeed[%s] (Limit Speed Model) CLOUD[%s] ->FILE[%s]SUCC" %(down_lim,fname,final_name))
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
    import shutil
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
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.REG_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)

        time.sleep(0.5)
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
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.REG_FILE, os.path.basename(up_file))
            #retn = cloud_up_file(oss, bucket, up_file, dst_file)
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)

        time.sleep(0.5)

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

    while True:

        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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

        # --> reg 注册文件下载
        reg_fname_list,reg_fname_size=get_cloud_xml_fname_limit(tmp_file, comm_data.REG_FILE)
        cloud_down_file_limit(oss,bucket,reg_fname_list,reg_fname_size,comm_data.comm_reg_down_path,down_lim)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

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

    while True:

        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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

        # --> reg 注册文件下载
        reg_fname_list = get_cloud_xml_fname(tmp_file, comm_data.REG_FILE)
        cloud_down_file(oss, bucket, reg_fname_list, comm_data.comm_reg_down_path)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

#-----------------------------------------.
# @brief 限速模式设备注册在线下行处理业务 |
#-----------------------------------------*
def reg_online_down_limit_busi():
    print "------------- Begin Limit Speed Model REG online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("--------- Begin Limit Speed Model REG online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_reg_down_limit_filelist.xml'

    while True:

        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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

        # --> reg 注册文件下载
        reg_fname_list ,reg_fname_size= get_cloud_xml_fname_limit(tmp_file, comm_data.REG_FILE)
        #cloud_down_file_limit(oss, bucket, reg_fname_list, comm_data.comm_reg_down_path)
        cloud_down_file_limit(oss,bucket,reg_fname_list,reg_fname_size,comm_data.comm_reg_down_path,down_lim)

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
    print 'OssAPI2() .......'
    bucket = comm_data.OSS_UP_BUCKET

    while True:

        # --> mtx 上传事件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_data.comm_me_ali_up_path], ['.ok']):
            print up_file
            #dst_file = str(uuid) + comm_data.MTX_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.MTX_FILE, os.path.basename(up_file))
            #retn = cloud_up_file(oss, bucket, up_file, dst_file)
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if retn == 0:
                #上传成功 删除本地文件
                os.remove(up_file)

        time.sleep(0.5)
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
            #dst_file = str(uuid) + comm_data.MTX_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.MTX_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            #retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if retn == 0:
                #上传成功 删除本地文件
                os.remove(up_file)

        time.sleep(0.5)

#-----------------------------------------.
# @brief REPORT 在线上传处理业务          |
#-----------------------------------------
def report_online_up_busi():
    print "-------------------- Begin REPORT online UP BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin REPORT online UP BUSINESS -------------")
    import shutil
    global uuid
    global retn
    

    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
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
                            cloud_dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                            #上传成功,拷贝一份到/data/tmpdata/comm/report/
                            retn = cloud_up_file(oss, bucket, up_file, cloud_dst_file)
                            if retn == 0:
                                #print '@@@@@@@[%s]@@@@@@@'%dst
                                other_dst_file = os.path.join(dst,os.path.basename(up_file))
                                shutil.copy(up_file,other_dst_file) 
                                print_color.print_copy(up_file,other_dst_file)
                                comm_cloud_logger.debug("==REPORT CP [%s]--->[%s]======" %(up_file,other_dst_file))
                                #删除源文件
                                os.remove(up_file)
                                print_color.print_delete_local(up_file)
        time.sleep(0.5)
#-----------------------------------------.
# @brief REPORT 限速-在线上传处理业务     |
#-----------------------------------------
def report_online_up_limit_busi():
    print "-------------------- Begin REPORT Limit Speed Model online UP BUSINESS -------------"
    comm_cloud_logger.debug("----------- Begin REPORT Limit Speed Model online UP BUSINESS -------")
    import shutil
    global uuid

    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    #print bucket
    #print 'in report up_lim %s'%up_lim 
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
        time.sleep(0.5)

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

    while True:
    # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
            else:
                print 'creat dir [%s] fail!!!!' %dst_file
                print_color.print_creat(dst_file)
                comm_cloud_logger.debug("---------CREAT REPORT CLOUD DIR[%s] FAIL!!--------"%dst_file)
            report_fname_list = get_cloud_report_xml_fname(tmp_file, src_dir) #返回桶内的列表
            cloud_down_file(oss, bucket, report_fname_list,dst_dir) #根据列表下载对应的列表

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)
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

    while True:
    # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
            fname_list,fname_size = get_cloud_report_xml_fname_limit(tmp_file, src_dir) #返回桶内的列表
            cloud_down_file_limit(oss,bucket,fname_list,fname_size,dst_dir,down_lim) #根据列表下载对应的列表

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

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
            #dst_file = str(uuid) + comm_data.MTX_FILE + os.path.basename(up_file)
                dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                retn = cloud_up_file(oss, bucket, up_file, dst_file)
                if retn == 0:
                    #上传成功 删除本地文件
                    os.remove(up_file)

        time.sleep(0.5)
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
            #dst_file = str(uuid) + comm_data.MTX_FILE + os.path.basename(up_file)
                dst_file = os.path.join(str(uuid), dst_dir, os.path.basename(up_file))
                #retn = cloud_up_file(oss, bucket, up_file, dst_file)
                retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
                if retn == 0:
                    #上传成功 删除本地文件
                    os.remove(up_file)

        time.sleep(0.5)

#-----------------------------------------.
# @brief MTX在 在线下传处理业务           |
#-----------------------------------------*
def mtx_online_down_busi():
    print "-------------------- Begin MTX online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin MTX online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_mtx_down_filelist.xml'

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
        mtx_fname_list = get_cloud_xml_fname(tmp_file, comm_data.MTX_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_me_ali_down_path)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

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

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
        mtx_fname_list,mtx_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.MTX_FILE)
        #cloud_down_file_limit(oss, bucket, mtx_fname_list, comm_data.comm_me_ali_down_path)
        cloud_down_file_limit(oss,bucket,mtx_fname_list,mtx_fname_size,comm_data.comm_me_ali_down_path,down_lim)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)


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
        time.sleep(0.5)
    
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
        time.sleep(0.5)
    
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

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_down_path , query_flow_src_path),     
                    (comm_data.comm_query_url_down_path, query_url_src_path),       
                    (comm_data.comm_query_dns_down_path, query_dns_src_path),    
                    )
        # --> query 下载事件
        for dst_dir,src_dir in dir_list:
            query_fname_list = get_cloud_query_xml_fname(tmp_file, src_dir)
            cloud_down_file(oss, bucket, query_fname_list, dst_dir)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)
    
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

    while True:
        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
        query_flow_src_path = comm_data.QUERY_FLOW_FILE
        query_url_src_path = comm_data.QUERY_URL_FILE 
        query_dns_src_path = comm_data.QUERY_DNS_FILE

        dir_list = ((comm_data.comm_query_flow_down_path , query_flow_src_path),     
                    (comm_data.comm_query_url_down_path, query_url_src_path),       
                    (comm_data.comm_query_dns_down_path, query_dns_src_path),    
                    )
        # --> query 下载事件
        for dst_dir,src_dir in dir_list:
            query_fname_list,query_fname_size = get_cloud_query_xml_fname_limit(tmp_file, src_dir)
            cloud_down_file_limit(oss,bucket,query_fname_list,query_fname_size, dst_dir,down_lim)

        # 4. 删除xml临时文件
        os.remove(tmp_file)

        time.sleep(0.5)

#------------------------------------------------------.
# @brief 规则库，白名单，升级包，配置 在线业务         |
#------------------------------------------------------*
def conf_online_down_busi():
    print "-------------------- Begin CONF online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("-------------------- Begin CONF online DOWN BUSINESS -------------")
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_down_filelist.xml'

    while True:

        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print "conf_online_down busi exception:"
            comm_cloud_logger.debug("conf_online_down busi exception:")
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
            comm_cloud_logger("[%s]"%ex)
            continue

        # --> 升级包 下载事件
        mtx_fname_list = get_cloud_xml_fname(tmp_file, comm_data.UPDATE_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_ue_down_path)

        # --> mtx规则库 下载事件
        mtx_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.MTX_RULE_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_mtx_rule_down_path)

        # --> vds规则库 下载事件
        mtx_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.VDS_RULE_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_vds_rule_down_path)

        # --> apt规则库 下载事件
        mtx_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.APT_RULE_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_apt_rule_down_path)

        # --> surl规则库 下载事件
        mtx_fname_list = get_cloud_query_xml_fname(tmp_file, comm_data.SURL_RULE_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_surl_rule_down_path)

        # --> 配置 下载事件
        mtx_fname_list = get_cloud_xml_fname(tmp_file, comm_data.CONF_FILE)
        cloud_down_file(oss, bucket, mtx_fname_list, comm_data.comm_conf_down_path)

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
        os.remove(tmp_file)

        time.sleep(0.5)
#------------------------------------------------------.
# @brief 限速模式规则库，白名单，升级包，配置 在线业务 |
#------------------------------------------------------*
def conf_online_down_limit_busi():
    print "------------- Begin Limit Speed Model CONF online DOWN BUSINESS -------------"
    comm_cloud_logger.debug("---------Begin Limit Speed Model CONF online DOWN BUSINESS -------")
    global uuid
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    tmp_file = '/tmp/dev_conf_down_limit_filelist.xml'

    while True:

        # 1. 读取云库中目前都有哪些文件
        try:
            res = oss.get_bucket(bucket, prefix=uuid)
        except Exception, ex:
            print "conf_online_down busi exception:"
            comm_cloud_logger.debug("conf_online_down busi exception:")
            print ex
            comm_cloud_logger.debug("[%s]" %ex)
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
            comm_cloud_logger("[%s]"%ex)
            continue

        # --> 升级包 下载事件
        up_fname_list,up_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.UPDATE_FILE)
        cloud_down_file_limit(oss,bucket,up_fname_list,up_fname_size,comm_data.comm_reg_down_path,down_lim)

        # --> mtx规则库 下载事件
        rule_fname_list,rule_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.MTX_RULE_FILE)
        cloud_down_file_limit(oss,bucket,rule_fname_list,rule_fname_size,comm_data.comm_mtx_rule_down_path,down_lim)

        # --> vds规则库 下载事件
        vds_fname_list,vds_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.VDS_RULE_FILE)
        cloud_down_file_limit(oss, bucket,vds_fname_list,vds_fname_size,comm_data.comm_vds_rule_down_path,down_lim)

        # --> apt规则库 下载事件
        apt_fname_list,apt_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.APT_RULE_FILE)
        cloud_down_file_limit(oss, bucket,apt_fname_list,apt_fname_size,comm_data.comm_apt_rule_down_path,down_lim)

        # --> surl规则库 下载事件
        surl_fname_list,surl_fname_size = get_cloud_query_xml_fname_limit(tmp_file, comm_data.SURL_RULE_FILE)
        cloud_down_file_limit(oss,bucket,surl_fname_list,surl_fname_list,comm_data.comm_surl_rule_down_path,down_lim)

        # --> 配置 下载事件
        conf_fname_list,conf_fname_size = get_cloud_xml_fname_limit(tmp_file, comm_data.CONF_FILE)
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
        os.remove(tmp_file)

        time.sleep(0.5)

#-----------------------------------------.
# @brief 设备状态在线上行处理业务         |
#-----------------------------------------*
def online_up_busi(comm_ali_path,comm_file):
    global uuid
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_ali_path], ['.ok']):
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_file, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
        time.sleep(0.5)
#-----------------------------------------.
# @brief 限速模块设备状态在线上行处理业务 |
#-----------------------------------------*
def online_up_limit_busi(comm_ali_path,comm_file):
    global uuid
    global up_lim,down_lim
    print '*!!!!!!!!!!!%s!!!!!!!!!!'%(up_lim,down_lim)
    oss = OssAPI2(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET

    while True:
        # --> devstat 设备信息文件
        for up_file in file_up_down_tran.file_filter_bysuffix([comm_ali_path], ['.ok']):
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_file, os.path.basename(up_file))
            #retn = cloud_up_file(oss, bucket, up_file, dst_file)
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
        time.sleep(0.5)
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
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.TOPO_FILE, os.path.basename(up_file))
            retn = cloud_up_file(oss, bucket, up_file, dst_file)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
        time.sleep(0.5)

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
            #dst_file = str(uuid) + comm_data.REG_FILE + os.path.basename(up_file)
            dst_file = os.path.join(str(uuid), comm_data.TOPO_FILE, os.path.basename(up_file))
            #retn = cloud_up_file(oss, bucket, up_file, dst_file)
            retn = cloud_up_file_limit(oss, bucket,up_file,dst_file,up_lim)
            if (retn == 0):
                #上传成功 删除本地文件
                os.remove(up_file)
        time.sleep(0.5)


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
    comm_ali_path = comm_data.comm_ve_ali_path
    comm_file = comm_data.VIRUS_FILE
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
            comm_log_record.logger.info(traceback.format_exc())
        finally:
            time.sleep(1)


#各种事件开始线程处理
def try_catch_busi(event_busi):
    try:
        thread = threading.Thread(target=event_busi,args=())
        thread.setDaemon(True)
        thread.start()
    except Exception, ex:
        print ex
        host_cloud_logger.debug("[%s]" %ex)

def judge_limit_conf():
    print comm_data.limit_file
    if os.path.exists(comm_data.limit_file):
        with open(comm_data.limit_file,'r') as fp:
            data = fp.read()
            up_limit = data.split('-')[0]
            down_limit = data.split('-')[1]
            limit_t = (up_limit,down_limit)
            return limit_t 
    else:
        return 0 
        
def main():
    
    global comm_cloud_logger
    global  up_lim
    global  down_lim


    # 读取目录信息
    comm_init.read_pre_json()

    #初始化日志
    host_cloud_log_init()

    # 读取配置文件
    read_interface_xml()
    # 创建目录
    create_dir()
    # 先启动配置重载线程，否则可能阻塞在注册的地方 
    try_catch_busi(config_reload_thread)
	

    if (cloudcon == "0"):
        print "Begin offline busi!"
        comm_cloud_logger.debug("Begin offline busi!")
        cloudid_topo.load_topo_dist()

        if (comm_data.intranet_mode == "1"):
            comm_cloud_logger.debug("intranet mode, do not start offline busi...")
            return 
        # 开启 设备注册 离线上行 业务
        try_catch_busi(reg_offline_up_busi)

        #开启 事件删除 离线业务
        try_catch_busi(offline_delete_event_busi)

        # 开启 设备注册 离线下行 业务
        try_catch_busi(reg_offline_down_busi)

        # 开启 自动离线注册 业务
        try_catch_busi(reg_offline_autoreg_busi)

        #开启MTX离线上行业务
        try_catch_busi(mtx_offline_up_busi)    

        # 开启MTX离线下行业务
        try_catch_busi(mtx_offline_down_busi)    

        # 开启 规则，配置，升级包，白名单 离线下发业务
        try_catch_busi(conf_offline_down_busi)    

    else:
        print 'comm_data.limit_file %s'%comm_data.limit_file
        ret = judge_limit_conf() 
        #print(type(ret))
        #print ret
        if ret == 0:
            print "Begin Online Busi!"
            comm_cloud_logger.debug("Begin Online Busi!")

            # 开启 设备注册 在线上行 业务
            try_catch_busi(reg_online_up_busi)    

            # 开启 设备注册 在线下行 业务
            try_catch_busi(reg_online_down_busi)    

            #开启MTX在线上行业务
            try_catch_busi(mtx_online_up_busi)    

            # 开启MTX在线下行业务
            try_catch_busi(mtx_online_down_busi)    

            # 开启远程查询在线上行业务
            try_catch_busi(query_online_down_busi)    

            # 开启远程查询在线下行业务
            try_catch_busi(query_online_up_busi)    

            # 开启 规则，配置，升级包，白名单 在线下行业务
            try_catch_busi(conf_online_down_busi)    
            
            # 开启 设备状态信息 在线上行业务
            try_catch_busi(conf_online_up_busi)    

            #开启 异常行为 在线上行业务
            try_catch_busi(abb_online_up_busi)    

            #开启 恶意URL 在线上行业务
            try_catch_busi(virus_online_up_busi)    

            #开启 恶意代码 在线上行业务
            try_catch_busi(surl_online_up_busi)    

            #开启 系统流量 在线上行业务
            try_catch_busi(sysflow_online_up_busi)    

            #开启 topo 在线上行业务
            try_catch_busi(topo_online_up_busi)    

            #开启 报表 上行业务
            try_catch_busi(report_online_up_busi)    

            #开启 报表 在线下行业务
            try_catch_busi(report_online_down_busi)    
        else:
            up_lim = ret[0]
            down_lim = ret[1]
            
            # 限速 - 开启设备注册 限速在线上行 业务
            try_catch_busi(reg_online_up_limit_busi)    

            # 限速 - 开启设备注册 限速在线下行 业务
            try_catch_busi(reg_online_down_limit_busi)    

            #限速 - 开启MTX 限速在线上行业务
            try_catch_busi(mtx_online_up_limit_busi)    

            #限速 - 开启MTX在线下行业务
            try_catch_busi(mtx_online_down_limit_busi)    

            #限速 -  开启远程查询在线上行行业务
            try_catch_busi(query_online_up_limit_busi)    

            #限速 -  开启远程限速在线下行业务
            try_catch_busi(query_online_down_limit_busi)    

            #限速 -开启 规则，配置，升级包，白名单 在线下行业务
            try_catch_busi(conf_online_down_limit_busi)    

            #限速- 开启 设备状态信息 在线上行业务
            try_catch_busi(conf_online_up_limit_busi)    

            #限速 - 开启 异常行为 在线上行业务
            try_catch_busi(abb_online_up_limit_busi)    

            #限速- 开启 恶意URL 在线上行业务
            try_catch_busi(virus_online_up_limit_busi)    

            #限速 - 启 恶意代码 在线上行业务
            try_catch_busi(surl_online_up_limit_busi)    

            ##限速 - 开启 系统流量 在线上行业务
            try_catch_busi(sysflow_online_up_limit_busi)    

            ##限速 -开启 topo 在线上行业务
            try_catch_busi(topo_online_up_limit_busi)    

            ##限速 -开启 报表 上行业务
            try_catch_busi(report_online_up_limit_busi)    

            ##限速 - 开启 报表 在线下行业务
            try_catch_busi(report_online_down_limit_busi) 



def host_cloud_log_init():
    global comm_cloud_logger
    comm_cloud_logger = cloud_log.CloudLogger(path = comm_data.cloud_log_path,level = '',format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')

if __name__ == "__main__":

    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)
    reload(sys)
    sys.setdefaultencoding('utf-8')
    main()

    while (True):
        time.sleep(3)

###################### file end ####################
