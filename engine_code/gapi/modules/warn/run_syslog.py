#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import traceback
import commands
import subprocess
from xml.etree import ElementTree
import common

 
level_list=['emerg','alert','crit','err','warning','notice','info','debug']
param_list=['enable','ip','port','module','level']
func_list=['enable_chk','ip_check','port_chk','module_chk','level_chk']

def enable_chk(enable):
    if enable != "0" and enable != "1":
        return 1
    return 0
def ip_check(ip):
    addr=ip.split('.')
    if len(addr) != 4:
        return 1
    for i in range(len(addr)):
        try:
            addr[i]=int(addr[i])
        except:
            return 1
        if addr[i]<=255 and addr[i]>=0:
           pass
        else:
            return 1
    return 0
def port_chk(port):
    try:
        port_int=int(port)
    except:
        return 1
    if port_int>=0 and port_int<=6535:
        return 0
    return 1
def module_chk(module):
    if module !="0" and module != "1":
        return 1
    return 0
def level_chk(level):
    try:
        level_int=int(level)
    except:
        return 1
    if level_int >= 0 and level_int <= 7:
        return 0
    return 1

# translate from list into dict 
def translate(argv):
    dict={}
    for i in range (len(argv)):
        index = argv[i].find('=')
        if index == -1:
            dict.clear()
            break;
        dict[argv[i][0:index]]=argv[i][index+1:len(argv[i])]
    return dict

# check value of param
def check(dict):
    if len(dict) == 0:
        return 1
    #if dict['enable'] == None: #will raise exception, so replace it 
    if dict.get('enable') == None:
        return 1
    if dict['enable'] == "0":
        if len(dict) != 1:
            return 1
        return 0
    # below is enable case 
    if len(dict) != len(param_list) or dict.keys() != {}.fromkeys(param_list).keys():
        print 'lxw not equal in keys'
        return 1

    for i in dict:
        #print 'chk one of dict: %s,%s' % (i,dict[i])
        ret = globals()[func_list[param_list.index(i)]](dict[i]) # dict[i]+'_chk' is another implemention
        if ret != 0:
            #print 'chck fail in field: %s' % i
            return 1
    return 0

# write good parameers into xml file 
def write_xml(dict):
    try:
        tree = ElementTree.parse(common.gms_config_file)
        node = tree.find('gms_warn/syslog')
    except:
        sys.stderr.write('config file [%s] format error.' % common.gms_config_file)
        return common.CONFIG_FORMAT_ERROR
    else:
        if dict['enable'] == "0":
            n = 1;
            node.attrib.clear()
        else:
            n = len(param_list)
        for i in range (n):
            node.attrib[param_list[i]] = dict[param_list[i]] 
        tree.write(common.gms_config_file)
    return 0

# when run syslog program, need read from xml 
def read_xml():
    try:
        tree = ElementTree.parse(common.gms_config_file)
        node = tree.find('gms_warn/syslog')
    except:
        sys.stderr.write('config file [%s] format error.' % common.gms_config_file)
        return common.CONFIG_FORMAT_ERROR
    return node.attrib

# call by modify or recover
def run_xml():
    # both cases, stop this service firstly !
    p = subprocess.Popen(['killall', 'rsyslogd'],
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    status = p.wait()
    if status != 0:
        sys.stderr.write(''.join(p.stderr.readlines()))
        #return common.ERR_SYSLOG_START_DAEMON_FAIL 

    # check stop or start
    dict = read_xml()
    if dict['enable'] == "0":
        # maybe redudant
       return 0
    # fill text 
    text=""
    if dict['module'] == "0":
        for i in range(0,8):
            module_text = "local"+str(i)+"."
            text=text+module_text+level_list[int(dict['level'])]+" @"+dict['ip']+":"+dict['port']+"\n"
    else:
        module_text = "*."
        text=module_text+level_list[int(dict['level'])]+" @"+dict['ip']+":"+dict['port']+"\n"

    # modify /etc/rsyslog.conf  and run syslog daemon 
    file = open('/etc/rsyslog.conf', 'w')
    file.write(text)
    file.close( )
    p = subprocess.Popen(['/usr/sbin/rsyslogd'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    status = p.wait()     
    if status != 0:
        sys.stderr.write(''.join(p.stderr.readlines()))
    return status

# call by modify
def run_syslog(argv):
    #<gms_warn> <syslog enable="1" ip="1.1.1.1" port="1" module="1" level="1" /> </gms_warn>
    dict = translate(argv)
    #print "after translate's dict: %s" % str(dict)

    if check(dict) != 0:
        return common.ERR_SYSLOG_CHECK_PARAM_FAIL

    write_xml(dict)
    if run_xml() != 0:
        return common.ERR_SYSLOG_START_DAEMON_FAIL 


if __name__ == '__main__':
    exit(run_syslog(sys.argv[1:]))

