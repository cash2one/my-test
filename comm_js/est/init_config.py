#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import getopt
from xml.etree import ElementTree as ET

xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
xml_tree = ET.parse(xml_file)
#dom = xml_tree.find("email")
#print dom.text, dom.get("head")

xmlNodeDict = {
               "email": '''
               <email head="GMS" switch="1"><send_index><mail>chanct_wangan@163.com</mail><pw>chanct-gms123</pw><mail_server>smtp.163.com</mail_server><port>25</port></send_index><send_other><mail></mail><pw></pw><mail_server></mail_server><port></port></send_other><container></container></email>
                        ''',
              "security": '<security num="5" set="0"></security>',
              "threshold": '<threshold mode="10" oday="100" pro="10" psize="" war="20" />',
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

def init_dom(tag, con, xml_tree):
        dom = xml_tree.find(tag)                                         
        if dom == None:
            xml_tree.getroot().append(ET.fromstring(con))
            xml_tree.write(xml_file)
            return display(xml_file)
        else:
            return False
 
# 获取配置文件，初始化成邮件功能需要的格式  
# 使用面向功能的方法，减少面向对象封装的冗余逻辑判断 
class InitConfig(object):
    def __init__(self):
        # 加入路径，准备获取配置文件信息 
        self.xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        self.xml_tree = ET.parse(xml_file)

    # 发件邮箱信息的获取 
    def init_send(self):
        for tag, con in xmlNodeDict.items():
            init_dom(tag, con , self.xml_tree)
        dom = self.xml_tree.find("email")
        if str(dom.get("switch")) == "2":
            sen = dom.find("send_other")
        else :
            sen = dom.find("send_index")
        send_dict = {"mail": sen.find("mail").text, "password": sen.find("pw").text, "head": dom.get("head"),\
                     "mail_server": sen.find("mail_server").text, "port": sen.find("port").text, }
        return send_dict

    # 某一事件的收件人邮箱列表的获取 
    def init_recv(self, events):
        dom = self.xml_tree.find("email/container")
        recv_list = []
        for child in dom.getchildren():
            if str(child.get("switch")) == "1" and str(child.get(events)) == "1" :
                recv_list.append([child.get("mail"), child.get("reci"), child.get("level")])
        return recv_list

    # 信任ip信息的获取 
    def init_security(self):
        dom = self.xml_tree.find("security")
        secu_dict = {"set": dom.get("set")}
        addr_str = ""
        for child in dom.getchildren():
            if len(str(child.text))>=3:
                addr_list = str(child.text).split("/")
            else:
                continue
            exchange_mask = lambda mask: sum(bin(int(i)).count('1') for i in mask.split('.'))
            addr = addr_list[0] + "/" + str(exchange_mask(addr_list[1])) + ";"
            addr_str=str(addr)
        secu_dict.update({"addr": addr_str})
        return secu_dict

    # 预警信息的获取 
    def init_threshold(self):
        dom = self.xml_tree.find("threshold")
        thre_dict = {"psize":dom.get("psize") , "war": dom.get("war"), "pro": dom.get("pro")}
        return thre_dict

if __name__ == "__main__":
    # xml树读取
    #pera = InitConfig()
    #print pera.init_send()
    #print pera.init_recv("event1")
    #print pera.init_security()
    #print pera.init_threshold()
    print xml_file
    print display(xml_file)








