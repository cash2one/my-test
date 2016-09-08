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
import test
sys.path.append("../")
from oss.oss_api import *


uplimit = ""
downlimit = ""

#记录comm_limit信息到xml中
#def comm_limit_xml(uplimit,downlimit):
#    uplimit = sys.argv[12]
#    downlimit = sys.argv[14]
#
#    tree = parse.read_xml("./gmsconfig.xml")
#    nodes = parse.find_nodes(tree,"gms_limit")
#    #如果没有<gms_limit>节点，则创建。
#    if not nodes:
#        newnode = parse.create_node("gms_limit",{"uplimit":uplimit,"downlimit":downlimit},"")  
#        parse.add_child_node(tnode,newnode)
#    else:
#        #如果有相应的节点，则修改它的属性。
#        parse.change_node_properties(nodes,{"uplimit":uplimit,"downlimit":downlimit},"")
#            
#    parse.write_xml(tree,"./gmsconfig.xml")    



#启动主程序
def main(argv):

    global uplimit
    global downlimit

    opts,args = getopt.getopt(sys.argv[1:],"h:l:",["uplimit=","downlimit="])
    list = ['HTTP','SOCK5','SOCK4']
    for op,val in opts:
        if op == "-h":
            uplimit = val
        if op == "-l":
            downlimit = val
    test.m(uplimit,downlimit)
    #www.http()
    #http_proxy_xml(sys.argv[4],sys.argv[6],sys.argv[8],sys.argv[10])
    #proxy_limit_xml(sys.argv[12],sys.argv[14])
    #comm_cloud.mainproc(uplimit,downlimit) #启动HTTP 模式下的代理程序
    #return 0  #0 表示启动成功
        
if __name__ == '__main__':
    while True:
        main(sys.argv[1:])
        time.sleep(1)
