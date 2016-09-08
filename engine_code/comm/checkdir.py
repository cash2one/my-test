#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import time
import syslog
import shutil
import json
import socket
import struct
import shutil
import comm_data
import comm_init

""" 用来测试的时候创建或者清空使用的目录 """

def config_path_dict():
    comm_init.read_pre_json()
    path_dict = {}
    for key in dir(comm_data):
        if key.find('_path', -5) != -1:
            path_dict[key] = getattr(comm_data, key)
    return path_dict

# 只提示创建不存在的目录
def config_path_creat():
    create_path_success_num = 0
    create_path_fail_num = 0
    for name, path in config_path_dict().items():
        if not os.path.exists(path):
            print("dir[%s:%s] not exist. create." % (name, path))
            try:
                os.makedirs(path)
            except Exception as reason:
                create_path_fail_num += 1
                print("create dir[%s] fail. %s." % (file, str(reason)))
            else:
                create_path_success_num += 1
    print("create dir num: success[%d] fail[%d]." % (create_path_success_num, create_path_fail_num))

# 只删除目录下的文件
def config_path_clear():
    remove_file_success_num = 0
    remove_file_fail_num = 0
    for name, path in config_path_dict().items():
        file_flag = False
        for file in os.listdir(path):
            file = os.path.abspath(os.path.join(path, file))
            if os.path.isfile(file):
                if file_flag == False:
                    print("dir[%s] not empty. clear." % path)
                    file_flag = True
                try:
                    os.remove(file)
                except Exception as reason:
                    remove_file_fail_num += 1
                    print("remove file[%s] fail. %s." % (file, str(reason)))
                else:
                    remove_file_success_num += 1
    print("remove file num: success[%d] fail[%d]." % (remove_file_success_num, remove_file_fail_num))

# 只显示目录下文件大于零的目录
def config_path_info():
    dir_num = 0
    total_file_num = 0
    for name, dir in config_path_dict().items():
        if os.path.exists(dir):
            file_num = file_count(dir)
            if file_num > 0:
                dir_num += 1
                total_file_num += file_num
                print("dir[%s] file num[%d]" % (dir, file_num))
        else:
            print("dir[%s] not exist." % dir)
    print("dir num [%d]. file num[%d]" % (dir_num, total_file_num))

def file_count(dir):
    return len(os.listdir(dir))

def usage():
    print("usage: %s [creat|clear|info]" % sys.argv[0])
    exit()

def checkdir():
    if len(sys.argv) == 2:
        if sys.argv[1] == 'creat':
            config_path_creat()
        elif sys.argv[1] == 'clear':
            config_path_clear()
        elif sys.argv[1] == 'info':
            config_path_info()
        else:
            usage()
    else:
        usage()

if __name__ == '__main__':
    checkdir()

