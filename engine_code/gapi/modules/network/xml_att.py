#!/usr/bin/python
#Filename:xml_att.py
# _*_ coding: utf8 _*_
import text_xml
import sys
import os
import socket
import struct
import check_ip
import json
import time
sys.path.append("/gms/gapi/modules/network/reg")
sys.path.append("/gms/gapi/modules/est/")
from xml_security import xml_security
import comm_reg
sys.path.append("/gms/gapi/modules/auth")
import com_to_conf
json_path="/gms/gapi/modules/network/network_conf.json"
path_dict={}
def iptolong(ipstr):
    return struct.unpack("!I", socket.inet_aton(ipstr))[0]
MACHINE_TYPE = "/cfcard/chanct_conf/machine_type.conf"
def read_json_conf():
    global path_dict
    if os.path.exists(json_path) == False:
        sys.exit(13)
    fp= open(json_path,"r")
    path_dict = json.load(fp)
    fp.close()
    return 0
def read_security_xml():
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"security")
    return nodes[0].attrib['set']
def read_dev_info_xml():
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_device/attr")
    return nodes[0].attrib['root']
def read_dev_name_xml():
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_device/attr")
    return nodes[0].attrib['name']
        
def dev_info_xml(devname,cloudval,rootval,ipval,upval,downval,headport,tailport):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_device")
    if nodes == []:
        a=text_xml.create_node("attr",{"cloudcon":cloudval,"root":rootval,"name":devname,"ip":ipval,"up_port":upval,"down_port":downval,"head_port":headport,"tail_port":tailport}," ","\n\t")
        b=text_xml.create_node("gms_device",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_device")
        text_xml.add_child_node(child_nodes,a)
    else:
        net_node=text_xml.find_nodes(tree,"gms_device/attr") 
        text_xml.change_node_properties(net_node,{"cloudcon":cloudval,"root":rootval,"name":devname,"ip":ipval,"up_port":upval,"down_port":downval,"head_port":headport,"tail_port":tailport},"")
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 13

def ip_xml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_network")
    if nodes == []:
        a=text_xml.create_node("net",{"ip":tmp_dict['ip'],"netmask":tmp_dict['netmask'],"gateway":tmp_dict["gateway"]}," ","\n\t")
        c=text_xml.create_node("dns",{"dns1":tmp_dict['dns1'],"dns2":tmp_dict['dns2'],"proxy":tmp_dict['proxy']}," ","\n\t")
        b=text_xml.create_node("gms_network",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_network")
        text_xml.add_child_node(child_nodes,a)
        text_xml.add_child_node(child_nodes,c)
    else:
        net_node=text_xml.find_nodes(tree,"gms_network/net") 
        text_xml.change_node_properties(net_node,{"ip":tmp_dict['ip'],"netmask":tmp_dict['netmask'],"gateway":tmp_dict["gateway"]},"")
        dns_node=text_xml.find_nodes(tree,"gms_network/dns") 
        text_xml.change_node_properties(dns_node,{"dns1":tmp_dict["dns1"],"dns2":tmp_dict["dns2"],"proxy":tmp_dict['proxy']},"")
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0

def inface_xml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['comm_interface_conf'])
    root = tree.getroot()
    nodes_cloud = text_xml.find_nodes(tree,"selfinfo/devtype/cloudcon")
    nodes_cloud[0].text=tmp_dict["cloud"]
    nodes_root = text_xml.find_nodes(tree,"selfinfo/devtype/root")
    nodes_root[0].text=tmp_dict["root"]
    nodes_ip = text_xml.find_nodes(tree,"selfinfo/ip")
    nodes_ip[0].text=tmp_dict["ip"]
    if com_to_conf.read_xge_device() == '1' and com_to_conf.is_manger_device() == '1':
        nodes_ip[0].text=path_dict["ip"]
    nodes_ip = text_xml.find_nodes(tree,"selfinfo/devname")
    nodes_ip[0].text=tmp_dict["devname"].decode('utf-8')
    nodes_cmd_port=text_xml.find_nodes(tree,"selfinfo/comm_cmd_port")
    cmd_port=nodes_cmd_port[0].text=tmp_dict['up_port']
    nodes_file_port=text_xml.find_nodes(tree,"selfinfo/comm_file_port")
    file_port=nodes_file_port[0].text=tmp_dict['down_port']
    text_xml.write_xml(tree, path_dict['comm_interface_conf'])
    if tmp_dict['root'] == "1":
        topu_filename="root_"+str(iptolong(tmp_dict['ip']))+"_"+file_port+"_"+cmd_port+".xml"
    else:
        topu_filename=str(iptolong(tmp_dict['ip']))+"_"+file_port+"_"+cmd_port+".xml"
    cp_cmd="cp -f "+path_dict['comm_interface_conf']+" "+path_dict['topo_path']+"/"+topu_filename
    print cp_cmd
    os.system("mkdir -p "+path_dict['topo_path'])
    ret=os.system(cp_cmd)
    return 13
#netmask to int
exchange_mask = lambda mask: sum(bin(int(i)).count('1')for i in mask.split('.'))
def write_ip(macnum,tmp_dict={}):
    ip_file=path_dict['ifcfg_path']+macnum
    import subprocess
    ipaddr_cmd="sed -i '/^IPADDR/d' "+ip_file+"; sed -i '1a IPADDR="+tmp_dict['ip']+"' "+ip_file 
    mask_cmd="sed -i  '/^NETMASK/d' "+ip_file+";sed  -i  '1a NETMASK="+tmp_dict['netmask']+"' "+ip_file 
    gw_cmd="sed -i  '/^GATEWAY/d' "+ip_file+";sed -i  '1a GATEWAY="+tmp_dict['gateway']+"' "+ip_file 
    pre_cmd="sed -i '/^PREFIX/d' "+ip_file+";sed -i '1a PREFIX="+str(exchange_mask(tmp_dict['netmask']))+"' "+ip_file 
    dns_cmd="sed -i  '/^DNS/d' "+ip_file+";sed -i '1a DNS="+tmp_dict['dns1']+"' "+ip_file 
    #subprocess.call(ipaddr_cmd,shell=True)
    #subprocess.call(mask_cmd,shell=True)
    #subprocess.call(gw_cmd,shell=True)
    #subprocess.call(pre_cmd,shell=True)
    #subprocess.call(dns_cmd,shell=True)
    os.popen(ipaddr_cmd)
    #time.sleep(0.2)
    os.popen(mask_cmd)
    #time.sleep(0.2)
    os.popen(gw_cmd)
    #time.sleep(0.2)
    os.popen(pre_cmd)
    #time.sleep(0.2)
    os.popen(dns_cmd)
    #time.sleep(0.2)
def check_ardio(strip):
    if strip.split('.')[-1] == "255":
        return True
    else:
        return False
if __name__ == "__main__":
    tmp_dict={}
    dns_flag=0
    dns2_flag=0
    gw_flag=0
    if len(sys.argv) > 2 :
        text_xml.change_dict(sys.argv,tmp_dict,len(sys.argv))
    else:
        text_xml.change_str(sys.argv[1],tmp_dict,len(sys.argv[1]))
    print tmp_dict
    read_json_conf()
    MACHINE_TYPE=path_dict['machine_type']
    ret=check_ip.check_ip(tmp_dict['ip'])
    if ret == 1 or check_ardio(tmp_dict['ip']):
        exit(1)
    ret=check_ip.check_ip(tmp_dict['netmask'])
    if ret == 1:
        exit(ret)
    if tmp_dict['gateway'] !='':
        ret=check_ip.check_ip(tmp_dict['gateway'])
        if ret == 1 or check_ardio(tmp_dict['gateway']):
            exit(1)
    else:
        gw_flag=1
    if tmp_dict["dns1"] != "":
        ret=check_ip.check_ip(tmp_dict['dns1'])
        if ret == 1:
            exit(ret)
    else:
        dns_flag=1
    print tmp_dict["dns2"]
    if tmp_dict["dns2"] != "":
        print tmp_dict["dns2"]
        ret=check_ip.check_ip(tmp_dict['dns2'])
        if ret == 1:
            exit(ret)
    else:
        dns2_flag=1
    if tmp_dict['up_port'] != "":
        ret=check_ip.check_port(tmp_dict['up_port'])
        if ret == 21:
            exit(ret)    
    if tmp_dict['down_port'] != "":
        ret=check_ip.check_port(tmp_dict['down_port'])
        if ret == 21:
            exit(ret)    
    f = open(MACHINE_TYPE,'r')
    mactye=f.read(1)
    f.close()
    if mactye == "1":
        ethtmp="eth0"
    else:
        ethtmp="eth1"    
    cmd_ip="ifconfig "+ethtmp+" "+tmp_dict['ip']+" netmask "+tmp_dict['netmask']
    cmd_gw="route add default gw  "+tmp_dict['gateway']
    cmd_dgw="route del default gw  "+tmp_dict['gateway']
    cmd_dns="echo "+"'nameserver "+tmp_dict["dns1"]+"'"+">"+"/etc/resolv.conf"
    cmd_dns1="echo 'nameserver "+tmp_dict["dns2"]+"' >>"+"/etc/resolv.conf"
    if os.system(cmd_ip) != 0:
        exit(10)
    if gw_flag != 1:
        os.system(cmd_dgw)
        if os.system(cmd_gw) != 0: 
            exit(11)
    if dns_flag != 1:
        if os.system(cmd_dns) != 0: 
            exit(11)
    if dns2_flag != 1:
        if os.system(cmd_dns1) != 0:
            exit(12)
    if mactye == "1":
        write_ip("ifcfg-eth0",tmp_dict)
    else:
        write_ip("ifcfg-eth1",tmp_dict)

    # 先更新comm_interface.xml ，以备comm_reg.create_local_reg_file()
    # 获取最新的设备名称
    inface_xml(tmp_dict)

    if tmp_dict["root"] != read_dev_info_xml() or tmp_dict['devname'].decode('utf-8') != read_dev_name_xml():
        comm_reg.create_local_reg_file()
    if os.path.exists(path_dict['gmsconfig']) == False:
        exit(6)
    dev_info_xml(tmp_dict['devname'].decode('utf-8'),tmp_dict['cloud'],tmp_dict['root'],tmp_dict['ip'],tmp_dict['up_port'],tmp_dict['down_port'],tmp_dict['head_port'],tmp_dict['tail_port'])
    ret=ip_xml(tmp_dict)

    if int(read_security_xml()) != 2 :
        xml_security()
    exit(0)
