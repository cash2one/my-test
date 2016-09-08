#!/usr/bin/python
import sys
sys.path.append("/gms/gapi/modules/network")
sys.path.append("/gms/gapi/modules/auth")
import sys_auth
import text_xml
import os
import socket
import struct
xge_device = 0
dev_id= None
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
	if sys_auth.read_auth_flag() == '1':
		ipval="0.0.0.0"
		ip_nodes=text_xml.find_nodes(tree,"neighbour/device")
		for ip_node in ip_nodes:
			if ip_node.attrib['dev_id'] == dev_id:
				ipval=ip_node.attrib['ip']
				break
		xge_ip_devid_xml(ipval,dev_id)
	return 0
def read_xge_device():
	tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	nodes_mange = text_xml.find_nodes(tree,"gms_xge_monitor/if_is_xge_device")
	return nodes_mange[0].text
def from_face_to_xml():
	xge_device=int(read_xge_device())
	if xge_device == 1:
		gmsconfig_monitor_xml()
	
	return 0
if __name__ == "__main__":
	sys_auth.read_auth_flag()
	dev_id=sys_auth.get_devid_id()
	exit(from_face_to_xml())
