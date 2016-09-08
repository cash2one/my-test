# -*- coding:utf-8 -*-
from xml.etree import ElementTree
import os
import json
import comm_data
import comm_conn_manage
import comm_log_record
import device_manage

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
pre_conf_file = os.path.abspath("/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json")

#连接初始化
def connect_init():
    #首先建立监听socket
    comm_conn_manage.creat_listen_socket()
    #如果是管理节点，需要跟下级节点建立命令通道
    comm_conn_manage.creat_command_channel()

#配置初始化
def conf_init():
    #comm所有配置文件初始化
    read_pre_json()                     # 这里读入日志目录
    comm_log_record.record_log_init()   # 初始化 logger
    comm_log_record.logger.info("comm_main start.")
    comm_log_record.logger.info("--------------------- config ---------------------")
    read_interface_xml()
    read_ftp_conf()

    print_init_info()
    comm_log_record.logger.info("--------------------------------------------------")

    #device_manage.topo_file_init()
    device_manage.load_topo_dist()

def read_interface_xml():
    global interface_conf_file
    global pre_conf_file
    #sub_dev = comm_data.sub_dev_info
    root = ElementTree.parse(interface_conf_file)
    lst_node = root.getiterator("cominfo")

    #读取本设备信息,判断是什么类型的设备
    devtype_node = root.find("selfinfo/devtype/manager")
    comm_data.ismanager = devtype_node.text

    devtype_node = root.find("selfinfo/devtype/monitor")
    comm_data.ismonitor = devtype_node.text

    devtype_node = root.find("selfinfo/devtype/root")
    comm_data.isroot = devtype_node.text

    devtype_node = root.find("selfinfo/devtype/cloudcon");
    comm_data.iscloudcon = devtype_node.text

    devtype_node = root.find("selfinfo/ip")
    comm_data.dev_ip = devtype_node.text

    devtype_node = root.find("selfinfo/uuid")
    comm_data.uuid = devtype_node.text

    devtype_node = root.find("selfinfo/devname")
    comm_data.dev_name = devtype_node.text

    devtype_node = root.find("selfinfo/comm_cmd_port")
    comm_data.comm_cmd_port = int(devtype_node.text)

    devtype_node = root.find("selfinfo/comm_file_port")
    comm_data.comm_file_port = int(devtype_node.text)

    comm_log_record.logger.info("file up port [%s]" % comm_data.comm_cmd_port)
    comm_log_record.logger.info("file down port [%s]" % comm_data.comm_file_port)
    comm_data.topo.insert(0, comm_data.dev_ip + ':' + comm_data.uuid) # 0索引留给自己

    comm_log_record.logger.info("dev type: root[%s] manager[%s] monitor[%s]" % (comm_data.isroot, comm_data.ismanager, comm_data.ismonitor))

    #读取子设备的信息
    node_findall = root.findall("subdev/devinfo")
    for node in node_findall:
        sub_dev = comm_data.sub_dev_info()
        for child in node.getchildren():
             if (child.tag == "ip"):
                sub_dev.ip = child.text
                comm_log_record.logger.info("sub dev ip: %s" % sub_dev.ip)
             if (child.tag == "uuid"):
                sub_dev.uuid = child.text
                comm_log_record.logger.info("sub dev uuid: %s" % sub_dev.uuid)
             if (child.tag == "comm_cmd_port"):
                sub_dev.comm_cmd_port = int(child.text)
                comm_log_record.logger.info("sub dev up port: %s" % sub_dev.comm_cmd_port)
             if (child.tag == "comm_file_port"):
                sub_dev.comm_file_port = int(child.text)
                comm_log_record.logger.info("sub dev down port: %s" % sub_dev.comm_file_port)
        comm_data.sub_dev_list.append(sub_dev)

def read_pre_json():
    with open(pre_conf_file) as fp:
        data = fp.read()
        kwdict = json.loads(data)
        for key, value in kwdict.items():
            if key == 'iscopytodb':
                if value.upper() == 'YES':
                    value = True
                else:
                    value = False
            setattr(comm_data, key, value)

def read_ftp_conf():
    import traceback
    import ConfigParser
    try:
        cf = ConfigParser.ConfigParser()
        cf.read(comm_data.ftp_conf_file)
        flag = cf.get('ftp_server', 'enable')
        if flag.upper() == 'YES':
            comm_data.iscopytoftp = True
        else:
            comm_data.iscopytoftp = False
    except:
        comm_data.iscopytoftp = False

def print_init_info():
    for key in dir(comm_data):
        if key.find('_path', -5) != -1:
            path = getattr(comm_data, key)
            comm_log_record.logger.info("%s = %s" % (key, path))
            if not os.path.exists(path):
                comm_log_record.logger.info("path[%s] not exist. creat." % path)
                os.makedirs(path)

