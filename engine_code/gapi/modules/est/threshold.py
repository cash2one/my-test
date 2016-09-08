#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os, sys
import getopt
from xml.etree import ElementTree as ET

from log.init_logger import init_logger
logger = init_logger()
#logger.debug('This is debug message')

# 系统预警 
class Threshold(object):
    def __init__(self, *args, **kargs):
        # 初始化时获取系统预警的参数 
        self.args = args[0]
        self.kargs = kargs
        self.tag = 0 

    # 修改系统预警配置文件的参数值 
    def run(self):
        sys.path.append("/gms/comm/")
        import comm_data
        setattr(comm_data, "sys_war", str(self.args.get("-l")))
        setattr(comm_data, "sys_pro", str(self.args.get("-s")))
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        xml_tree = ET.parse(xml_file)
        
        xml_root = xml_tree.getroot()
        dom = xml_root.findall("threshold")
        if len(dom) != 1 :
            for i in dom:
                xml_root.remove(i)
            test.append(ET.Element("threshold"))
            dom = xml_tree.find("threshold")
        else:
            dom = dom[0]
        xml_tree.write(xml_file)
                
        #python /gms/gapi/modules/est/threshold.pyc -l 20 -s 10 -t 2 -d 10
        dom.set("pro", str(self.args.get("-s")))
        dom.set("war", str(self.args.get("-l")))
        dom.set("mode", str(self.args.get("-t")))
        if self.args.get("-t") == "2":
            dom.set("oday", str(self.args.get("-d")))
        xml_tree.write(xml_file)
        logger.debug('opera %s' % self.args)
        return self.tag

# 执行函数 
def main():
    opts,args = getopt.getopt(sys.argv[1:],"l:s:t:d:",["help",])    
    logger.warning('This is user\'s opear message for threshold: %s' % opts)
    sec = Threshold(dict(opts))
    return sec.run()
if __name__ == '__main__':
    exit(main())
