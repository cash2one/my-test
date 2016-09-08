#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import traceback
import commands
import subprocess
import shlex
from xml.etree import ElementTree
import common

def system(cmd):
    p = subprocess.Popen(shlex.split(cmd),
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    return (p.returncode, stdout, stderr)

def start_ssh():
    cmd = '/usr/local/sbin/sshd'
    ret, stdout, stderr = system(cmd)
    if ret != 0 or stderr != '':
        sys.stderr.write('exec cmd[%s] ret[%s] stdout[%s] stderr[%s]' % (cmd, ret, stdout, stderr))
    return ret

def stop_ssh():
    cmd = 'killall sshd'
    ret, stdout, stderr = system('killall sshd')
    if ret != 0 or stderr != '':
        sys.stderr.write('exec cmd[%s] ret[%s] stdout[%s] stderr[%s]' % (cmd, ret, stdout, stderr))
    return ret

# 如果进程不存在返回 None
def getpid(proc):
    ret, stdout, stderr = system('pidof ' + proc)
    if ret == 0:
        return stdout
    else:
        return None

def init_ssh():
    try:
        tree = ElementTree.parse(common.gms_config_file)
        node = tree.find('gms_debug/ssh')
    except Exception as ex:
        sys.stderr.write('config file [%s] format error. %s.' % common.gms_config_file, str(ex))
        return common.CONFIG_FORMAT_ERROR
    else:
        try:
            enable_ssh = node.attrib['enable']
        except Exception as ex:
            sys.stderr.write('config file [%s] format error. %s.' % (common.gms_config_file, str(ex)))
            return common.CONFIG_FORMAT_ERROR
        else:
            if enable_ssh == common.SSH_ENABLE:
                if getpid('sshd') == None:
                    return start_ssh()
                else:
                    return 0
            elif enable_ssh == common.SSH_DISABLE:
                if getpid('sshd') == None:
                    return 0
                else:
                    return stop_ssh()
            else:
                sys.stderr.write('config file [%s] format error. gms_debug.ssh.attrib["enable"] = [%s].' % (common.gms_config_file, node.attrib['enable']))
                return common.CONFIG_FORMAT_ERROR

if __name__ == '__main__':
    exit(init_ssh())

