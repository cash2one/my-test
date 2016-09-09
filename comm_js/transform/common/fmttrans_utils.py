# _*_ coding: utf8 _*_

import os
import sys
import time
import syslog
import zipfile
import logging
import threading
import ConfigParser
sys.path.append("..")
sys.path.append("../../")
from comm_common import *
from comm_data import *
from comm_log_record import logger


def isemptydir(path):
    if os.path.isdir(path) and os.listdir(path) == []:
        return True
    else:
        return False

def fwritelines(fname, data_list):
    # 先写入该目录下的临时文件，后缀为 .tmp，写完后改名
    tmpname = fname + '.tmp'
    with file(tmpname, 'w') as fp:
        fp.writelines(data_list)
    os.rename(tmpname, fname)

def buftozip(zipname, arcname, buf):
    tmpfile = zipname + ".tmp"
    fp = zipfile.ZipFile(tmpfile, 'w', zipfile.ZIP_DEFLATED)
    fp.writestr(arcname, buf)
    fp.close()
    os.rename(tmpfile, zipname)

def filetozip(zipname, arcname, srcfile):
    ''' 压缩 srcfile 到压缩包 zipname。压缩包内的文件名为 arcname。'''
    tmpfile = zipname + ".tmp"
    fp = zipfile.ZipFile(tmpfile, 'w', zipfile.ZIP_DEFLATED)
    fp.write(srcfile, arcname)
    fp.close()
    os.rename(tmpfile, zipname)

LOG_FORMAT = '==%(asctime)s %(funcName)s[%(lineno)d]== %(message)s'
LOG_LEVEL = logging.DEBUG

def getFileLogger(logname, logfile):
    logfile = os.path.abspath(logfile)
    logdir = os.path.split(logfile)[0]
    creat_if_dir_notexist(logdir)
    handler = logging.FileHandler(logfile)
    handler.setFormatter(logging.Formatter(LOG_FORMAT))
    logger = logging.getLogger(logname)
    logger.addHandler(handler)
    logger.setLevel(LOG_LEVEL)
    return logger

def make_unique_file_name(dname, fname):
    name = os.path.join(dname, fname)
    if os.path.exists(name):
        # 这个文件名不应该存在，如果现在的处理有问题，应该调整流程
        cnt = 1
        basename, extname = os.path.splitext(name)
        while True:
            newname = "%s_(%d)%s" % (basename, cnt, extname)
            if os.path.exists(newname):
                cnt += 1
                continue
            else:
                name = newname
                break
    return name

if __name__ == '__main__':
    syslog.openlog(sys.argv[0], syslog.LOG_PID|syslog.LOG_PERROR)
    syslog.syslog("hello world.")
    syslog.closelog()

    filetozip('test1.zip', 'test.ok', 'test.txt')
    buftozip('test2.zip', 'test2.txt', 'buftozip testtttttttttttt')

