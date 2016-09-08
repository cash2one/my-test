#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import traceback
import commands
import subprocess
from xml.etree import ElementTree
import common

def disable_ssh():
    #<gms_debug> <ssh enable="1" /> </gms_debug>
    try:
        tree = ElementTree.parse(common.gms_config_file)
        node = tree.find('gms_debug/ssh')
    except:
        sys.stderror.write('config file [%s] format error.' % common.gms_config_file)
        return common.CONFIG_FORMAT_ERROR
    else:
        # 这句话即使执行失败了，在我的虚拟机上还是返回 0
        p = subprocess.Popen(['killall', 'sshd'],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        status = p.wait()
        if status == 0:
            node.attrib['enable'] = common.SSH_DISABLE
            tree.write(common.gms_config_file, encoding="utf-8")
        else:
            sys.stderr.write(''.join(p.stderr.readlines()))
        return status

if __name__ == '__main__':
    exit(disable_ssh())

