#!/usr/bin/python
# _*_ coding: utf8 _*_
import os,sys
from xml.etree import ElementTree

interface_conf_file = os.path.abspath("/gms/conf/comm_interface_conf.xml")

# 判断是否有上级结点。
def ischild():
    try:
        root = ElementTree.parse(interface_conf_file)
        devtype_node = root.find("selfinfo/devtype/root")
        isroot = devtype_node.text
        if isroot == '0':
            sys.stdout.write('yes')
        else:
            sys.stdout.write('no')
        return 0
    except:
        import traceback
        print traceback.format_exc()
        return 4    # 配置文件异常，见 gmserrno.conf

if __name__ == "__main__":
    exit(ischild())

