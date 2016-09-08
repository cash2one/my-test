#!/usr/bin/env python
#coding:utf8

import sys 
import time
import getopt
import subprocess
import os
import cloud_log

uplimit = ''
downlimit = ''
logger = ''
cloud_main = os.path.abspath('/gms/gapi/modules/proxy/cloud/cloud_comm_main.pyc')
#test = os.path.abspath('/root/test/gapi/modules/proxy/cloud/testa.py')
#-----------------------------------
# @bref start comm cloud busi      |
#-----------------------------------
def start_comm_cloud(cloud_main,uplimit,downlimit):
    print 'cloud process path is [%s]'%(cloud_main)
    logger.debug("cloud process path is [%s]"%(cloud_main))
    print "uplimit[%s],downlimit[%s]"%(uplimit,downlimit)
    p=subprocess.Popen(["python %s -h %s -l %s >/dev/null 2>&1 &"%(cloud_main,uplimit,downlimit)],stdout=subprocess.PIPE,shell=True)
    status = p.wait()
    return status
#-----------------------------------------
#@ bref 检测存在的cloud_main，以免启动多个|
#-----------------------------------------
def detect(process,uplimit,downlimit): 
    p=subprocess.Popen(["ps aux |grep cloud_comm_main.py |grep -v grep |awk -F 'r' '{print $1}'"],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    status = p.wait()
    print 'status::',status
    if status == 0:
        data = p.communicate()[0]
        print '======%s======='%data
        if  data:
            fdata = data.split()
            #print fdata
            for pid in fdata:
                p=subprocess.Popen(["kill -9 %s"%(pid)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
                status = p.wait()
                if status == 0:
                    print 'comm_cloud was been detected,so stop it.'
                    logger.debug("comm_cloud was been detected,so stop it.")
                else:
                    print 'stop cloud_comm_main[%s] fail..'%pid
                    logger.debug('stop cloud_comm_main[%s] fail..'%pid)
   
            #确保已启动的进程全被kill
            ret = start_comm_cloud(process,uplimit,downlimit)
            if ret == 0:
                print 'start comm_cloud succeed .'
                logger.debug("start comm_cloud succeed .")
            else:
                print 'start comm_cloud fail .'
                logger.debug("start comm_cloud fail .")
        else:
            #print '()()()()()()()('
            #print cloud_main,uplimit,downlimit
            ret = start_comm_cloud(process,uplimit,downlimit)
            if ret == 0:
                print 'start comm_cloud succeed .'
                logger.debug("start comm_cloud succeed .")
            else:
                print 'start comm_cloud fail .'
                logger.debug("start comm_cloud fail .")
    else:
        print 'exce [ps aux fail].....'
        logger.debug("exce [ps aux fail] ....")

def cloud_log_init(logpath):
    global logger
    logger = cloud_log.CloudLogger(path = logpath,level = '',format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')    
#-----------------------------
# @bref main process entry   |
#----------------------------
def test():
    global uplimit
    global downlimit
    cloud_log_init("/data/log/cloud/")
    opts,args = getopt.getopt(sys.argv[1:],"h:l:","desc=")
    for opt,arg in opts:
        if opt == '-h':
            uplimit = arg
        if opt == '-l':
            downlimit = arg
    detect(cloud_main,uplimit,downlimit)
if __name__ == '__main__':
    exit(test())

