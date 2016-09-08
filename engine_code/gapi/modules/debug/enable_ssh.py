#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import traceback
import commands
import subprocess
from xml.etree import ElementTree
import common

def enable_ssh():
    #<gms_debug> <ssh enable="1" /> </gms_debug>
    try:
        tree = ElementTree.parse(common.gms_config_file)
        node = tree.find('gms_debug/ssh')
    except:
        sys.stderr.write('config file [%s] format error.' % common.gms_config_file)
        return common.CONFIG_FORMAT_ERROR
    else:
        #status, result = commands.getstatusoutput('service ssh start')
        p = subprocess.Popen(['/usr/local/sbin/sshd'],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        status = p.wait()
        if status == 0:
            node.attrib['enable'] = common.SSH_ENABLE
            tree.write(common.gms_config_file, encoding="utf-8")
        else:
            sys.stderr.write(''.join(p.stderr.readlines()))
        return status

if __name__ == '__main__':
    exit(enable_ssh())

