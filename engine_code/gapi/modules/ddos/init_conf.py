#!/usr/bin/env python
#-*- coding:utf-8 -*-

'''
@auther: wangkun
@date: September, 01, 2015
'''

import os
import ConfigParser
from xml.etree import ElementTree as ET
PATH = os.path.abspath("/gms/tcad/ddos/iplist.conf")

class OperationFile(object):
    def __init__(self, file_name):
        self.file_name = file_name
        self.resoult = (True, False) 
    def write(self, sect,*args ,**kargs):
        config = ConfigParser.ConfigParser()
        config.read(self.file_name)
        if not sect in config.sections() and kargs :
            config.add_section(sect)
            for vari, content in kargs.items():
                config.set(sect, vari, content)
            with open(self.file_name, 'w') as f:
                config.write(f)
                f.close()
            resoult = self.resoult[0]
        else:
            resoult = self.resoult[1]
        return resoult 
    def read(self, options=None, confs=None):
        config = ConfigParser.ConfigParser()
        config.read(self.file_name)  
        if options and confs:
            resoult = config.get(options, confs)
        elif options and not confs:
            resoult = config.items(options)
        elif not options :
            resoult = config.sections()
        else:
            resoult = self.resoult[1] 
        return resoult
    def modify(self, sections, options, confs):
        if sections and options and confs :
            config = ConfigParser.ConfigParser()
            config.read(self.file_name)
            config.set(sections, options, confs)
            with open(self.file_name, 'w') as f:
                config.write(f)
            resoult = self.resoult[0]
        else:
            resoult = self.resoult[1]
        return resoult
    def delete(self, section, option = None):
        config = ConfigParser.ConfigParser() 
        config.read(self.file_name)
        resoult = self.resoult[0]
        if section and option :
            config.remove_option(section, option)
        elif not option:
            config.remove_section(section)
        else:
            resoult = self.resoult[1]
        if resoult:
            with open(self.file_name, 'w') as f: 
                config.write(f)
        return resoult

class XmlAction(object):
    def __init__(self):
        self.xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        self.xml_tree = ET.parse(self.xml_file)
        self.dom = self.xml_tree.find("ddos_set")

    def update(self, tag, key, value ):
        node = self.dom.find(tag)
        node.set(key, value)
        return True

    def create(self, value):
        child_node = ET.Element("ip_list")
        child_node.text = value
        self.dom.append(child_node)
        return True

    def delete(self, value):
        ip_list = self.dom.findall("ip_list")
        value_list = value.split(";")
        for ip in ip_list:
            if ip.text in value_list:
                self.dom.remove(ip)

    def save(self):
        self.xml_tree.write(self.xml_file)
        return True

def ip_list_conf(ope, ip):
    with open(PATH) as fp:
        file = fp.readlines()
    with open(PATH, "w") as fp :
        if ope == "c":
            file.append(ip + "\n")
        elif ope == "d":
            file = [i for i in file if not ip + "\n" == i]
        for i in file:
            fp.write(i)
    return True

if __name__ == "__main__":
    #init_file = OperationFile("ddos.conf")
    ##print init_file.write("aaaaaa",aa="123")
    #print init_file.read("ddos", "debug" ) # ,"debug")
    #print init_file.modify("ddos","debug",12311)
    ##print init_file.delete("aaaaaa","aa")
    ## xml 
    #aaa = XmlAction()
    #aaa.update("ddos_switch", "open", "1")
    #aaa.save()
    # ip list
    ip_list_conf("d", "11.1.1") 
