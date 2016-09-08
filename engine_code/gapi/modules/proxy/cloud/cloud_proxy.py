#!/usr/bin/python
#coding:utf8
import socks
import socket
import time
import threading
import sys
import getopt
import os
import parse
import comm_cloud
#import cloud_main
import cloud_log
#sys.path.append("/gms/gapi/modules/proxy/comm/oss/")
#from oss.oss_api import *
#sys.path.append("../")
from oss.oss_api import *
import subprocess

gmsconfigxml = '/gms/conf/gmsconfig.xml'
cloud_main = os.path.abspath('/gms/gapi/modules/proxy/cloud/cloud_comm_main.pyc')
proxyType = ''
logger = ''
logpath = '/data/log/cloud/'

#记录proxy信息到xml中
def proxy_xml(dict,proxyType):
    tree = parse.read_xml(gmsconfigxml)
    tnode = parse.find_nodes(tree,"gms_proxy")
    nodes = parse.find_nodes(tree,"gms_proxy/%s"%(proxyType))
    #如果没有<gms_proxy/sock5>结点，则创建。
    if not nodes:
        newnode = parse.create_node(proxyType,{"ip":dict['ip'],"port":dict['port'],"username":dict['user'],"passwd":dict['passwd']},"")  
        parse.add_child_node(tnode,newnode)
    else:
        #查找到sock5结点，修改其属性 
        parse.change_node_properties(nodes,{"ip":dict['ip'],"port":dict['port'],"username":dict['user'],"passwd":dict['passwd']})
        print '[cloud_proxy]---Modify %s attrib .'%(proxyType)
        logger.debug("[cloud_proxy]---Modify %s attrib ."%(proxyType))
    pronodes = parse.find_nodes(tree,"gms_proxy/pro")
    if not pronodes:
        try:
            newnode = parse.create_node("pro",{"flag":dict['method']},"")
        except Exception,ex:
            print ex
        else:
            parse.add_child_node(tnode,newnode)
    else:
        parse.change_node_properties(pronodes,{"flag":dict['method']})
        print '[cloud_proxy]---Modify flag[%s]  attrib .'%(proxyType)
        logger.debug("[cloud_proxy]---Modify flag[%s] attrib ."%(proxyType))
    parse.write_xml(tree,gmsconfigxml)    

#记录proxy_limit信息到xml中
def proxy_limit_xml(dict):
    tree = parse.read_xml(gmsconfigxml)
    tnode = parse.find_nodes(tree,"gmsconf")
    nodes = parse.find_nodes(tree,"gms_limit")
    #如果没有<gms_limit>节点，则创建。
    if not nodes:
        newnode = parse.create_node("gms_limit",{"uplimit":dict['uplimit'],"downlimit":dict['downlimit']},"")  
        parse.add_child_node(tnode,newnode)
    else:
        #如果有相应的节点，则修改它的属性。
        parse.change_node_properties(nodes,{"uplimit":dict['uplimit'],"downlimit":dict['downlimit']},"")
    parse.write_xml(tree,gmsconfigxml)    
    print '[cloud_proxy]---Modify limit speed uplimit[%s] downlimit[%s] .'%(dict['uplimit'],dict['downlimit'])
    logger.debug("[cloud_proxy]---Modify limit speed uplimit[%s] downlimit[%s]."%(dict['uplimit'],dict['downlimit']))
def proxy_test():
    oss = OssAPI("oss.aliyuncs.com","ZnY7oVJVSvQIdztK","CBYbwqpWpMqOk8jIecVANAHec5m5bA")
    ret = oss.get_bucket("oss-event-gms-down")
    return  ret.status

def start_comm_cloud(pid,uplimit,downlimit):
    print '[cloud_proxy]---comm_cloud was been detected,restart it.'
    logger.debug("[cloud_proxy]---comm_cloud was been detected,restart it.")
    #print 'restart ....'
    p=subprocess.Popen(["python %s -h %s -l %s >/dev/null 2>&1 &"%(pid,uplimit,downlimit)],stdout=subprocess.PIPE,shell=True)
    status = p.wait()
    if status == 0:
        print '[cloud_proxy]---exce [python %s -h %s -l %s >/dev/null 2>&1 &] succeed.'%(pid,uplimit,downlimit)
        logger.debug("[cloud_proxy]---exce [python %s -h %s -l %s >/dev/null 2>&1 &] succeed."%(pid,uplimit,downlimit))
    else:
        print '[cloud_proxy]---exce [python %s -h %s -l %s >/dev/null 2>&1 &] Fail.'%(pid,uplimit,downlimit)
        logger.debug("[cloud_proxy]---exce [python %s -h %s -l %s >/dev/null 2>&1 &] Fail."%(pid,uplimit,downlimit))
    return status
#检测存在的cloud_main，以免启动多个
def detect(process,uplimit,downlimit):
    print '[cloud_proxy]---cloud_comm_main [%s]'%(process)
    logger.debug("[cloud_proxy]---cloud_comm_main [%s]"%(process))
    p=subprocess.Popen(["ps aux |grep cloud_comm_main.py |grep -v grep |awk '{print $1}'"],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    status = p.wait()
    print status,'----------------------'
    if status == 0:
        data = p.communicate()[0]
        print 'detect pid ,,,,,,%s'%data
        if data:
            fdata = data.split()
            print fdata,'----------pid'
            for pid in fdata:
                p=subprocess.Popen(["kill -9 %s"%(pid)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
                status = p.wait()
                if status == 0:
                    print '[cloud_proxy]---comm_cloud was been detected,so stop it[%s].'%(pid)
                    logger.debug("[cloud_proxy]---comm_cloud was been detected,so stop it[%s]."%(pid))
                else:
                    print '[cloud_proxy]---stop cloud_comm_main[%s] fail..'%pid
                    logger.warn("[cloud_proxy]---comm_cloud was been detected,stop it[%s] Fail ."%(pid))
            #确保已启动的进程全被kill
            ret = start_comm_cloud(process,uplimit,downlimit)
            if ret == 0:
                print '[cloud_proxy]---start comm_cloud succeed .'
                logger.debug("[cloud_proxy]---start comm_cloud succeed .")
            else:
                print '[cloud_proxy]---start comm_cloud fail .'
                logger.warn("[cloud_proxy]---start comm_cloud Fail .")
            return ret
        else:
            print '------no cloud_comm start-------'
            #目前没有检测到程序启动
            ret = start_comm_cloud(process,uplimit,downlimit)
            if ret == 0:
                print '[cloud_proxy]---start comm_cloud succeed .'
                logger.debug("[cloud_proxy]---start comm_cloud succeed .")
            else:
                print '[cloud_proxy]---start comm_cloud fail.'
                logger.warn("[cloud_proxy]---start comm_cloud Fail .")
            return ret
    else:
        print '[cloud_proxy]---exce [ps aux fail].....'
        logger.warn("[cloud_proxy]---exce [ps aux fail].....")

def init(logpath):
    global logger 
    logger = cloud_log.CloudLogger(path = logpath,level = '',format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')     

#动主程序
def main(argv):
    init(logpath)
    dict={"method":"","ip":"","port":"","user":"","passwd":"","uplimit":"","downlimit":"","test":""}
    opts,args = getopt.getopt(sys.argv[1:],"m:i:p:u:w:h:l:t:",["method=","ip=","port=","user=","passwd="])
    for opt,arg in opts:
        if opt == '-m':
            dict['method'] = arg
        elif opt == '-i':
            dict['ip'] = arg
        elif opt == '-p':
            dict['port'] = arg
        elif opt == '-u':
            dict['user']= arg
        elif opt == '-w':
            dict['passwd'] = arg
        elif opt == '-h':
            dict['uplimit']= arg
        elif opt == '-l':
            dict['downlimit'] = arg 
        elif opt == '-t':
            dict['test'] = arg
    if dict['test'] == 'test':
        if dict['method'] == 'HTTP': #验证输入的参数是HTTP,SOCK5还是SOCK4
            socks.setdefaultproxy(socks.PROXY_TYPE_HTTP,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            res = proxy_test()
            if res == 200:
                print '[cloud_proxy]---Test http proxy aviliable..'
                logger.debug("[cloud_proxy]---Test http proxy aviliable..")
                print '测试代理成功.....return 0'
                proxy_xml(dict,"http")
                return 0
                #设置提示代理服务器可用
            else:
                print '[cloud_proxy]---http proxy unavilable..'
                logger.debug("[cloud_proxy]---Test http proxy unaviliable..")
                print '测试代理失败....return -1'
                proxy_xml(dict,"http")
                return -1
        #sock5
        elif dict['method'] == 'SOCK5':
            socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            res = proxy_test()
            if res == 200:
                print '[cloud_proxy]---sock5 proxy aviliable..'
                logger.debug("[cloud_proxy]---Test sock5 proxy aviliable..")
                print '测试代理成功.....return 0'
                proxy_xml(dict,"sock5")
                return 0
            else:
                print '[cloud_proxy]---sock5 proxy unavilable..'
                logger.debug("[cloud_proxy]---Test scok5 proxy aviliable..")
                print '测试代理失败....return -1'
                proxy_xml(dict,"sock5")
                return 22 
        #sock4
        elif  dict['method'] == 'SOCK4':
            socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS4,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            res = proxy_test()
            if res == 200:
                print '[cloud_proxy]---sock4 proxy aviliable..'
                logger.debug("[cloud_proxy]---Test sock4 proxy aviliable..")
                print '测试代理成功.....return 0'
                proxy_xml(dict,"sock4")
                return 0
            else:
                print '[cloud_proxy]---sock4 proxy unavilable..'
                logger.debug("[cloud_proxy]---Test sock4 proxy aviliable..")
                proxy_xml(dict,"sock4")
                print '测试代理失败....return -1'
                return 22
    else: 
        #http
        if dict['method'] == 'HTTP': #验证输入的参数是HTTP,SOCK5还是SOCK4
            print 'HTTP......'
            socks.setdefaultproxy(socks.PROXY_TYPE_HTTP,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            detect(cloud_main,dict['uplimit'],dict['downlimit'])
            #http_proxy_xml(dict)
            proxy_xml(dict,"http")
            proxy_limit_xml(dict)
            logger.debug("[cloud_proxy]---http_proxy_xml's and proxy_limit_xml's args are writed.")
        #sock5
        elif dict['method'] == 'SOCK5':
            socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            detect(cloud_main,dict['uplimit'],dict['downlimit'])
            #sock5_proxy_xml(dict)
            proxy_xml(dict,"sock5")
            proxy_limit_xml(dict)
            logger.debug("[cloud_proxy]---sock5_proxy_xml's and proxy_limit_xml's args are writed.")
        #sock4
        elif  dict['method'] == 'SOCK4':
            socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS4,dict['ip'],int(dict['port']),dict['user'],dict['passwd'])
            socket.socket = socks.socksocket
            detect(cloud_main,dict['uplimit'],dict['downlimit'])
            #sock4_proxy_xml(dict)
            proxy_xml(dict,"sock4")
            proxy_limit_xml(dict)
            logger.debug("[cloud_proxy]---sock5_proxy_xml's and proxy_limit_xml's args are writed.")

if __name__ == '__main__':
    #init_log("./log")
    exit(main(sys.argv[1:]))
