# -*- coding:utf-8 -*-
import comm_data
import comm_conn_manage
import comm_cmd_proc
import comm_log_record
import xml_file
import shutil
import threading
import os
from xml.etree.ElementTree import ElementTree,Element
import pickle

comm_topo_dist = {} #通讯内部的topo结构
topo_mutex = threading.Lock()   #拓扑数据结构锁
#M1->M2->M3->C

tree = ElementTree()
root = []
tmp_file = "./conf/tmp_topo.xml"

#用户侧内部的拓扑关系
# M3 存放 拓扑为 {'c的uuid':'c的ip地址'} M2存放的拓扑 {'c的uuid':'M3的ip'}
# M1 存放的拓扑为 {'c的uuid':'M2的ip地址'}

#国家主机的拓扑关系：
# M1 : {'M1的云id':'M1的uuid'}
# M2 : {'M2的云id':'M1的uuid-M2的uuid'}
# M3 : {'M3的云id':'M1的uuid-M2的uuid-M3的uuid'}
# c :  {'c的云id':'M1的uuid-M2的uuid-M3的uuid-c的uuid'}

###################函数说明#########################
#读取topo文件，初始化已经生成的topo
#参数说明:
    #filepath:topo文件路径
###################函数说明#########################
def topo_file_init():
    global root
    if os.path.exists(comm_data.topo_file)==True:
        root = xml_file.read_xml(comm_data.topo_file, tree)
        comm_log_record.logger.info(root)
    else:
        return
    node_findall = tree.findall("uuid_ip")
    for node in node_findall:
        for child in node.getchildren():
            if (child.tag == "uuid"):
                uuid = child.text
            elif (child.tag == "ip"):
                ip = child.text
        tmp_dist = {uuid:ip}
        comm_topo_dist.update(tmp_dist)

def load_topo_dist():
    global comm_topo_dist
    if (os.path.exists(comm_data.topo_file) != True):
        fp = open(comm_data.topo_file, 'w')
        fp.close()

    comm_topo_dist = streload(comm_data.topo_file)
    if (comm_topo_dist == "no_value"):
        comm_topo_dist = {}
        stdump(comm_topo_dist, comm_data.topo_file)

    comm_log_record.logger.info('topo info: %s' % comm_topo_dist)

###################函数说明#########################
#建立通讯内部topo,
#参数说明:
    #uuid:注册设备的uuid，
    #ip 上传注册命令的设备ip
###################函数说明#########################
def creat_comm_topo(uuid,ip,para):
    global root
    value = ip + "#" + para
    tmp_dist = {uuid:value}
    # 猜这个 root 大概已经没用了
    #comm_log_record.logger.info('root: %s' % root)
    if topo_mutex.acquire(1):
        comm_topo_dist.update(tmp_dist)
        comm_log_record.logger.info('update topo dict: %s' % comm_topo_dist)
        #更新topo文件 增加一个新节点
        update_topo_file()
        topo_mutex.release()

###################函数说明#########################
#功能说明：通讯内部topo ip获取
#参数说明：uuid ：下发数据对应的最终设备的id
#返回值：返回本设备发往子设备的ip
###################函数说明#########################
def get_comm_topo_ip(uuid):

    if topo_mutex.acquire(1):
        try:
            value = comm_topo_dist[uuid]
            ip = value.split("#")[0]
        except Exception,ex:
            ip = "no_value"
    topo_mutex.release()
    return ip

def get_comm_topo_regpara(uuid):

    if topo_mutex.acquire(1):
        try:
            value = comm_topo_dist[uuid]
            para = value.split("#")[1]
        except Exception,ex:
            para = "no_value"
    topo_mutex.release()
    return para

def get_all_sub_uuid():
    global comm_topo_dist
    if topo_mutex.acquire(1):
        uuid_list = comm_topo_dist.keys()
    topo_mutex.release()
    return uuid_list


###################函数说明#########################
#功能说明：通讯内部topo ip删除,当收到注册失败命令时调用
#参数说明：uuid ：最终设备的id
###################函数说明#########################
def del_comm_topo_ip(uuid):
    if topo_mutex.acquire(1):
        del comm_topo_dist[uuid]
        node_findall = root.findall("uuid_ip")
        for node in node_findall:
            for child in node.getchildren():
                if (child.tag == "uuid"):
                    if child.text == uuid:
                        root.remove(node)
        xml_file.write_xml(tree, tmp_file)
        shutil.move(tmp_file, comm_data.topo_file)
        topo_mutex.release()

def stdump(st, fname):
    fp = open(fname, 'w')
    pickle.dump(st, fp)
    fp.close()

def streload(fname):

    fp = open(fname, 'r')
    try:
        st = pickle.load(fp)
    except Exception as reason:
        comm_log_record.logger.info(reason)
        fp.close()
        return "no_value"

    fp.close()
    return st

###################函数说明#########################
#功能说明：根据内存建立的topo结构，存入文件中
###################函数说明#########################
def update_topo_file():
    global comm_topo_dist
    stdump(comm_topo_dist, comm_data.topo_file)

#def main():
 #   topo_file_init()
  #  print "begin"
   # creat_comm_topo("aaaa11111111", "bbbb2222222")
    #print "end"

#if __name__ == "__main__":
 #   main()
