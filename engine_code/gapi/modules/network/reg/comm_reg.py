# -*- coding:utf-8 -*-

from ctypes import *
import os
import comm_data
import comm_init
import traceback


#----------------------------------------------.
# @brief 调用需捕获异常                        |
#        得到注册文件内容到内存                |
#----------------------------------------------*
def get_regbuf_from_file(file):
    reg_buf = ""
    fd = open(file, "r")
    reg_buf = fd.read()
    fd.close()
#items = reg_buf.split("|")
#devname = items[0]
#pid = items[1]
#uuid = items[2]
#ip = items[3]
#custom = items[4]
#devtype = items[5]
#cloud_pid = items[6]
#reg_id = items[7]
    return reg_buf


def do_something_if_dir_notexist(dir, func):
    if not os.path.exists(dir) or not os.path.isdir(dir):
        func(dir)

def creat_if_dir_notexist(dir):
    do_something_if_dir_notexist(dir, os.makedirs)
#--------------------------------------.
# @brief 封装本地注册文件数据          |
#--------------------------------------*
def make_local_reg_data():

    reg_buf = ""
    #设备名称
    reg_buf += comm_data.dev_name #deviceName
    reg_buf += "|"

    #父设备ID
    if (comm_data.local_pid == ""):
        reg_buf += "\N"
    else:
        reg_buf += comm_data.local_pid
    reg_buf += "|"

    #设备UUID
    reg_buf += comm_data.uuid
    reg_buf += "|"

    #设备本地IP
    reg_buf += comm_data.dev_ip
    reg_buf += "|"

    #客户名称
    reg_buf += comm_data.custom_name
    reg_buf += "|"

    #设备类型
    if (comm_data.ismonitor == '1'):
        device_type = "monitor"
    elif (comm_data.isroot == '1' and comm_data.ismanager == '1'):
        device_type = "root"
    elif (comm_data.ismanager == '1'):
        device_type = "manager"
    reg_buf += device_type
    reg_buf += "|"
    ##import reg_test
    #import rsa_test
    #from rsa_test import rsa_test
    #DGName =  rsa_test.rsa_test()
    #if DGName:
    #    reg_buf += DGName[0] + "|"
    #    reg_buf += DGName[1] + "|"

    #云平台内部父设备ID
    reg_buf += "\N"
    reg_buf += "|"

    #注册成功码
    reg_buf += "\N"

    return reg_buf

def remove_reg_file():
    reg_dat_file = comm_data.reg_dat_path + "reg.dat"

    try:
        if os.path.exists(reg_dat_file):
            os.remove(reg_dat_file)
            print "=======[remove_reg_file]===========%s=================="
    except Exception,ex:
        print ex


#--------------------------------------.
# @brief  判断本端设备是否已经注册完毕 |
#         XXX 需要交要注册文件信息     |
# @return 1 已经注册                   |
#         0 没有注册                   |
#--------------------------------------*
def is_reg_done():
    try:
        reg_dat_file = comm_data.reg_dat_path + "reg.dat"
        reg_buf = ""
        ret = 0
        if (os.path.exists(reg_dat_file) == True):
            reg_buf = get_regbuf_from_file(reg_dat_file)
            if (reg_buf == ''):
                print "====REG_FILE is NULL===="
                ret = 0
            else:
                items = reg_buf.split("|")
                #devname = items[0]
                #pid = items[1]
                #uuid = items[2]
                #ip = items[3]
                #custom = items[4]
                #devtype = items[5]
                #cloud_pid = items[6]
                reg_id = items[7]
                #print reg_id
                if (reg_id == '\\N\n' or reg_id == '\\N' or reg_id == '' or reg_id == '\n'):
                    ret = 0
                else:
                    ret = 1
        return ret
    except:
        # 可能在文件内容出错的时候走到这里（上面的items[7]），也认为没有注册成功
        print ('%s' % traceback.format_exc())
        return 0

# -------------------------------------------
##
# @brief  设备主动注册，顶级节点注册模式
#
# @returns
# -------------------------------------------
def active_device_regsiter(uuid):

#    if (is_reg_done() == 1):
#        return

    reg_name = "reg_" + uuid
    creat_if_dir_notexist(comm_data.comm_reg_up_path)
    reg_path = comm_data.comm_reg_up_path + reg_name + ".ok"

#if not os.path.exists(reg_path):
    reg_tmp_path = comm_data.comm_reg_up_path + reg_name + ".ok" + ".tmp"

    reg_buf = make_local_reg_data()

    try:
        tmp_file = open(reg_tmp_path, 'w')
    except IOError, ex:
        #comm_log_record.logger.info(ex)
        print ex
    else:
        tmp_file.write(reg_buf.encode("utf-8"))
        tmp_file.close()
        os.rename(reg_tmp_path, reg_path)
        #comm_log_record.logger.info('create local reg file[%s] data[%s]' % (reg_path, reg_buf))
        print ('create local reg file[%s] data[%s]' % (reg_path, reg_buf.encode("utf-8")))

def create_local_reg_file():
    comm_init.conf_init()
    active_device_regsiter(comm_data.uuid)

if __name__ == "__main__":
    create_local_reg_file()

#################### file end ##########################
