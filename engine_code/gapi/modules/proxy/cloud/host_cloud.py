#!/usr/bin/python
# _*_coding:utf8 _*_
import json
import signal
import os
import sys
import time
from xml.etree import ElementTree
import xml.dom.minidom
import threading
import thread
import print_color
import cloudid_topo
import cloud_log
import shutil
import traceback
from shutil import copy
sys.path.append("../")
import file_up_down_tran
import comm_common
import comm_data
import comm_init
from oss.oss_api import *

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
local_uuid = 0
host_cloud_logger=None

def create_dir():
    #mtx
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_dst_path+"zip/")
    #reg
    comm_common.creat_if_dir_notexist(comm_data.host_reg_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_reg_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_reg_dst_path+"temp/")

    #update
    comm_common.creat_if_dir_notexist(comm_data.host_update_src_path)
    #rule
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_rule_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_rule_src_path+"zip/")
    comm_common.creat_if_dir_notexist(comm_data.host_vds_rule_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_vds_rule_src_path+"zip/")
    comm_common.creat_if_dir_notexist(comm_data.host_apt_rule_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_apt_rule_src_path+"zip/")
    comm_common.creat_if_dir_notexist(comm_data.host_surl_rule_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_surl_rule_src_path+"zip/")
    #conf
    comm_common.creat_if_dir_notexist(comm_data.host_conf_src_path)
    
    #远程flow
    comm_common.creat_if_dir_notexist(comm_data.host_query_flow_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_flow_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_flow_dst_path+"zip/")
    #远程dns
    comm_common.creat_if_dir_notexist(comm_data.host_query_dns_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_dns_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_dns_dst_path+"zip/")
    #远程url
    comm_common.creat_if_dir_notexist(comm_data.host_query_url_src_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_url_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_query_url_dst_path+"zip/")
    
    #surl
    comm_common.creat_if_dir_notexist(comm_data.host_surl_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_surl_dst_path+"zip/")
    #sysflow
    comm_common.creat_if_dir_notexist(comm_data.host_flow_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_flow_dst_path+"zip/")
    #abb
    comm_common.creat_if_dir_notexist(comm_data.host_abb_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_abb_dst_path+"zip/")
    #virus
    comm_common.creat_if_dir_notexist(comm_data.host_virus_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_virus_dst_path+"zip/")
    #stat
    comm_common.creat_if_dir_notexist(comm_data.host_stat_dst_path)
    comm_common.creat_if_dir_notexist(comm_data.host_stat_dst_path+"zip/")
    #topo
    comm_common.creat_if_dir_notexist(comm_data.host_topo_dst_path)
#------------------------------.
# @brief 主程序捕获函数        |
#------------------------------*
def sig_exit(sig, stack):
    print("recv signal[%d]. exit." %sig)
    host_cloud_logger.debug("recv signal[%d]. exit." %sig)
    exit()

#------------------------------.
# @brief 主程序捕获函          |
# @param buf        源数据     |
# @param file_path  保存的文件 |
#------------------------------*
def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        print e
        host_cloud_logger.debug("[%s]" %e)
#------------------------------.
# @brief 主程序捕获函数        |
#------------------------------*
def sig_exit(sig, stack):
    print("recv signal[%d]. exit." %sig)
    host_cloud_logger.debug("recv signal[%d]. exit." %sig)
    exit()

#------------------------------.
# @brief 主程序捕获函          |
# @param buf        源数据     |
# @param file_path  保存的文件 |
#------------------------------*
def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        print e
        host_cloud_logger.debug("[%s]" %e)
    else:
        tmp_file.write(buf)
        tmp_file.close()

#------------------------------.
# @brief 主程序捕获函数        |
# @param xml_path xml文件路径  |
# @return list                 |
#------------------------------*
def get_cloud_xml_fname(xml_path,prefix_path):
    dom = xml.dom.minidom.parse(xml_path)
    root = dom.documentElement
    content_list = root.getElementsByTagName('Contents')

    list = []
    for file_info in content_list:
        keynode = file_info.getElementsByTagName("Key")[0]
        fname = keynode.childNodes[0].nodeValue
        dir_name = fname.split("/")
        if (dir_name[1] == 'query'):
            dir = dir_name[1]+"/"+dir_name[2]
        else:
            dir = dir_name[1]
        if(dir == prefix_path):
            list.append(keynode.childNodes[0].nodeValue)
    return list

def read_interface_xml():
    global interface_conf_file
    global local_uuid
    root = ElementTree.parse(interface_conf_file)

    devtype_node = root.find("selfinfo/uuid")
    local_uuid = devtype_node.text
    print "local uuid is %s" %(local_uuid)
    host_cloud_logger.debug("local uuid is %s" %(local_uuid))

#-------------------------------------------.
# @brief 安全拷贝一个 .ok 后缀文件到某个目录|
# @param srcfile 源文件名                   |
# @param dstpath 目标名                     |
# @return 0 成功 -1 失败                    |
#-------------------------------------------*
def okfile_safe_copy(srcfile, dstpath):
    try:
        if os.path.isdir(dstpath):
            basename = os.path.basename(srcfile)
            dstfile = os.path.join(dstpath, basename)
        else:
            dstfile = dstpath
        tmpfile = dstfile + '.tmp'
        shutil.copy(srcfile, tmpfile)
        os.rename(tmpfile, dstfile)
    except Exception:
        print(traceback.format_exc())
        host_cloud_logger.debug("[%s]" %(traceback.format_exc()))
        return -1
    else:
        print "MOVE LOCAL[%s] -> COMM[%s] SUCC!" %(srcfile, dstfile)
        host_cloud_logger.info("MOVE LOCAL[%s] -> COMM[%s] SUCC!" %(srcfile, dstfile))
        return 0

#------------------------------.
# @brief 从云库下载主业        |
#                              |
# @param oss 云库连接句柄      |
# @param path 下载到的文件路径 |
# @return                      |
#------------------------------*
def cloud_down_file(oss, bucket, flist, path):

    #  遍历文件list 分别进行下载
    for fname in flist:
        down_fname = ''
        final_name = path + os.path.basename(fname)
        down_fname = final_name + ".tmp"
        t = time.time()

        try:
            print_color.print_downloading(fname, down_fname)
            host_cloud_logger.debug("CLOUD[%s] -> FILE[%s]..." %(fname,down_fname))
            res = oss.get_object_to_file(bucket, fname, down_fname)
        except Exception, ex:
            print ex
            host_cloud_logger.debug("%s" %ex)
            continue
        if res.status != 200:
            print_color.print_download(fname, final_name, "FAIL")
            host_cloud_logger.debug("CLOUD[ %s] ->FILE[%s]FILE" %(fname,final_name))
            print res.read()
            host_cloud_logger.debug("[%s]" %(res.read()))
            break
        else:
            print_color.print_download(fname, final_name, "SUCC")
            host_cloud_logger.debug("CLOUD[%s] ->FILE[%s]SUCC" %(fname,final_name))
            print "-->spend [%f] second(s). file_size[%d]." %(time.time()-t, os.path.getsize(down_fname))
            host_cloud_logger.debug("-->spend [%f] second(s). file_size[%d]."\
                                     %(time.time()-t, os.path.getsize(down_fname)))
            try:
                os.rename(down_fname, final_name)
            except Exception,ex:
                print ex
                host_cloud_logger.debug("[%s]"%ex)
                continue
            # 5. 下载成功 删除云端对应的文件
            try:
                res = oss.delete_object(bucket, fname)
            except Exception,ex:
                print ex
                host_cloud_logger.debug("[%s]" %ex)
                continue
            if (res.status != 204):
                print_color.print_delete(fname, "FAIL")
                host_cloud_logger.debug("CLOUD[%s] FAIL" %(fname))
                print res.read()
                host_cloud_logger.debug("[%s]" %(res.read()))
            else:
                print_color.print_delete(fname, "SUCC")
                host_cloud_logger.debug("CLOUD[%s] SUCC" %(fname))

#-----------------------------------------.
# @brief 像云端上传本地文件               |
# @param oss                远端连接句柄  |
# @param local_fname        文件目录      |
# @param remote_fname,      后缀          |
#-----------------------------------------*
def cloud_up_file(oss, bucket, local_fname, remote_fname):
    t = time.time()
    try:
        res = oss.put_object_from_file(bucket, remote_fname, local_fname)
    except Exception, ex:
        print ex
        return -1
    if res.status != 200:
        print_color.print_upload(local_fname, remote_fname, "FAIL")
        host_cloud_logger.debug("UP FILE[%s] -> CLOUD[%s] FAIL" %(local_fname,remote_fname))
        print res.read()
        retn = -1
    else:
        print_color.print_upload(local_fname, remote_fname, "SUCC")
        host_cloud_logger.debug("UP FILE[%s] -> CLOUD[%s] SUCC" %(local_fname,remote_fname))
        print "-->spend [%f] second(s). file_size[%d]" %(time.time()-t, os.path.getsize(local_fname))
        host_cloud_logger.debug("-->spend [%f] second(s). file_size[%d]"\
                                %(time.time()-t, os.path.getsize(local_fname)))
        retn = 0
    return retn

    
#如果对应值为空，用\N代替
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
# 升级包和规则库的处理事件                |
#-----------------------------------------*
def cloud_up_event(src_path,dst_path,bucket):

    for up_file in comm_common.file_filter_bysuffix([src_path], ['.ok']):
        oldname = os.path.basename(up_file)
        name = os.path.splitext(oldname)[0]
        item = name.split['_']
        if item[1] == 'all':
            new_name = item[0]
            dst_file = dst_path + new_name
            retn = cloud_up_file(oss,bucket,new_name,dst_file)
        elif item[1] == 'devidlist':
            up_fl = open(up_file, "r")
            tran_file = json.load(up_file)
            up_fl.close()
            i = 0
            for tran_file[i] in tran_file:
                new_pid = tran_file[i]["pid"]
                new_name = item[0]+".ok"
                dst_file = new_pid + dst_path + new_name
                retn = cloud_up_file(oss,bucket,new_name,dst_file)
        else:
            pass
        if retn == 0:
            os.remove(up_file)

def down_decompre_busi(dst_path):
    dst_path_zip = dst_path+"zip/"
    for down_file in comm_common.file_filter_bysuffix([dst_path_zip],[".ok"]):
        if os.path.exists(down_file):
            try:
                if dst_path.split("/")[3]=='query':
                    file = os.path.basename(down_file)
                    dst_path = dst_path + file[:-3]
                    comm_common.extractzip(down_file,dst_path,passwd='9527')
                else:
                    comm_common.extractzip(down_file,dst_path,passwd='9527')
            except Exception, ex:
                print "==extractzip Excepiton==>file[%s] DELETE IT!" %(down_file)
                host_cloud_logger.debug("==extractzip Excepiton==>file[%s] DELETE IT!" %(down_file))
                print ex
                host_cloud_logger.debug("[%s]" %ex)
                os.remove(down_file)
                continue
            os.remove(down_file)
        else:
            break    
    time.sleep(0.5)

def cloud_conf_up_busi():
    print "-------------------- Begin CONF UP BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin CONF UP BUSINESS ------")
    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET

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
            host_cloud_logger.debug("*** WARNING **** file[%s] name illegal. backup.")
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
        else :
            return FTYPE_DEVID_MULTICAST

    def specialcast_src_file_list(src_dir):
        for fname in os.listdir(src_dir):
            fname = os.path.join(src_dir, fname)
            if os.path.isfile(fname) and file_type(fname) == FTYPE_DEVID_MULTICAST:
                # 切掉结尾的 _devidlist.ok 就是对应的数据文件的名称
                if os.path.exists(fname) and os.path.isfile(fname):
                    if src_dir == comm_data.host_update_src_path:
                        # 对升级包作 md5 校验
                        # 约定先拷贝 _devlist.ok 文件，后拷贝升级包文件
                        # 升级包格式如下:
                        #GMS-1.1.1.2-b227c5683aecd4c0a21f1c32468c3f1e-normal_devlist.ok
                        #GMS-1.1.1.2-b227c5683aecd4c0a21f1c32468c3f1e-normal.ok
                        if checksum(src_dir) == True:
                            yield fname
                    else:
                        yield fname

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
            shutil.copy(src_file, dst_tmp_file)
            os.rename(dst_tmp_file, dst_file)

    def multi_file_cloud_up(oss, bucket, src_file, dst_file_list):
        for dst_file in dst_file_list:
            #1. 从dst_file中的uuid通过拓扑得到topuuid
            # dst_file 为/FILE_TYPE/20140312100500_uuid.ok
            # 取得结果name = /FILE_TYPE/2014031200500_uuid
            name = os.path.splitext(dst_file)[0]
            item = name.split("_")
            uuid = item[-1]
            topuuid = cloudid_topo.get_topid_by_uuid(uuid)
            print "======="
            print topuuid
            print "======="
            host_cloud_logger.debug("[%s]" %topuuid)

            if topuuid == "no_value":
                print "file %s ,uuid %s not in topo!!!" %(src_file, uuid)
                host_cloud_logger.debug("file %s ,uuid %s not in topo!!!" %(src_file, uuid))
                continue

            if topuuid == local_uuid:
                # 本地文件。在二合一模式下会出现。
                dst_basename = os.path.basename(dst_file)

                if comm_data.UPDATE_FILE in dst_file:
                    dst_dir = comm_data.comm_ue_down_path
                elif comm_data.CONF_FILE in dst_file:
                    dst_dir = comm_data.comm_conf_down_path
                elif comm_data.MTX_RULE_FILE in dst_file:
                    dst_dir = comm_data.comm_mtx_rule_down_path
                elif comm_data.VDS_RULE_FILE in dst_file:
                    dst_dir = comm_data.comm_vds_rule_down_path
                elif comm_data.SURL_RULE_FILE in dst_file:
                    dst_dir = comm_data.comm_surl_rule_down_path
                elif comm_data.APT_RULE_FILE in dst_file:
                    dst_dir = comm_data.comm_apt_rule_down_path
                dst_zipfile = os.path.join(dst_dir, dst_basename)
                print dst_zipfile
                dst_file = os.path.split(src_file)[0]+"/"+dst_basename
                if src_file == dst_file:
                    comm_common.createzip(dst_zipfile,dst_file,passwd='9527')
                else:
                    copy(src_file,dst_file)
                    comm_common.createzip(dst_zipfile,dst_file,passwd='9527')
                    os.remove(dst_file)
                continue

            #2. 封装成 topuuid/FILE_TYPE/2014031200500_uuid.ok
            #up_file = os.path.join(topuuid, dst_file)
            up_file = topuuid + dst_file

            #3. 上传至 阿里云
            dst_dir = os.path.split(src_file)[0]+"/zip/"
            dst_basename = os.path.basename(dst_file)
            dst_file = os.path.split(src_file)[0]+"/"+dst_basename
            print dst_file
            if src_file == dst_file:
                src_file = dst_file 
                dst_zipfile = os.path.join(dst_dir,dst_basename)
                print dst_file,dst_zipfile
                comm_common.createzip(dst_zipfile,dst_file,passwd='9527')
            else:
                copy(src_file,dst_file)
                dst_zipfile = os.path.join(dst_dir,dst_basename)
                print dst_file,dst_zipfile
                comm_common.createzip(dst_zipfile,dst_file,passwd='9527')
                os.remove(dst_file)
            retn = cloud_up_file(oss,bucket,dst_zipfile,up_file)
            if retn == 0:
                #上传成功，删除本地文件
                print "==== CONF ==== up file [%s] -> [%s] SUCC!" %(dst_zipfile, up_file)
                host_cloud_logger.debug("==== CONF ==== up file [%s] -> [%s] SUCC!" %(src_file, up_file))
                os.remove(dst_zipfile)
            else:
                print "==== CONF ==== up file [%s] -> [%s] FAIL!!!!!!!!!!!!!" %(dst_zipfile, up_file)
                host_cloud_logger.debug("==== CONF ==== up file [%s] -> [%s] FAIL!!!!!!!!!!!!!" %(src_file, up_file))
                #上传失败，退出循环

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
                host_cloud_logger.debug("*****ERROR******")
                print(traceback.format_exc())
                uuid_list = []
        return uuid_list

    def specialcast_dst_file_list(src_file, dst_dir):
        file_list = []
        fbase_name = os.path.basename(src_file)
        dst_file = os.path.join(dst_dir, fbase_name )
        file_list.append(dst_file)
        return file_list

    def multicast_dst_file_list(src_file, devid_file, dst_dir):
        uuid_list = make_dst_uuid_list(devid_file)
        file_list = []
        fbase_name = os.path.split(src_file)[-1][:-3]
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

    update_dst_path = "/" + comm_data.UPDATE_FILE + "/"
    conf_dst_path = "/" + comm_data.CONF_FILE + "/"
    mtx_rule_dst_path = "/" + comm_data.MTX_RULE_FILE + "/"
    apt_rule_dst_path = "/" + comm_data.APT_RULE_FILE + "/"
    vds_rule_dst_path = "/" + comm_data.VDS_RULE_FILE + "/"
    surl_rule_dst_path = "/" + comm_data.SURL_RULE_FILE + "/"

    dir_list = ((comm_data.host_mtx_rule_src_path , mtx_rule_dst_path),      # mtx规则库
                (comm_data.host_apt_rule_src_path, apt_rule_dst_path),             # apt规则库 
                (comm_data.host_vds_rule_src_path, vds_rule_dst_path),             # vds规则库
                (comm_data.host_surl_rule_src_path, surl_rule_dst_path),             # surl规则库
                (comm_data.host_conf_src_path, conf_dst_path),             # 配置
                (comm_data.host_update_src_path, update_dst_path),     # 升级包
                )


    while True:
        try:
            for src_dir, dst_dir in dir_list:
                if (src_dir == comm_data.host_conf_src_path or src_dir == comm_data.host_update_src_path):
                    for src_file, devid_file in multicast_src_file_list(src_dir):
                        dst_file_list = multicast_dst_file_list(src_file, devid_file,dst_dir)
                        host_cloud_logger.debug("src file[%s] " % (src_file))
                        host_cloud_logger.debug("dst file list[%s]" % dst_file_list)
                        if dst_file_list == []:
                            print("**** WARNING **** multicast dst uuid list == null")
                            host_cloud_logger.debug("*** WARNING **** multicast dst uuid list == null")
                            os.rename(src_file, src_file + '.bak')
                            os.rename(devid_file, devid_file + '.bak')
                        else:
                            multi_file_cloud_up(oss, bucket, src_file, dst_file_list)
                            os.remove(src_file)
                else:
                    for src_file in specialcast_src_file_list(src_dir):
                        dst_file_list = specialcast_dst_file_list(src_file, dst_dir)
                        host_cloud_logger.debug("src file[%s] " % (src_file))
                        host_cloud_logger.debug("dst file list[%s]" % dst_file_list)
                        if dst_file_list == []:
                            print("**** WARNING **** multicast dst uuid list == null")
                            host_cloud_logger.debug("*** WARNING **** multicast dst uuid list == null")
                            os.rename(src_file, src_file + '.bak')
                            #os.rename(devid_file, devid_file + '.bak')
                        else:
                            multi_file_cloud_up(oss, bucket, src_file, dst_file_list)
                            os.remove(src_file)

                for src_file in broadcast_src_file_list(src_dir):
                    dst_file_list = broadcast_dst_file_list(src_file, dst_dir)
                    print("src file[%s]" % src_file)
                    host_cloud_logger.debug("src file[%s]" % src_file)
                    print("dst file list[%s]" % dst_file_list)
                    host_cloud_logger.debug("dst file list[%s]" % dst_file_list)
                    if dst_file_list == []:
                        print("**** WARNING **** broadcast dst uuid list == null")
                        host_cloud_logger.debug("*** WARNING **** broadcast dst uuid list == null")
                        os.rename(src_file, src_file + '.bak')
                    else:
                        multi_file_cloud_up(oss, bucket, src_file, dst_file_list)
                        os.remove(src_file)
        except Exception:
            print("**** ERROR ****")
            host_cloud_logger.debug("*** ERROR **** ")
            print(traceback.format_exc())
            host_cloud_logger.debug("[%s]" %(traceback.format_exc()))
        finally:
            time.sleep(1)
#规则事件拷贝改名
def rule_event_busi():
    src_path = comm_data.host_vds_src_path     # vds规则库
    dst_path = comm_data.host_vds_rule_src_path
    while True:
        for src_file in file_up_down_tran.file_filter_bysuffix([src_path], ['.gz']):
            oldname = os.path.basename(src_file)
            name = oldname.split(".")[0] 
            new_name = name+"_all.ok"
            new_name = os.path.join(src_path,new_name)
            os.rename(src_file,new_name)
            okfile_safe_copy(new_name,dst_path)
            os.remove(new_name)

        time.sleep(1)
        

#连接阿里云，遍历到对应的bucket和文件目录
def handle_bucket(cloud_oss,cloud_bucket,cloud_prefix):
        # 1. 读取云库中目前都有哪些文件
    global buf
    try:
        if cloud_prefix == "/":
            res = cloud_oss.get_bucket(cloud_bucket,delimiter=cloud_prefix)
            #buf=res.read()
            return res.read()
        else:
            res = cloud_oss.get_bucket(cloud_bucket,prefix = cloud_prefix)
            buf=res.read()
            return buf
    except Exception, ex:
        print "@@@@@@ get Exceptin @@@@@@@@"
        host_cloud_logger.debug("@@@@@@ get Exceptin @@@@@@@@")
        print ex
        host_cloud_logger.debug(ex)
        return -1
    else:
        if res.status != 200:
            print "list %s bucket FAIL." %(bucket)
            host_cloud_logger.debug("list %s bucket FAIL." %(bucket))
            print res.status, res.read()
            host_cloud_logger.debug(res.status,res.read())
            return -1

#连接阿里云 无特殊处理事件的下载
def cloud_down_busi(host_dst_path,data_file,type_file,type_path):
    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    #进入上传循环业务
    while True:
        # 1. 读取云库中目前都有哪些文件
        delimiter = "/"
        buf = handle_bucket(oss,bucket,delimiter)
        if buf==-1:
            continue
        try:
            write_buf_to_file(buf,type_file)
        except Exception,ex:
            host_cloud_logger.debug(ex)
            continue
        try:
            dom = xml.dom.minidom.parse(type_file)
        except Exception,ex:
            host_cloud_logger.debug(ex)
            continue
        root = dom.documentElement
        surl_list = root.getElementsByTagName('CommonPrefixes')
        for topid in surl_list:
            keynode = topid.getElementsByTagName("Prefix")[0]
            pre_uuid = keynode.childNodes[0].nodeValue
            prefix_path = pre_uuid +type_path
            buf = handle_bucket(oss,bucket,prefix_path)
            if buf==-1:
                continue

        # 2. 将得到的xml文件存在本地
            try:
                write_buf_to_file(buf, type_file)
            except Exception,ex:
                print ex
                host_cloud_logger.debug(ex)
                continue

        #3. 设备状态信息文件下载
            surl_fname_list = get_cloud_xml_fname(type_file, data_file)
            if type_path == 'topo/':
                dst_path = os.path.join(host_dst_path , pre_uuid)
                comm_common.creat_if_dir_notexist(dst_path)
            else:
                dst_path = host_dst_path+"zip/"
            cloud_down_file(oss, bucket, surl_fname_list,dst_path)
           
            os.remove(type_file)
            time.sleep(0.5)
        if type_path == 'topo/':
            pass
        else:
            down_decompre_busi(host_dst_path)
    else:
        print("EXIT!")
        host_cloud_logger.debug("EXIT!")

def cloud_topo_down_busi():
    print "-------------------- Begin topo DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin topo DOWN BUSINESS ------")
    tmp_file = '/tmp/ali_topo_up_filelist.xml'
    path = 'topo/'
    dst_path = comm_data.host_topo_dst_path
    cloud_down_busi(dst_path,comm_data.TOPO_FILE,tmp_file,path)
       
def cloud_abb_down_busi():
    print "-------------------- Begin ABB DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin ABB DOWN BUSINESS ------")
    tmp_file = '/tmp/ali_abb_up_filelist.xml'
    path = 'abb/'
    dst_path = comm_data.host_abb_dst_path
    cloud_down_busi(dst_path,comm_data.ABB_FILE,tmp_file,path)
       
def cloud_virus_down_busi():
    print "-------------------- Begin VIRUS DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin VIRUS DOWN BUSINESS ------")
    tmp_file = '/tmp/ali_virus_up_filelist.xml'
    path = 'virus/'
    dst_path = comm_data.host_virus_dst_path
    cloud_down_busi(dst_path,comm_data.VIRUS_FILE,tmp_file,path)

def cloud_flow_down_busi():
    print "-------------------- Begin FLOW DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin FLOW DOWN BUSINESS ------")
    tmp_file = '/tmp/ali_flow_up_filelist.xml'
    path = 'sysflow/'
    dst_path = comm_data.host_flow_dst_path
    cloud_down_busi(dst_path,comm_data.FLOW_FILE,tmp_file,path)

def cloud_surl_down_busi():
    print "-------------------- Begin SURL DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin SURL DOWN BUSINESS ------")
   # 将读到的res.read()内容存储到xml文件中 
    tmp_file = '/tmp/ali_surl_up_filelist.xml'
    path = 'surl/'
    dst_path = comm_data.host_surl_dst_path
    cloud_down_busi(dst_path,comm_data.SURL_FILE,tmp_file,path)


def cloud_stat_down_busi():
    print "-------------------- Begin STAT DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin STAT DOWN BUSINESS ------")
    tmp_file = '/tmp/ali_stat_up_filelist.xml'
    path = 'stat/'
    dst_path = comm_data.host_stat_dst_path
    cloud_down_busi(dst_path,comm_data.STAT_FILE,tmp_file,path)
#-----------------------------------------.
# MTX 上报事件上行处理线程                |
#-----------------------------------------*
def cloud_mtx_down_busi():
    print "-------------------- Begin MTX DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin MTX DOWN BUSINESS -------")
    tmp_file = '/tmp/ali_mtx_up_filelist.xml'
    path = 'mtx/'
    dst_path = comm_data.host_mtx_dst_path
    cloud_down_busi(dst_path,comm_data.MTX_FILE,tmp_file,path)
    
def cloud_remote_flow_down_busi():
    print "-------------------- Begin REMOTE FLOW DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REMOTE FLOW DOWN BUSINESS -------")
    tmp_file = '/tmp/ali_remote_flow_up_filelist.xml'
    path = 'query/flow/'
    dst_path = comm_data.host_query_flow_dst_path
    cloud_down_busi(dst_path,comm_data.QUERY_FLOW_FILE,tmp_file,path)


def cloud_remote_dns_down_busi():
    print "-------------------- Begin REMOTE DNS DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REMOTE DNS DOWN BUSINESS -------")
    tmp_file = '/tmp/ali_remote_dns_up_filelist.xml'
    path = 'query/dns/'
    dst_path = comm_data.host_query_dns_dst_path
    cloud_down_busi(dst_path,comm_data.QUERY_DNS_FILE,tmp_file,path)

def cloud_remote_url_down_busi():
    print "-------------------- Begin REMOTE URL  DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REMOTE URL DOWN BUSINESS -------")
    tmp_file = '/tmp/ali_remote_url_up_filelist.xml'
    path = 'query/url/'
    dst_path = comm_data.host_query_url_dst_path
    cloud_down_busi(dst_path,comm_data.QUERY_URL_FILE,tmp_file,path)

    
    # 内网模式下
def inline_reg_down_busi():
    print "-------------------- Begin  REG INLINE DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REG INLINE DOWN BUSINESS ------")
    while True:
        # --> 从国家主机顶级设备读取
        for reg_file in comm_common.file_filter_bysuffix([comm_data.comm_reg_up_path], ['.ok']):
            #1. 将reg 入库格式 直接转换为 json 格式
            file = open(reg_file, "r")
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

            #更新 uuid 对应 topid 拓扑关系表   
            cloudid_topo.update_topid_uuid_topo(item[2], item[1]) 

            file_name = os.path.basename(reg_file)
            tmp_name = file_name + ".tmp"
            final_tmp_name = os.path.join(comm_data.host_reg_dst_path, tmp_name)
            final_name = os.path.join(comm_data.host_reg_dst_path, file_name)

                #2. 将转换后的文件直接放入 comm_data.host_reg_dst_path 中去
            file = open(final_tmp_name, 'w')
            file.write(json_context)
            file.close()
            os.rename(final_tmp_name, final_name)

            #3. 删除该文件
            os.remove(reg_file)
            print "===== reg APPLY ===== move (dbase)[%s] -> (json)[%s]" %(reg_file, final_name)
            host_cloud_logger.debug("===== reg APPLY ===== move (dbase)[%s] -> (json)[%s]" %(reg_file, final_name))
        time.sleep(0.5)
    else:
        print("REG INLINE EXIT!")
        host_cloud_logger.debug("REG INLINE EXIT!")

def inline_topo_down_busi():
    global local_uuid
    print "-------------------- Begin  MTX INLINE DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin MTX INLINE DOWN BUSINESS ------")
    while True:
        # 1. 在二合一模式下，直接解压本机的 mtx 压缩包到博雅目录
        for up_file in comm_common.file_filter_bysuffix([comm_data.comm_topo_up_src_path], ['.xml']):
            if os.path.exists(up_file):
                #1. 得到文件 直接解压到北大博雅目录下
                try:
                    dst_path = comm_data.host_topo_dst_path+local_uuid
                    comm_common.creat_if_dir_notexist(dst_path)
                    okfile_safe_copy(up_file,dst_path)
                except Exception, ex:
                    print("==copy Excepiton==>file[%s] DELETE IT!" % up_file)
                    host_cloud_logger.debug("==copy Excepiton==>file[%s] DELETE IT!" % up_file)
                    print ex
                    host_cloud_logger.debug("[%s]" %ex)
                else:
                    print("LOCOL[%s] -> PATH[%s] SUCC!" %(up_file, comm_data.host_mtx_dst_path))
                    host_cloud_logger.debug("LOCOL[%s] -> PATH[%s] SUCC!" %(up_file, comm_data.host_mtx_dst_path))
                finally:
                    os.remove(up_file)
            time.sleep(0.5)
    else:
        print(" INLINE EXIT!")
#内网模式下进行解压缩的处理
def inline_down_busi(comm_ali_path,host_dst_path):
    print "-------------------- Begin  MTX INLINE DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin MTX INLINE DOWN BUSINESS ------")
    while True:
        # 1. 在二合一模式下，直接解压本机的 mtx 压缩包到博雅目录
        for up_file in comm_common.file_filter_bysuffix([comm_ali_path], ['.ok']):
            if os.path.exists(up_file):
                #1. 得到文件 直接解压到北大博雅目录下
                try:
                    comm_common.extractzip(up_file, host_dst_path,passwd='9527')
                except Exception, ex:
                    print("==extractzip Excepiton==>file[%s] DELETE IT!" % up_file)
                    host_cloud_logger.debug("==extractzip Excepiton==>file[%s] DELETE IT!" % up_file)
                    print ex
                    host_cloud_logger.debug("[%s]" %ex)
                else:
                    print("LOCOL[%s] -> PATH[%s] SUCC!" %(up_file, comm_data.host_mtx_dst_path))
                    host_cloud_logger.debug("LOCOL[%s] -> PATH[%s] SUCC!" %(up_file, comm_data.host_mtx_dst_path))
                finally:
                    os.remove(up_file)
            time.sleep(0.5)
    else:
        print(" INLINE EXIT!")
        host_cloud_logger.debug(" INLINE EXIT!")
#mtx内网事件
def inline_mtx_down_busi():
    inline_mtx_path = comm_data.comm_me_ali_up_path
    mtx_dst_path = comm_data.host_mtx_dst_path
    inline_down_busi(inline_mtx_path,mtx_dst_path)

#surl内网事件
def inline_surl_down_busi():
    inline_surl_path = comm_data.comm_surl_ali_path
    surl_dst_path = comm_data.host_surl_dst_path
    if comm_data.surl_isupload == 'no':
        inline_delete_event_busi(inline_surl_path) 
    else:
        inline_down_busi(inline_surl_path,surl_dst_path)

#stat内网事件
def inline_stat_down_busi():
    inline_stat_path = comm_data.comm_devstat_ali_path
    stat_dst_path = comm_data.host_stat_dst_path
    if comm_data.stat_isupload == 'no':
        inline_delete_event_busi(inline_stat_path)
    else:
        inline_down_busi(inline_stat_path,stat_dst_path)

#abb内网事件
def inline_abb_down_busi():
    inline_abb_path = comm_data.comm_ae_ali_path
    abb_dst_path = comm_data.host_abb_dst_path
    if comm_data.abb_isupload == 'no':
        inline_delete_event_busi(inline_abb_path)
    else:
        inline_down_busi(inline_abb_path,abb_dst_path)

#flow内网事件
def inline_flow_down_busi():
    inline_flow_path = comm_data.comm_flow_ali_path
    flow_dst_path = comm_data.host_flow_dst_path
    if comm_data.flow_isupload == 'no':
        inline_delete_event_busi(inline_flow_path)
    else:
        inline_down_busi(inline_flow_path,flow_dst_path)

#virus内网事件
def inline_virus_down_busi():
    inline_virus_path = comm_data.comm_ve_ali_path
    virus_dst_path = comm_data.host_virus_dst_path
    if comm_data.virus_isupload == 'no':
        inline_delete_event_busi(inline_virus_path)
    else:
        inline_down_busi(inline_virus_path,virus_dst_path)

def inline_remote_flow_busi():
    inline_remote_flow_path = comm_data.comm_query_flow_ali_path
    remote_flow_dst_path = comm_data.host_query_flow_dst_path
    inline_down_busi(inline_remote_flow_path,remote_flow_dst_path)

def inline_remote_dns_busi():
    inline_remote_dns_path = comm_data.comm_query_dns_ali_path
    remote_dns_dst_path = comm_data.host_query_dns_dst_path
    inline_down_busi(inline_remote_dns_path,remote_dns_dst_path)

def inline_remote_url_busi():
    inline_remote_url_path = comm_data.comm_query_url_ali_path
    remote_url_dst_path = comm_data.host_query_url_dst_path
    inline_down_busi(inline_remote_url_path,remote_url_dst_path)

#reg事件外网模式下的下载处理
def cloud_reg_down_busi():
    #global host_cloud_logger
    print "-------------------- Begin REG DOWN BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REG DOWN BUSINESS ------")

    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_UP_BUCKET
    #进入上传循环业务
    while True:
        # 1. 读取云库中目前都有哪些文件
        #res = oss.list_all_my_buckets()
        delimiter = "/"
        buf = handle_bucket(oss,bucket,delimiter)
        if buf==-1:
            continue

        tmp_file = '/tmp/ali_reg_up_filelist.xml'
        try:
            write_buf_to_file(buf, tmp_file)
        except Exception,ex:
            print ex
            host_cloud_logger.debug("[%s]" %ex)
            continue
        #取到云端的顶级结点目录
        dom = xml.dom.minidom.parse(tmp_file)
        root = dom.documentElement
        reg_list = root.getElementsByTagName('CommonPrefixes')
        for topid in reg_list:
            keynode = topid.getElementsByTagName("Prefix")[0]
            pre_uuid = keynode.childNodes[0].nodeValue
            prefix_path = pre_uuid +"reg/"
            buf = handle_bucket(oss,bucket,prefix_path)
            if buf==-1:
                continue
        
            # 2. 将得到的xml文件存在本地
            try:
                write_buf_to_file(buf, tmp_file)
            except Exception,ex:
                print ex
                host_cloud_logger.debug("[%s]" %ex)
                continue
        

            # --> 从阿里云中下载文件
            reg_fname_list = get_cloud_xml_fname(tmp_file, comm_data.REG_FILE)
            dst_path = comm_data.host_reg_dst_path+"temp/"
            cloud_down_file(oss, bucket, reg_fname_list,dst_path)
            os.remove(tmp_file)
            for reg_file in comm_common.file_filter_bysuffix([dst_path],[".ok"]):
                file = open(reg_file,"r")
                read_str = file.readline()
                item = read_str.split('|')
                for i in range(1,8):
                    if item[i] == '\N':
                        item[i] = ''

                write_str = "[{"+'"deviceName"'+":"+'"'+item[0]+'"'+\
                    ',"pid"'+":"+'"'+item[1]+'"'+',"uuid"'+":"+'"'\
                    +item[2]+'"'+',"ip"'+":"+'"'+item[3]+'"'+\
                    ',"customName"'+":"+'"'+item[4]+'"'+',"deviceType"'\
                    +":"+'"'+item[5]+'"'+',"ParentId"'+":"+'"'+item[6]+\
                    '"'+',"id"'+":"+'"'+item[7]+'"'+"}]"
                #更新 uuid 对应 topid 拓扑关系表   
                cloudid_topo.update_topid_uuid_topo(item[2], item[1]) 
                    
                file.close()
                dst_down_file = reg_file.split('/')[-1]
                os.remove(reg_file)
                dst_file = open(comm_data.host_reg_dst_path+dst_down_file,"w")
                dst_file.write(write_str)
                dst_file.close()
        
        time.sleep(2)
    else:
        print("EXIT!")
        host_cloud_logger.debug("EXIT")

def cloud_reg_up_busi():
    global host_cloud_logger
    global local_uuid
    print "-------------------- Begin REG UP BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin REG UP BUSINESS ------")
    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET

    #进入上传循环业务
    while True:
        # --> 设备注册
        src_path = comm_data.host_reg_src_path
        dst_path = "/" + comm_data.REG_FILE + "/"
        for reg_file in comm_common.file_filter_bysuffix([src_path], ['.ok']):

            up_fl = open(reg_file, "r")
            tran_file = json.load(up_fl)
            up_fl.close()
            i=0

            for tran_file[i] in tran_file:
                reg_json_null_to_N(tran_file[i])
                dbase_context = tran_file[i]["deviceName"]+"|"+tran_file[i]["pid"]+"|"+tran_file[i]["uuid"]+"|"+tran_file[i]["ip"]+"|"+tran_file[i]["customName"]+"|"+tran_file[i]["deviceType"]+"|"+str(tran_file[i]["ParentId"])+"|"+str(tran_file[i]["id"])

                #分配的注册ID返回码
                cloudid = tran_file[i]["id"]
                #原设备UUID
                uuid = tran_file[i]["uuid"]
                #原设备父节点UUID
                pid = tran_file[i]["pid"]

                if (cloudid == ""):
                    if (pid == '\N'):
                        pid = ''
                    # 注册失败
                    # 1. 得到topid
                    if(pid == ""):
                        # 顶级设备
                        topid = uuid
                    else:
                        # 非顶级设备
                        topid = cloudid_topo.get_topid_by_uuid(pid)
                        if (topid == "no_value"):
                            print "********* file [%s]'s pid[%s] not in topo, can not find *****" %(reg_file, pid)
                            host_cloud_logger.debug("********* file [%s]'s pid[%s] not in topo, can not find *****"\
                                                     %(reg_file, pid))
                            continue
                else:
                    # 注册成功
                    # 1. 根据cloudid, uuid, pid 建立拓扑结构
                    #一个目录下，手动移动到另一个目录下，然后生成id 建立拓扑结构。否则me取值为空
                    print "cloudid:[%s], uuid[%s], pid[%s]" %(cloudid, uuid, pid)
                    host_cloud_logger.debug("cloudid:[%s],uuid[%s],pid[%s]" %(cloudid,uuid,pid))
                    if (pid == '\N'):
                        pid = ''
                    try:
                        cloudid_topo.update_cloudid_uuid_topo(cloudid, uuid, pid)
                    except Exception, ex:
                        print ex
                        host_cloud_logger.debug("[%s]" %ex)
                        continue

                    # 2 .得到topid
                    if(pid == ""):
                        # 顶级设备
                        topid = uuid
                    else:
                        # 非顶级设备
                        topid = cloudid_topo.get_topid_by_uuid(uuid)

                print "GET TOPID = %s" %(topid)

                reg_file_name = "reg_"+uuid+".ok"

                if (topid == local_uuid):
                    # 国家内网的注册文件 需要给国家内网
                    final_file = os.path.join(comm_data.comm_reg_down_path, reg_file_name)
                    final_tmp_file = final_file + ".tmp"

                    #3.将转换成数据库格式的文件 直接拷贝到 comm_data.comm_reg_down_path中
                    file = open(final_tmp_file, 'w')
                    file.write(dbase_context.encode('utf8'))
                    file.close()
                    os.rename(final_tmp_file, final_file)
                    #4.删除该文件
                    os.remove(reg_file)
                    print "======= reg RESULT ======= move (json)[%s] -> (dbase)[%s]" %(reg_file, final_file)
                    host_cloud_logger.debug("======= reg RESULT ======= move (json)[%s] -> (dbase)[%s]" %(reg_file, final_file))
                else:
                    # 外网注册文件 需要上传到阿里云
                    dst_file = topid + "/" + comm_data.REG_FILE + "/" + reg_file_name
                    write_up_file = open(reg_file_name,"w")
                    write_up_file.write(dbase_context.encode('utf8'))
                    write_up_file.close()

                    retn = cloud_up_file(oss,bucket, reg_file_name, dst_file)
                    if retn == 0:
                        #上传成功，删除本地文件
                        os.remove(reg_file_name)
                        os.remove(reg_file)
                    else:
                        os.remove(reg_file_name)
                        os.rename(reg_file,reg_file+'.bak')
                        #上传失败，退出循环
                        break
                i+=1
        #for done
        time.sleep(0.5)
    else:
        print("EXIT!")
        host_cloud_debug("EXIT!")

#远程查询命令下行处理线程
def remote_query_up_busi():
    print "-------------------- Begin QUERY UP BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin QUERY UP BUSINESS ------")
    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET

    query_flow_dst_path = comm_data.QUERY_FLOW_FILE
    query_url_dst_path = comm_data.QUERY_URL_FILE 
    query_dns_dst_path = comm_data.QUERY_DNS_FILE

    dir_list = ((comm_data.host_query_flow_src_path , query_flow_dst_path),     
                (comm_data.host_query_url_src_path, query_url_dst_path),       
                (comm_data.host_query_dns_src_path, query_dns_dst_path),    
                )
    #进入上传循环业务
    while True:
        for src_dir,dst_dir in dir_list: 
            for up_file in comm_common.file_filter_bysuffix([src_dir], ['.ok']):
                #通过云topo 得到该文件的顶级设备uuid 和 文件源设备的uuid
                oldname = os.path.basename(up_file)
                #文件名 oldname = 2014010203123301_uuid.ok
                name = os.path.splitext(oldname)[0]
                item = name.split("_")
                uuid = item[1]
                topuuid = cloudid_topo.get_topid_by_uuid(int(uuid))
                if (topuuid == "no_value"):
                    #没有查询到此云ID对应的拓扑结构
                    print "************ file[%s]'s cloudid[%s] is not in topo. Can not find. DELETE IT!!!*********"\
                        %(up_file, uuid)
                    host_cloud_logger.debug("************ file[%s]'s cloudid[%s] is not in topo."\
                        " Can not find. DELETE IT!!!*********" %(up_file, uuid))
                    os.remove(up_file)
                    continue
                
                zip_up_file = item[0]+"_"+uuid+".ok"
                comm_common.createzip(zip_up_file,up_file)
                if topuuid == local_uuid:
                    if comm_data.QUERY_FLOW_FILE in dst_dir:
                        dst_dir = comm_data.comm_query_flow_down_path
                    if comm_data.QUERY_URL_FILE in dst_dir:
                        dst_dir = comm_data.comm_query_url_down_path
                    if comm_data.QUERY_DNS_FILE in dst_dir:
                        dst_dir = comm_data.comm_query_dns_down_path
                    retn = okfile_safe_copy(zip_up_file,dst_dir)
                else:
                    dst_file = topuuid +"/"+dst_dir+"/"+zip_up_file
                    retn = cloud_up_file(oss,bucket,zip_up_file,dst_file)
                try:
                    os.remove(zip_up_file)
                except Exception,ex:
                    host_cloud_logger.debug(ex)
                if retn == 0:
                    #上传成功，删除本地文件
                    os.remove(up_file)
        time.sleep(0.1)
    else:
        host_cloud_logger.debug("EXIT!")

#-----------------------------------------.
# MTX 上报事件下行处理线程                |
#-----------------------------------------*
def cloud_mtx_up_busi():
    global local_uuid
    global host_cloud_logger
    print "-------------------- Begin MTX UP BUSINESS -------------"
    host_cloud_logger.debug("-------- Begin MTX UP BUSINESS ------")
    #与阿里云 oss 服务建立连接
    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
    bucket = comm_data.OSS_DOWN_BUCKET
    #进入上传循环业务
    while True:
        src_path = comm_data.host_mtx_src_path
        dst_path = "/" + comm_data.MTX_FILE + "/"
        for up_file in comm_common.file_filter_bysuffix([src_path], ['.ok']):
            #通过云topo 得到该文件的顶级设备uuid 和 文件源设备的uuid
            oldname = os.path.basename(up_file)
            print oldname
            #文件名 oldname = 2014010203123301_cloudid.ok
            name = os.path.splitext(oldname)[0]
            item = name.split("_")
            cloudid = item[1]
            topuuid_uuid = cloudid_topo.get_cloud_topo(cloudid)
            if (topuuid_uuid == "no_value"):
                #没有查询到此云ID对应的拓扑结构
                print "************ file[%s]'s cloudid[%s] is not in topo. Can not find. DELETE IT!!!*********"\
                        %(up_file, cloudid)
                host_cloud_logger.debug("************ file[%s]'s cloudid[%s] is not in topo."\
                        " Can not find. DELETE IT!!!*********" %(up_file, cloudid))
                os.remove(up_file)
                continue
            topuuid = topuuid_uuid.split("_")[0]
            uuid = topuuid_uuid.split("_")[1]

            zip_up_file = item[0]+"_"+uuid+".ok"
            new_up_file = src_path + zip_up_file
            os.rename(up_file, new_up_file)
            
            print "Begin ZIP...size %s" %(os.path.getsize(new_up_file))
            comm_common.createzip(zip_up_file, new_up_file,passwd='9527')
            print "END ZIP...size %s" %(os.path.getsize(zip_up_file)) 
            if topuuid == local_uuid:
                retn = okfile_safe_copy(zip_up_file, comm_data.comm_me_ali_down_path)
            else:
                dst_file = topuuid + dst_path + zip_up_file
                retn = cloud_up_file(oss,bucket,zip_up_file,dst_file)
            try: 
                os.remove(zip_up_file)
            except Exception, ex:
                print ex
                host_cloud_logger.debug("[%s]" %ex)
            if retn == 0:
                # 2.上传成功，删除本地文件
                os.remove(new_up_file)
            else:
                os.rename(new_up_file,up_file)
        #for done
        time.sleep(0.5)
    else:
        print("EXIT!")
        host_cloud_logger.debug("EXIT!")

def inline_delete_event_busi(del_dir):
    while True:
        for del_file in file_up_down_tran.file_filter_bysuffix([del_dir], ['.ok']):
            try:
                os.remove(del_file)
                print "delete [%s] is success!!!!!!!!!!!!!" %del_file
                host_cloud_logger.debug("delete [%s] is success!!!!!!!!!!!!!!" %del_file)
            except Exception,ex:
                print ex
                host_cloud_logger.debug(ex)
                print "delete [%s] is fail!!!!!!!!!!!!!" %del_file
                host_cloud_logger.debug("delete [%s] is fail!!!!!!!!!!!!!!" %del_file)
                continue
        time.sleep(0.5)


#各种事件开始线程处理
def try_catch_busi(event_busi):
    try:
        thread = threading.Thread(target=event_busi,args=())
        thread.setDaemon(True)
        thread.start()
    except Exception, ex:
        print ex
        host_cloud_logger.debug("[%s]" %ex)

def main():

    global host_cloud_logger

    #读取配置文件
    comm_init.read_pre_json()

    #初始化日志
    host_cloud_log_init()

    # 读取配置文件
    read_interface_xml()
    # 创建目录
    create_dir()

    #加载云端拓扑结构
    cloudid_topo.load_topo_dist()

    #开启远程查询flow事件下载线程
    try_catch_busi(cloud_remote_flow_down_busi)

    #开启远程查询dns事件下载线程
    try_catch_busi(cloud_remote_dns_down_busi)

    #开启远程查询url事件下载线程
    try_catch_busi(cloud_remote_url_down_busi)

    #开启远程查询事件上传线程
    try_catch_busi(remote_query_up_busi)

    #开启mtx事件上传线程
    try_catch_busi(cloud_mtx_up_busi)

    #开启mtx规则事件上传线程
    try_catch_busi(rule_event_busi)
    
    #开启mtx事件下载线程
    try_catch_busi(cloud_mtx_down_busi)

    #开启reg注册上传线程
    try_catch_busi(cloud_reg_up_busi)

    #开启reg注册下载线程
    try_catch_busi(cloud_reg_down_busi)

    #开启 规则库，配置，升级包上传线程
    try_catch_busi(cloud_conf_up_busi)

    #开启 abb事件 下载线程
    try_catch_busi(cloud_abb_down_busi)
    
    #开启 virus事件 下载线程
    try_catch_busi(cloud_virus_down_busi)

    #开启 flow事件 下载线程
    try_catch_busi(cloud_flow_down_busi)

    #开启 surl事件 下载线程
    try_catch_busi(cloud_surl_down_busi)

    #开启 stat事件 下载线程
    try_catch_busi(cloud_stat_down_busi)
    
    #开启 内网reg事件 下载线程
    try_catch_busi(inline_reg_down_busi)

    #开启 内网mtx事件 下载线程
    try_catch_busi(inline_mtx_down_busi)

    #开启 内网surl事件 下载线程
    try_catch_busi(inline_surl_down_busi)

    #开启 内网stat事件 下载线程
    try_catch_busi(inline_stat_down_busi)

    #开启 内网abb事件 下载线程
    try_catch_busi(inline_abb_down_busi)

    #开启 内网flow事件 下载线程
    try_catch_busi(inline_flow_down_busi)

    #开启 内网virus事件 下载线程
    try_catch_busi(inline_virus_down_busi)

    #开启 内网topo事件 下载线程
    try_catch_busi(inline_topo_down_busi)

    #开启 外网 topo事件 下载线程
    try_catch_busi(cloud_topo_down_busi)

def host_cloud_log_init():
    global host_cloud_logger
    host_cloud_logger = cloud_log.CloudLogger(path = comm_data.cloud_log_path,
                        level = '',
                        format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')

if __name__=="__main__":
    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)
    main()
    while (True):
        time.sleep(3)
##################file end##################
