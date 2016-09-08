# -*- coding:utf-8 -*-
import ConfigParser
import os
import time
import datetime
import sys
import string
import signal
import socket
import codecs
import logging
from logging.handlers import RotatingFileHandler
from ftplib import FTP
import shutil
import cProfile

sys.path.append('../cloud/')
import cloud_log

from subprocess import Popen

sys.path.append('../')
import comm_common
import comm_data
import comm_init
import comm_log_record


UPLOAD_DATA_CONF = '/gms/conf/ftp_server.conf'
#HOST_CONF = '/nmsmw/etc/host.conf'


### init log ###
def remote_log_init():
    global remote_logger
    remote_logger = cloud_log.CloudLogger(path = comm_data.remote_log_path,
                        level = '',
                        format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')

def _sig_INT_handle(signum, frame):
    remote_logger.debug("SIGINT recieve, exit()")
    sys.exit()

def create_dir():
    comm_common.creat_if_dir_notexist(comm_data.host_mtx_src_path)
    comm_common.creat_if_dir_notexist(comm_data.ftp_abb_path)
    comm_common.creat_if_dir_notexist(comm_data.ftp_mtd3rd_path)
    comm_common.creat_if_dir_notexist(comm_data.ftp_mtdatt_path)
    comm_common.creat_if_dir_notexist(comm_data.ftp_surl_path)
    comm_common.creat_if_dir_notexist(comm_data.ftp_virus_path)

def create_ftp(remote_dir):
    create_dir = init_ftp(remote_ip,remote_port,user,passwd)
    first_dir = remote_dir.split("/")[1]
    if first_dir not in create_dir.nlst():
  #  if not os.path.exists(first_dir) or not os.path.isdir(first_dir):
        try:
            create_dir.mkd(first_dir)
        except Exception,e:
            remote_logger.debug("cwd %s failed:%s"%(remote_dir,e))
            return -1
    if first_dir == "mtx":
        second_dir = remote_dir.split("/")[2]
        try:
            create_dir.cwd(first_dir)
        except Exception,e:
            remote_logger.debug("cwd %s failed:%s"%(remote_dir,e))
            return -1
        if second_dir not in create_dir.nlst():
    #    if not os.path.exists(second_dir):
            try:
                create_dir.mkd(second_dir)
            except Exception,e:
                remote_logger.debug("cwd %s failed:%s"%(remote_dir,e))
                return -1
    create_dir.quit()
   
def create_ftp_dir():
    create_ftp("/"+comm_data.ABB_FILE+"/")
    create_ftp("/"+comm_data.MTX_FILE+"/3rd/")
    create_ftp("/"+comm_data.MTX_FILE+"/att/")
    create_ftp("/"+comm_data.SURL_FILE+"/")
    create_ftp("/"+comm_data.VIRUS_FILE+"/")

def init_remote_config(config_file):
    host_config = {}
    try:
        config = ConfigParser.ConfigParser()
        config.read(config_file)

        host_config['user'] = config.get('ftp_server', 'user')
        host_config['port'] = config.get('ftp_server', 'port')
        host_config['passwd'] = config.get('ftp_server', 'pwd')
        host_config['remote_ip'] = config.get('ftp_server', 'ip')
        host_config['enable'] = config.get('ftp_server','enable')
    except Exception,msg:
        remote_logger.debug(msg)
        sys.exit(1)

    return host_config

def init_ftp(ip,port,user,passwd):
    ftp = FTP()
    try:
        ftp.connect(ip,port)
        ftp.login(user,passwd)
        print ftp.getwelcome() 
    except Exception,e:
        remote_logger.debug("connect ftp failed:%s" %e)
        return 0 
    return ftp

def upload_file(remote_dir,filename):
    #global upload_ftp
    remote_logger.debug("upload file:%s starting..."%filename)
    upload_ftp = init_ftp(remote_ip,remote_port,user,passwd)
    if upload_ftp == 0:
        return -1
    try:
        upload_ftp.cwd(remote_dir)
    except Exception,e:
        remote_logger.debug("cwd %s failed:%s"%(remote_dir,e))
        return -1
    file_handler = open(filename,'rb')
    file_name = filename.split('/')[-1]
    down_name = file_name + ".tmp"
    try:
        upload_ftp.storbinary('STOR '+down_name,file_handler)
    except Exception,e:
        remote_logger.debug("upload failed:%s" %e)
        return -1
    print down_name
    try:
        upload_ftp.rename(down_name,file_name)
    except Exception,e:
        remote_logger.debug("upload failed:%s" %e)
        return -1
    file_handler.close()
    try:
        upload_ftp.quit()
    except Exception,e:
        remote_logger.debug("upload failed:%s"%e)
        return -1
    remote_logger.debug("upload file:%s" %filename)
    
    return 0

def upload_files():

    def check_dir(src_dir):
        for fname in os.listdir(src_dir):
            #print "src_dir[%s]"%src_dir
            fname = os.path.join(src_dir,fname)
            if os.path.isfile(fname):
                yield fname

    abb_dst_path = "/"+comm_data.ABB_FILE+"/"
    mtx_3rd_dst_path = "/"+comm_data.MTX_FILE+"/3rd/"
    mtx_att_dst_path = "/"+comm_data.MTX_FILE+"/att/"
    surl_dst_path = "/"+comm_data.SURL_FILE+"/"
    virus_dst_path = "/"+comm_data.VIRUS_FILE+"/"

    dir_list = ((comm_data.ftp_abb_path,abb_dst_path),
                (comm_data.ftp_mtd3rd_path,mtx_3rd_dst_path),
                (comm_data.ftp_mtdatt_path,mtx_att_dst_path),
                (comm_data.ftp_surl_path,surl_dst_path),
                (comm_data.ftp_virus_path,virus_dst_path),
            )
    while 1:
        host_config = init_remote_config(UPLOAD_DATA_CONF)
        enable = host_config['enable']
        if enable == 'yes':
            for src_dir,dst_dir in dir_list:
               #改一下 
                for src_file in check_dir(src_dir):
                    if os.path.splitext(src_file)[1] == '.ok':
                        try:
                            upload_file(dst_dir,src_file)
                        except Exception, ex:
                            print ex
                            remote_logger.debug("%s" %ex)
                            continue
                        os.remove(src_file)
                    else:
                        break
                time.sleep(0.1)
        elif enable == 'no':
            print "sleep```````````"
            time.sleep(1)
            continue
def main():
    global remote_logger
    global type_count
    global user
    global passwd
    global remote_ip
    global remote_port

    comm_init.read_pre_json()
    
    remote_log_init()
    
    create_dir()
    
    host_config = init_remote_config(UPLOAD_DATA_CONF)
    user = host_config['user']
    passwd = host_config['passwd']
    remote_ip = host_config['remote_ip']
    remote_port = host_config['port']    
    print user,passwd,remote_ip,remote_port   
    
    create_ftp_dir()
   
    try: 
        upload_files()
    except Exception ,ex:
        remote_logger.debug(ex)
        return 

if __name__ == '__main__':
    signal.signal(signal.SIGINT, _sig_INT_handle)
    main()
