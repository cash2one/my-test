import json
import getopt
import sys
import os
import shutil
import subprocess

comm_pre_path = '/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json'
comm_pre_test_path = '/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json.test'
comm_pre_comm_path = '/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json.comm'
comm_pre_zook_path = '/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json.zook'

def main(argv):
    opts , args = getopt.getopt(sys.argv[1:],'s',['switch=']) 
    for opt ,arg in opts:
        if arg == 't':
            print 'change oss path is test_bucket [oss-ceshi-event-up and oss-ceshi-event-down].' 
            shutil.copy(comm_pre_test_path,comm_pre_path) 
        elif arg == 'c':
            print 'change oss path is common_bucket. [oss-event-gms-up and oss-event-gms-down]'
            shutil.copy(comm_pre_comm_path,comm_pre_path) 
        elif arg == 'z':
            print 'change oss path is zookeeper_bucket. [oss-event-gms-zook-up and oss-event-gms-zook-down]'
            shutil.copy(comm_pre_zook_path,comm_pre_path)

    p = subprocess.Popen(["ps aux |grep cloud_comm_main.pyc |grep -v grep |awk '{print $1}'"],stdout=subprocess.PIPE,shell=True)
    pid =  p.communicate()[0].strip()
    if p.wait() == 0:
        os.system("kill -9 %s" %(pid))
        os.system("/bin/sh /gms/gapi/modules/auth/gms_logmon.sh >/dev/null 2>&1 &")
        print 'restart cloud_comm_main.pyc suceed .'

if __name__ == '__main__':
    main(sys.argv[1:])

