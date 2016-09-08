#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os, sys
import getopt
from xml.etree import ElementTree as ET

#from init_iptables import init_main, other_trust_ip, index_trust_ip
from xml_security import xml_security

# 调用日志系统 
from log.init_logger import init_logger
logger = init_logger()
#logger.debug('This is debug message')

# 信任ip的操作 
class Security(object):
    def __init__(self, *args, **kargs):
        self.args = args[0]
        self.kargs = kargs
        self.tag = 0
    def run(self):
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        xml_tree = ET.parse(xml_file)
        xml_root = xml_tree.getroot()
        dom_all = xml_root.findall("security")
        if len(dom_all) > 1:
            for i in dom_all[1:] :
                if i.getchildren() == 0:
                    xml_root.remove(i)
            xml_tree.write(xml_file) 
        dom = xml_tree.find("security")
        logger.debug('-i :%s' % self.args.get("-i"))
        # 添加非直连网段的ip 
        if self.args.get("-i"):
            # 添加信任ip 
            if str(self.args["-o"]) == "c" :
                addr = ET.Element("addr")
                addr.text = self.args.get("-i")
                dom.append(addr)
                xml_tree.write(xml_file)
                ##other_trust_ip("-A", self.args.get("-i"))
                ##init_main()
                logger.info('opera OK!')
            # 删除信任ip 
            elif self.args["-o"] == "d" :
                args_list = self.args.get("-i").split(";")
                for child in dom.findall('./addr'): 
                    if child.text in args_list:
                        #dom = xml_tree.find("security")
                        dom.remove(child)
                        xml_tree.write(xml_file)
                        ##other_trust_ip("-D", self.args.get("-i"))
                ##init_main()
                logger.info('opera OK!')
            else:
                logger.error('opera Failed!')
                self.tag = 1
                sys.exit(120)
        else:
            # 修改最多的非直连网段的信任ip个数，是否直连 
            dom.set("set", str(self.args.get("-d")))
            dom.set("num", str(self.args.get("-m")))
            xml_tree.write(xml_file)
            ##if str(self.args.get("-d")) == "0":
            ##    index_trust_ip("-D")
            ##else:
            ##    index_trust_ip("-A")
            ##init_main()
            ##logger.info('opera init security OK!')
        xml_security()
        logger.info('opera security OK!')
        return self.tag

def main():
    opts,args = getopt.getopt(sys.argv[1:],"o:d:m:i:",["help",])    
    logger.warning('This is user\'s opear message for security : %s' % opts)
    sec = Security(dict(opts))
    return sec.run()
if __name__ == '__main__':
    main()
