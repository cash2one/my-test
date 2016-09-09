# -*- coding:utf-8 -*-
import xml_file
import shutil
import threading
import os
import pickle
from xml.etree.ElementTree import ElementTree,Element
cloudid_topo_dist = {} # 国家主机云的内部id跟通讯内部id的对应关系结构
cloud_uuid_topo_dist = {} # 国家主机上的子设备与顶级节点对应关系，用于查找顶级节点用

#M1->M2->M3->C

#用户侧内部的拓扑关系
# M3 存放 拓扑为 {'c的uuid':'c的ip地址'} M2存放的拓扑 {'c的uuid':'M3的ip'}
# M1 存放的拓扑为 {'c的uuid':'M2的ip地址'}

#国家主机的拓扑关系：
# M1 : {'M1的云id':'M1的uuid'}
# M2 : {'M2的云id':'M1的uuid-M2的uuid'}
# M3 : {'M3的云id':'M1的uuid-M3的uuid'}
# c :  {'c的云id':'M1的uuid-c的uuid'}


cloudtopo_cache_path = "/gms/comm/cloud/conf/cloud_topo.cache"
topid_cache_path = "/gms/comm/cloud/conf/topid.cache"


###################函数说明#########################
#功能说明：建立云id与内部id的转换topo, 通过父id可以查找对应的顶级设备的id
#内部需要维护一个内部uuid对应的顶级节点的id
#比如拓扑关系 M1-M2-M3-c, {"M1的uuid":"M1的uuid", "M2的uuid":"M1的uuid", "M3的uuid":"M1的uuid", "C的uuid":"M1的uuid"}

#参数说明：
    #cloudid:云平台内部id
    #uuid：注册设备自身的id
    #pid：注册设备的父id
###################函数说明#########################
def creat_cloud_topo(cloudid, uuid, pid):
    if pid == '': #父id为空说明是顶级设备
        tmp_id_va = uuid + '_' + uuid   #拼接数据 格式为 顶级设备uuid_本设备的uuid 
        top_uuid = uuid
        tmp_cloudid_dist = {cloudid:tmp_id_va}
        tmp_uuid_dist = {uuid:uuid}
        cloudid_topo_dist.update(tmp_cloudid_dist)
        cloud_uuid_topo_dist.update(tmp_uuid_dist)
    else:
        top_uuid = cloud_uuid_topo_dist[pid] #首先通过pid 查找对应的顶级节点id
        tmp_id_va = top_uuid + '_' + uuid   #拼接数据 格式为 顶级设备uuid_本设备的uuid 
        tmp_cloudid_dist = {cloudid:tmp_id_va}
        tmp_uuid_dist = {uuid:top_uuid}
        cloudid_topo_dist.update(tmp_cloudid_dist)
        cloud_uuid_topo_dist.update(tmp_uuid_dist)

    update_topo_file()

# -------------------------------------------
##
# @brief  更新或插入 uuid 对应的 topid 拓扑
#
# @returns   
# -------------------------------------------
def update_topid_uuid_topo(uuid, pid):
    global cloud_uuid_topo_dist
    global cloudid_topo_dist
    if pid == '': #父id为空说明是顶级设备
        tmp_uuid_dist = {uuid:uuid}
        cloud_uuid_topo_dist.update(tmp_uuid_dist)
    else:
        top_uuid = cloud_uuid_topo_dist[pid] #首先通过pid 查找对应的顶级节点id
        tmp_uuid_dist = {uuid:top_uuid}
        cloud_uuid_topo_dist.update(tmp_uuid_dist)

    update_topo_file()

# -------------------------------------------
##
# @brief  更新或插入 cloudid 对应的uuid 拓扑
#
# @returns   
# -------------------------------------------
def update_cloudid_uuid_topo(cloudid, uuid, pid):
    global cloud_uuid_topo_dist
    global cloudid_topo_dist
    if pid == '': #父id为空说明是顶级设备
        tmp_id_va = uuid + '_' + uuid   #拼接数据 格式为 顶级设备uuid_本设备的uuid 
        tmp_cloudid_dist = {cloudid:tmp_id_va}
        cloudid_topo_dist.update(tmp_cloudid_dist)
    else:
        top_uuid = cloud_uuid_topo_dist[pid] #首先通过pid 查找对应的顶级节点id
        tmp_id_va = top_uuid + '_' + uuid   #拼接数据 格式为 顶级设备uuid_本设备的uuid 
        tmp_cloudid_dist = {cloudid:tmp_id_va}
        cloudid_topo_dist.update(tmp_cloudid_dist)

    update_topo_file()
 
###################函数说明#########################
#功能说明：通过云内部的id查找其对应的设备uuid以及顶级节点的uuid
#参数说明：
    #cloudid: 云内部id
#返回值：顶级节点uuid_本设备的id
###################函数说明#########################
def get_cloud_topo(cloudid):
    try:
        return cloudid_topo_dist[cloudid]
    except Exception as reason:
        print reason
        return 'no_value'

###################函数说明#########################
#功能说明：通过父ｉｄ　获取顶级设备的ｉｄ
#参数说明：
    #pid: 设备对应的父id
#返回值：顶级节点uuid
###################函数说明#########################
def get_topid_by_uuid(pid):
    try:
        return cloud_uuid_topo_dist[pid]
    except Exception as reason:
        print reason
        return 'no_value'

def stdump(st, fname):
    fp = open(fname, 'w')
    pickle.dump(st, fp)
    fp.close()

def streload(fname):
    fp = open(fname, 'r')
    try:
        st = pickle.load(fp)
    except Exception as reason:    
        print reason
        fp.close()
        return "no_value"

    fp.close()
    return st


###################函数说明#########################
#功能说明：加载本地拓扑文件 , 如果没有文件则创建
###################函数说明#########################
def load_topo_dist():
    global cloudid_topo_dist
    global cloud_uuid_topo_dist


    if (os.path.exists(cloudtopo_cache_path) != True):
        fp = open(cloudtopo_cache_path, 'w')
        fp.close()
    if (os.path.exists(topid_cache_path) != True):
        fp = open(topid_cache_path, 'w')
        fp.close()

    cloudid_topo_dist = streload(cloudtopo_cache_path)    
    cloud_uuid_topo_dist = streload(topid_cache_path)
    if (cloudid_topo_dist == "no_value"):
        cloudid_topo_dist = {}
        stdump(cloudid_topo_dist, cloudtopo_cache_path)
    if (cloud_uuid_topo_dist == "no_value"):
        cloud_uuid_topo_dist = {}
        stdump(cloud_uuid_topo_dist, topid_cache_path)

###################函数说明#########################
#功能说明：将内存的拓扑写到文件中去
###################函数说明#########################
def update_topo_file():
    global cloudid_topo_dist
    global cloud_uuid_topo_dist
    stdump(cloudid_topo_dist, cloudtopo_cache_path)
    stdump(cloud_uuid_topo_dist, topid_cache_path)

def get_all_uuid_list():
    global cloud_uuid_topo_dist
    return cloud_uuid_topo_dist.keys() 

def main():
    #topo_file_init()
    global cloudid_topo_dist
    global cloud_uuid_topo_dist
    load_topo_dist()
    #creat_cloud_topo("cloud1", "uuid1", "")
    #creat_cloud_topo("cloud2", "uuid2", "uuid1")
    #creat_cloud_topo("cloud3", "uuid3", "uuid2")
    #creat_cloud_topo("cloud4", "uuid4", "uuid2")

    uuidlist = get_all_uuid_list()
    print "===cloudid_topo_dist is ==="
    print cloudid_topo_dist
    print "===cloud_uuid_topo_dist is ==="
    print cloud_uuid_topo_dist
    print "===uuidlist is ==="
    print uuidlist
    for uuid in uuidlist:
        print uuid

if __name__ == "__main__":
    main()
