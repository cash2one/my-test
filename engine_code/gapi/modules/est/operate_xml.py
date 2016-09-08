#!/usr/bin/evn python
# -*- coding:utf-8 -*-

import os
#from xml.dom import  minidom as ope_xml
from xml.etree import ElementTree
class OperateXml(object):
    def __init__(self):
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        self.root = ElementTree.parse(xml_file)
        child_code = self.root.getiterator("gmsconf")
    def read_xml(self, str_dom):
        return self.root.findall(str_dom)

def ope_xml():
    ope = OperateXml()
    ope.read_xml("email/send_index/mail")
if __name__ == "__main__":
    ope_xml()

    
