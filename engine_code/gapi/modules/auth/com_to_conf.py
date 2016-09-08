#!/usr/bin/python
# _*_ coding: utf8 _*_
import sys
sys.path.append("/gms/gapi/modules/network")
sys.path.append("/gms/gapi/modules/auth")
sys.path.append("/gms/gapi/modules/network/reg")
import sys_auth
import text_xml
import os
import socket
import json
import struct
import time
import comm_reg
json_path="/gms/gapi/modules/auth/conf/auth_path.json"
path_dict={}
xge_device = 0
def get_fork_stat(forkname):
	comm_main_cmd="ps x|grep -v 'grep'|grep "+forkname+"|sed 's@^ @@g'|cut -f 1 -d ' '|awk '{a=$0;getline;print $0}'|cut -f 1 -d r|awk 'NF>0'"
	res=os.popen(comm_main_cmd)
	ret=res.read()
	#print ret
	# 进程没有开启
	if ret == "":
		return False
	# 进程已经开启
	else:
		return ret
def read_json_conf():
	global path_dict
	if os.path.exists(json_path) == False:
		sys.exit(13)
	fp= open(json_path,"r")
	path_dict = json.load(fp)
	fp.close()
	return 0
def iptolong(ipstr):
	return struct.unpack("!I", socket.inet_aton(ipstr))[0]
def read_man_xml():
	tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	root = tree.getroot()
	nodes = text_xml.find_nodes(tree,"gms_device/attr")
	return nodes[0].attrib['monitor']
def write_conf_xml(manager,monitor,cloudcon,rootval,ipval,devname,cmd_port,file_port):
	tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	root = tree.getroot()
	nodes = text_xml.find_nodes(tree,"gms_device")
	if nodes == []:
		a=text_xml.create_node("attr",{"cloudcon":cloudcon,"down_port":file_port,"ip":ipval,"manager":manager,"monitor":monitor,"name":devname,"root":rootval,"up_port":cmd_port,"head_port":cmd_port,"tail_port":file_port}," ","\n\t")
		b=text_xml.create_node("gms_device",{},"\n\t","\n")
		root.append(b)
		child_nodes=text_xml.find_nodes(tree,"gms_device")
		text_xml.add_child_node(child_nodes,a)
	else:
		net_node=text_xml.find_nodes(tree,"gms_device/attr") 
		text_xml.change_node_properties(net_node,{"cloudcon":cloudcon,"down_port":file_port,"ip":ipval,"manager":manager,"monitor":monitor,"name":devname,"root":rootval,"up_port":cmd_port,"head_port":cmd_port,"tail_port":file_port},"")
	text_xml.write_xml(tree, "/gms/conf/gmsconfig.xml")  
	return 0
def add_monitor(monitorval=[]):
	tree = text_xml.read_xml("/gms/conf/comm_interface_conf.xml")
	root = tree.getroot()
	nodes_one = text_xml.find_nodes(tree,"subdev")
	nodes = text_xml.find_nodes(tree,"subdev/devinfo")
	if nodes == []:
		i=0
		for node in monitorval:	
			print node.attrib['ip']
			a=text_xml.create_node("devinfo",{}," ","\n\t")
			text_xml.add_child_node(nodes_one,a)
			nodes = text_xml.find_nodes(tree,"subdev/devinfo")
			a1=text_xml.create_node("ip",{},node.attrib['ip'],"\n\t")
			a2=text_xml.create_node("comm_cmd_port",{},node.attrib['up_port'],"\n\t")
			a3=text_xml.create_node("comm_file_port",{},node.attrib['down_port'],"\n\t")
			text_xml.add_child_node(nodes[i:],a1)	
			text_xml.add_child_node(nodes[i:],a2)	
			text_xml.add_child_node(nodes[i:],a3)
			i=i+1	
	text_xml.write_xml(tree, "/gms/conf/comm_interface_conf.xml")  
def xge_ip_devid_xml(ipval,dev_id):
	tree = text_xml.read_xml("/gms/conf/comm_interface_conf.xml")
	nodes = text_xml.find_nodes(tree,"selfinfo/ip")
	if nodes != []:
		text_xml.change_node_text(nodes,ipval)
	nodes_id = text_xml.find_nodes(tree,"selfinfo/uuid")
	if nodes_id != []:
		text_xml.change_node_text(nodes_id,dev_id)
	text_xml.write_xml(tree, "/gms/conf/comm_interface_conf.xml")  
	
def gmsconfig_monitor_xml():
	tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	root = tree.getroot()
	nodes = text_xml.find_nodes(tree,"neighbour/device")
	if nodes == []:
		return 0
	else:
		add_monitor(nodes)
	return 0
def read_xge_device():
	tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	nodes_mange = text_xml.find_nodes(tree,"gms_xge_monitor/if_is_xge_device")
	if nodes_mange == []:
		return 0
	else:
		return nodes_mange[0].text
def is_manger_device():
	tree = text_xml.read_xml("/gms/conf/comm_interface_conf.xml")
	nodes_mange = text_xml.find_nodes(tree,"selfinfo/devtype/manager")
	if nodes_mange == []:
		return 0
	else:
		return nodes_mange[0].text
def from_face_to_xml():
	global monitor
	read_json_conf()
	xge_device=int(read_xge_device())
	tree = text_xml.read_xml("/gms/conf/comm_interface_conf.xml")
	root = tree.getroot()
	nodes_mange = text_xml.find_nodes(tree,"selfinfo/devtype/manager")
	manager=nodes_mange[0].text
	if xge_device == 1 and sys_auth.read_auth_flag() == '0':
		manager=nodes_mange[0].text="1"
	
	nodes_monitor = text_xml.find_nodes(tree,"selfinfo/devtype/monitor")
	monitor=nodes_monitor[0].text
	if xge_device == 1 and sys_auth.read_auth_flag() == '0':
		monitor=nodes_monitor[0].text="1"
	
	nodes_cloud = text_xml.find_nodes(tree,"selfinfo/devtype/cloudcon")
	cloudcon=nodes_cloud[0].text
	
	nodes_root = text_xml.find_nodes(tree,"selfinfo/devtype/root")
	rootval=nodes_root[0].text
	
	nodes_ip = text_xml.find_nodes(tree,"selfinfo/ip")
	ipval=nodes_ip[0].text
	if xge_device == 1 and sys_auth.read_auth_flag() == '0':
		nodes_ip[0].text=path_dict['ip']
	
	nodes_devname = text_xml.find_nodes(tree,"selfinfo/devname")
	devname=nodes_devname[0].text
	
	nodes_cmd_port=text_xml.find_nodes(tree,"selfinfo/comm_cmd_port")
	cmd_port=nodes_cmd_port[0].text
	
	nodes_file_port=text_xml.find_nodes(tree,"selfinfo/comm_file_port")
	file_port=nodes_file_port[0].text
	if int(cmd_port) > int(file_port):
		tmp_port=cmd_port
		cmd_port=nodes_cmd_port[0].text=file_port
		file_port=nodes_file_port[0].text=tmp_port
	text_xml.write_xml(tree, "/gms/conf/comm_interface_conf.xml")
	#com_st=get_fork_stat("comm_main.py")
	#print com_st
	if monitor != read_man_xml():
		comm_reg.create_local_reg_file()
	if rootval == "1":
		topu_filename="root_"+str(iptolong(ipval))+"_"+file_port+"_"+cmd_port+".xml"
	else:
		topu_filename=str(iptolong(ipval))+"_"+file_port+"_"+cmd_port+".xml"
	cp_cmd="cp -f /gms/conf/comm_interface_conf.xml "+"/data/tmpdata/comm/topo/self/"+topu_filename
	#if xge_device == 1 and sys_auth.read_auth_flag() != '0':
	#if sys_auth.read_auth_flag() != '0':
	os.system("mkdir -p /data/tmpdata/comm/topo/self")
	ret=os.system(cp_cmd)
	ret=write_conf_xml(manager,monitor,cloudcon,rootval,ipval,devname,cmd_port,file_port)
	if xge_device == 1 and sys_auth.read_auth_flag() == '0':
		gmsconfig_monitor_xml()
	
	return ret 
if __name__ == "__main__":
	time.sleep(0.5)
	exit(from_face_to_xml())
