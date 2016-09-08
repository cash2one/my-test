# -*- coding:utf-8 -*-
from xml.etree import ElementTree
import os
import comm_data
import sys
import shlex
import subprocess
uuid_script="/gms/gapi/modules/auth/genkey.pyc"

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")
gms_conf_file = os.path.abspath("/gms/conf/gmsconfig.xml")
comm_reg_up_path = "/data/tmpdata/comm/reg/up/"
comm_xge_reg_up_path = "/data/tmpdata/comm/reg/tmp/"

class CommException(Exception):
    def __init__(self, msg):
        self.msg = msg
        BaseException.__init__(self)

    def __str__(self):
        return self.msg


def system(cmd):
    argv = shlex.split(cmd)
    script = argv[0]
    ext_name = os.path.splitext(script)[-1]
    ext_map = {'.py': 'python', '.pyc': 'python'}

    if ext_map.has_key(ext_name):
        argv.insert(0, ext_map[ext_name])

    p = subprocess.Popen(argv,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    return (p.returncode, stdout, stderr)

def read_uuid():
    try:
        ret, stdout, stderr = system(str(uuid_script))
    except Exception as ex:
        raise CommException('read uuid fail. [%s].' % str(ex))
    else:
        if ret == 0:
            return stdout
        else:
            raise CommException("exec uuid script[%s]: ret[%s] stdout[%s] stderr[%s]" % (uuid_script, ret, stdout, stderr))
def getpid(proc):
    ret, stdout, stderr = system('pidof ' + proc)
    if ret == 0:
        return stdout
    else:
        return None

def read_interface_xml():
    global interface_conf_file
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


    devtype_node = root.find("selfinfo/devname")
    comm_data.dev_name = devtype_node.text
    
    comm_data.uuid = read_uuid()

def xge():
    if comm_data.if_is_xge_device == '1' :
        comm_data.comm_reg_up_path = comm_xge_reg_up_path
    else :
        comm_data.comm_reg_up_path = comm_reg_up_path

def conf_init():
    read_interface_xml()
    xge()
     
