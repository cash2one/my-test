# -*- coding:utf-8 -*-

from ctypes import *
import os
import comm_data
import comm_log_record
import comm_common
import device_manage
import pickle
import traceback


#上级信息字典
parent_info_dic = {}

#---------------------------------------.
# @brief 检测上级节点是否更换           |
# @param parent_ip  当前上级节点ip      |
# @param parent_uuid当前上级节点uuid    |
# @return 0 未变  1 变动
#---------------------------------------*
def check_parent_changed(parent_ip, parent_uuid):
    global parent_info_dic
    retn = 0

    def stdump(st, fname):
        fp = open(fname, 'w')
        pickle.dump(st, fp)
        fp.close()

    def streload(fname):
        fp = open(fname, 'r')
        try:
            st = pickle.load(fp)
        except Exception as reason:
            comm_log_record.logger.info(reason)
            fp.close()
            return "no_value"
        fp.close()
        return st

    def load_parent_info_dic():
        global parent_info_dic
        if (os.path.exists(comm_data.parent_info_file) != True):
            fp = open(comm_data.parent_info_file, 'w')
            fp.close()
        parent_info_dic = streload(comm_data.parent_info_file)
        if (parent_info_dic == "no_value"):
            parent_info_dic = {}
            stdump(parent_info_dic, comm_data.parent_info_file)

    def set_parent_info(ip, uuid):
        global parent_info_dic
        tmp_ip_dic = {"IP":ip}
        tmp_uuid_dic = {"UUID":uuid}
        parent_info_dic.update(tmp_ip_dic)
        parent_info_dic.update(tmp_uuid_dic)
        stdump(parent_info_dic, comm_data.parent_info_file)

    def get_parent_ip():
        global parent_info_dic
        try:
            result = parent_info_dic["IP"]
        except Exception,ex:
            result = "no_value"
            comm_log_record.logger.info(ex)
        return result

    def get_parent_uuid():
        global parent_info_dic
        try:
            result = parent_info_dic["UUID"]
        except Exception,ex:
            result = "no_value"
            comm_log_record.logger.info(ex)
        return result

    load_parent_info_dic()

    ip = get_parent_ip()
    uuid = get_parent_uuid()
    if (ip == "no_value" or uuid == "no_value"):
        #不存在父节点信息
        comm_log_record.logger.info("No parent infomation, need update [%s] to set parent ip[%s], uuid[%s]" %(comm_data.parent_info_file, parent_ip, parent_uuid))
        retn = 1
    else:
        #存在父节点信息, 比较父节点信息
        if (ip == parent_ip and uuid == parent_uuid):
            comm_log_record.logger.info("Parent infomation not changed.")
            retn = 0
        else:
            comm_log_record.logger.info("Parent infomation is changed.Need regsit again!  ip: [%s]->[%s], uuid: [%s]->[%s]" %(ip, parent_ip, uuid, parent_uuid))
            retn = 1

    #更新父节点信息
    set_parent_info(parent_ip, parent_uuid)

    return retn

#------------------------------.
# @brief 主程序捕获函数        |
# @param buf        源数据     |
# @param file_path  保存的文件 |
#------------------------------*
def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        comm_log_record.logger.info(e)
    else:
        tmp_file.write(buf)
        tmp_file.close()

#---------------------------------.
# @brief 将命令参数转换为命令文件 |
#---------------------------------*
def make_reg_para_to_file(para, dst_path):
    para_tmp = para.replace("+", "|")

    # 构建reg文件名
    items = para_tmp.split("|")
    reg_fname = dst_path + "reg_" + items[2] + ".ok"
    reg_tmp_fname = reg_fname + ".tmp"

    write_buf_to_file(para_tmp, reg_tmp_fname)

    os.rename(reg_tmp_fname, reg_fname)
    comm_log_record.logger.info('create sub dev reg file[%s] data[%s]' % (reg_fname, para_tmp.decode('utf-8')))

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

#----------------------------------------------.
# @brief 调用需捕获异常                        |
#        根据指定数据生成reg.dat设备注册文件   |
#----------------------------------------------*
def make_reg_dat_by_buf(reg_buf):
    reg_dat_file = comm_data.reg_dat_path + "reg.dat"
    fd = open(reg_dat_file, "w")
    fd.write(reg_buf)
    fd.close()
    comm_log_record.logger.info("==ret==>CREATE REG DAT![%s]" %reg_dat_file)


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
    except Exception,ex:
        comm_log_record.logger.info(ex)


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
                comm_log_record.logger.info("====REG_FILE is NULL====")
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
        comm_log_record.logger.info('%s' % traceback.format_exc())
        return 0

# -------------------------------------------
##
# @brief  设备主动注册，顶级节点注册模式
#
# @returns
# -------------------------------------------
def active_device_regsiter(uuid):

    if (is_reg_done() == 1):
        return

    reg_name = "reg_" + uuid
    comm_common.creat_if_dir_notexist(comm_data.comm_reg_up_path)
    reg_path = comm_data.comm_reg_up_path + reg_name + ".ok"

    if not os.path.exists(reg_path):
        reg_tmp_path = comm_data.comm_reg_up_path + reg_name + ".ok" + ".tmp"

        reg_buf = make_local_reg_data()

        try:
            tmp_file = open(reg_tmp_path, 'w')
        except IOError, ex:
            comm_log_record.logger.info(ex)
        else:
            tmp_file.write(reg_buf.encode("utf-8"))
            tmp_file.close()
            os.rename(reg_tmp_path, reg_path)
            comm_log_record.logger.info('create local reg file[%s] data[%s]' % (reg_path, reg_buf))

def create_local_reg_file():
    active_device_regsiter(comm_data.uuid)

def create_child_reg_file(uuid):
    reg_data = device_manage.get_comm_topo_regpara(uuid)
    make_reg_para_to_file(reg_data, comm_data.comm_reg_up_path)

def create_topo_reg_file():
    create_local_reg_file()
    for uuid in device_manage.get_all_sub_uuid():
        create_child_reg_file(uuid)

#################### file end ##########################
