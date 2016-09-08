#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
from xml.etree import ElementTree as ET
import json
import comm_init
import comm_data


RESOLVFILE = os.path.abspath("/etc/resolv.conf")
XMLFILE = os.path.abspath("/gms/conf/gmsconfig.xml")

def _DNS_file(dns_ip):
    with open(RESOLVFILE, "w") as fp :
        dns_line = "nameserver " + dns_ip
        fp.write(dns_line)
    return True

def _write_DNS_file(dns_str):
    xml_tree = ET.parse(XMLFILE)
    dom = xml_tree.find("gms_network/dns")
    dom.set("dns1", dns_str)
    xml_tree.write(XMLFILE)
    res = _DNS_file(dom.get("dns1"))
    return res

def check_DNS(DNS_list):
    result = os.system("nslookup oss.aliyuncs.com") 
    if not result:
        print ' Check DNS , Network  reachable .'
        return True
    else :
        for dns in DNS_list:
            try:
                _write_DNS_file(dns)
                if not os.system("nslookup oss.aliyuncs.com"):
                    break
            except Exception, exDNS_bak:
                print exDNS_bak
                return False
    return False
         
if __name__ == "__main__":
    import json
    #backup_dns=['223.5.5.5','223.6.6.6','1.2.4.8','210.2.4.8','114.114.114.114', \
               #'114.114.115.115','112.124.47.27','114.215.126.16','101.226.4.6','218.30.118.6',\
               #'123.125.81.6','140.207.198.6','8.8.8.8','8.8.4.4','208.67.222.222',\
               #'208.67.220.220','178.79.131.110','180.76.76.76','202.112.0.33']

    comm_init.read_pre_json()                                                                                                    
    comm_data.backup_dns = eval(comm_data.backup_dns)                                                                                                   
    check_DNS(comm_data.backup_dns)

