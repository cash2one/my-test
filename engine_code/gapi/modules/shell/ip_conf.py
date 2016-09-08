#!/usr/bin/python
import sys
sys.path.append("/gms/gapi/modules/network")
import text_xml
import os
import socket
import struct
def gmsconfig_monitor_xml(ipval,dev):
    tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
    ip_nodes=text_xml.find_nodes(tree,"neighbour/device")
    for ip_node in ip_nodes:
        print ip_node.attrib['ip'],ipval
        if ip_node.attrib['ip'] == ipval:
            ip_node.attrib['dev_id']=dev
            break
    text_xml.write_xml(tree, "/gms/conf/gmsconfig.xml")
    return 0
if __name__ == "__main__":
    exit(gmsconfig_monitor_xml(sys.argv[1],sys.argv[2]))
