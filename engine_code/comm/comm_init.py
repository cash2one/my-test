# -*- coding:utf-8 -*-
from xml.etree import ElementTree
import os
import json
import comm_data
import comm_common
import comm_conn_manage
import comm_log_record
import device_manage
from est.init_config import init_dom, xmlNodeDict

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
gms_conf_file = os.path.abspath("/gms/conf/gmsconfig.xml")
pre_conf_file = os.path.abspath("/gms/comm/conf/comm_pre_conf.json")


#配置初始化
def init_conf(levelLog = "error" ):
    #comm所有配置文件初始化
    read_pre_json()                     # 这里读入日志目录
    comm_log_record.record_log_init(levelLog)   # 初始化 logger
    comm_log_record.logger.info("comm_main start.%s" % levelLog)
    comm_log_record.logger.info("comm_main start.")
    comm_log_record.logger.info("--------------------- config ---------------------")

    comm_data.uuid = read_uuid()

    read_interface_xml()
    read_ftp_conf()

    print_init_info()
    comm_log_record.logger.info("--------------------------------------------------")

    #device_manage.topo_file_init()
    device_manage.load_topo_dist()
    comm_log_record.logger.info("-----------------------10G-------------------------")
    xge()
def create_comm_link_node():
    from xml.etree.ElementTree import ElementTree,Element
    tree = ElementTree()
    tree.parse(gms_conf_file)
    root = tree.getroot()
    p = tree.find("comm_stat")
    if p == None:
        element = Element("comm_stat", {"link":"1"})
        element.text = " "
        element.tail = "\n\t"
        root.append(element)
        tree.write(gms_conf_file)

def read_interface_xml():
    global interface_conf_file
    global pre_conf_file
    #10G
    global gms_conf_file
    root = ElementTree.parse(interface_conf_file)
    lst_node = root.getiterator("cominfo")
    #sub_dev = comm_data.sub_dev_info

    #10G
    root_10G = ElementTree.parse(gms_conf_file)
    lst_node_10G = root_10G.getiterator("gmsconf")

    devtype_node_10G = root_10G.find("gms_xge_monitor/if_is_xge_device")
    comm_data.if_is_xge_device = devtype_node_10G.text

    devtype_node_10G = root_10G.find("comm_stat")
    try:
        comm_data.link = devtype_node_10G.get("link")
    except:
        #comm_link node not found ,so create fixed by zdw
        create_comm_link_node()
        comm_data.link = '1'

    devtype_node_10G = root_10G.find("gms_xge_monitor/comm_sh_port")
    comm_data.comm_sh_port = devtype_node_10G.text

    #  
    for tag, con in xmlNodeDict.items():                                                    
        init_dom(tag, con , root_10G)
    comm_data.sys_war = root_10G.find("threshold").get("war")
    comm_data.sys_pro = root_10G.find("threshold").get("pro")

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

    #devtype_node = root.find("selfinfo/uuid")
    #comm_data.uuid = devtype_node.text

    devtype_node = root.find("selfinfo/devname")
    comm_data.dev_name = devtype_node.text
    #old flow event
    #devtype_node = root.find("selfinfo/old_flow")
    #comm_data.old_flow = devtype_node.text

    devtype_node = root.find("selfinfo/comm_cmd_port")
    comm_data.comm_cmd_port = int(devtype_node.text)

    devtype_node = root.find("selfinfo/comm_file_port")
    comm_data.comm_file_port = int(devtype_node.text)

    comm_log_record.logger.info("file up port [%s]" % comm_data.comm_cmd_port)
    comm_log_record.logger.info("file down port [%s]" % comm_data.comm_file_port)
    comm_data.topo.insert(0, comm_data.dev_ip + ':' + comm_data.uuid) # 0索引留给自己

    comm_log_record.logger.info("uuid: [%s]" % (comm_data.uuid))
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
def xge():
    if comm_data.if_is_xge_device == '1' :
        file_name = comm_data.comm_reg_up_path
        comm_data.comm_reg_up_path = comm_data.comm_reg_up_tmp_path
        comm_data.comm_reg_up_tmp_path = file_name
    else:
        comm_log_record.logger.info("read json if it is 1G: read_pre_json." )
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

def read_uuid():
    try:
        ret, stdout, stderr = comm_common.system(str(comm_data.uuid_script))
    except Exception as ex:
        raise comm_common.CommException('read uuid fail. [%s].' % str(ex))
    else:
        if ret == 0:
            return stdout
        else:
            raise comm_common.CommException("exec uuid script[%s]: ret[%s] stdout[%s] stderr[%s]" % (comm_data.uuid_script, ret, stdout, stderr))

def print_init_info():
    for key in dir(comm_data):
        if key.find('_path', -5) != -1:
            path = getattr(comm_data, key)
            comm_log_record.logger.info("%s = %s" % (key, path))
            if not os.path.exists(path):
                comm_log_record.logger.info("path[%s] not exist. creat." % path)
                os.makedirs(path)

