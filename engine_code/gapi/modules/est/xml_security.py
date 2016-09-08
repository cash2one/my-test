#!/usr/bin/env python
#-*- coding:utf-8 -*-

'''
@auther: wangkun 
@date: Sept, 15, 2015
'''

import os,time, subprocess
from xml.etree import ElementTree as ET


xmlNodeDict = { 
               "email": ''' 
               <email head="GMS" switch="1"><send_index><mail>chanct_wangan@163.com</mail><pw>chanct-gms123</pw><mail_server>smtp.163.com</mail_server><port>25</port></send_index><send_other><mail></mail><pw></pw><mail_server></mail_server><port></port></send_other><container></container></email>
                        ''',
              "security": '<security num="5" set="0"></security>',              "threshold": '<threshold mode="10" oday="100" pro="10" psize="" war="20" />',
              "ddos_set": '''<ddos_set><ddos_switch open="1" self_learn="0" /><in_threshold flow="100" tcp_live_conn="1000" tcp_new_conn="100000" /><out_threshold flow="100" tcp_live_conn="1000" tcp_new_conn="100000" /></ddos_set> 
                          ''',
              }   

def display(xmlpath):                                                                                   
    with open(xmlpath) as fp:                                                                           
        input = fp.read()
    print str(input)
    output = input.replace("><", ">\n<")                                                                   
    with open(xmlpath, "w") as fp:                                                                         
        fp.write(output)                                                                                 
    return True 

def init_dom(tag, con, xml_tree, xml_file):
        dom = xml_tree.find(tag)    
        if dom == None:
            xml_tree.getroot().append(ET.fromstring(con))
            xml_tree.write(xml_file)
            return display(xml_file)
        else:
            return False

def read_xml():
    try:
        result = []
        dports = "10107:10108"
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        xml_tree = ET.parse(xml_file) 
        for tag, con in xmlNodeDict.items():
            init_dom(tag, con , xml_tree, xml_file)
        dom = xml_tree.find("security").getchildren()
        result += [ i.text for i in dom]
        if xml_tree.find("security").get("set") == "1" : 
            dom = xml_tree.find("gms_network/net")
            result.append(dom.get("ip")+"/"+dom.get("netmask"))
        else:
            result.append("0.0.0.0/0")
        dom = xml_tree.find("gms_device/attr")
        dports = str(dom.get("up_port")) + ":" + str(dom.get("down_port"))
    except Exception, ex:
        print ex
        logger.error(ex)
    finally:
        return (result, dports)
def xml_security():
    try:
        data_init = "iptables -F INPUT ; iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT;iptables -A INPUT -p icmp -j ACCEPT;iptables -A INPUT -i lo -j ACCEPT"
        os.system(data_init)
        xml_data = read_xml()
        subprocess.Popen(["iptables", "-A", "INPUT", "-m", "state", "--state", "NEW", "-m", "tcp", \
                          "-p", "tcp", "--dport",xml_data[1] ,"-j", "ACCEPT"], stdout=subprocess.PIPE)
        for i in xml_data[0] :
            for j in ["22", "443"]:
                subprocess.Popen(["iptables", "-A", "INPUT", "-s", i,"-m", "state", \
                      "--state", "NEW", "-m", "tcp", "-p", "tcp", \
                      "--dport", j , "-j", "ACCEPT"], stdout=subprocess.PIPE)
        subprocess.Popen(["iptables", "-A", "INPUT", "-j", "REJECT", "--reject-with", "icmp-host-prohibited"], stdout=subprocess.PIPE)
    except Exception, ex :
        print ex
        logger.error(ex)
    finally:
        return True

if __name__ == "__main__":
    print xml_security()
