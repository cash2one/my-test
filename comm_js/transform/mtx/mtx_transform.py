#!/usr/bin/env python
# _*_ coding: utf8 _*_

# 运行在顶级管理结点，将 mtx 检测返回的 *_result.ok 文件转换成本地格式

import os
import sys
import time
import shutil
import json
import socket
import struct
import zipfile
import traceback
import comm_data
from transform import common

# 配置选项
input_dir = comm_data.comm_me_ali_down_path
output_dir = comm_data.comm_me_down_path
backup_dir = comm_data.trans_backup_me_path
#添加对杰斯的接口
js_dir = comm_data.comm_js_path

# 执行统计信息
input_zip_file_count = 0
input_mtx_file_count = 0
input_mtx_line_count = 0
output_file_count = 0
output_line_count = 0
illegal_file_count = 0
illegal_line_count = 0
illegal_file_name_count = 0
rep_output_file_name_count = 0
extract_file_fail_count = 0

logger = common.logger

def check_config():
    logger.info("input dir = %s" % input_dir)
    logger.info("output dir = %s" % output_dir)
    logger.info("backup dir = %s" % backup_dir)
    common.creat_if_dir_notexist(input_dir)
    common.creat_if_dir_notexist(output_dir)
    common.creat_if_dir_notexist(backup_dir)
    common.creat_if_dir_notexist(os.path.join(output_dir, 'att'))
    common.creat_if_dir_notexist(os.path.join(output_dir, '3rd'))

def fwritelines(file_name, data_list):
    try:
        if os.path.exists(file_name):
            # 这个文件名不应该存在，如果现在的处理有问题，应该调整流程
            global rep_output_file_name_count
            rep_output_file_name_count += 1
            logger.warning("repeat file name[%s]." % file_name)
            dname, fname = os.path.split(file_name)
            file_name = common.make_unique_file_name(dname, fname)

        common.fwritelines(file_name, data_list)
    except IOError:
        logger.error(traceback.format_exc())
        logger.error("save mtx transform result file[%s] fail." % file_name)
        # 继续上传异常。因为假如这里没有写权限，那下一步创建压缩包会找不到文件。
        raise

def ename_format(evt_dict):
    ename = ''
    if evt_dict.has_key('eventBaseType'):
        ename = evt_dict['eventBaseType']
        if evt_dict.has_key('eventType'):
            ename = ename + '-' + evt_dict['eventType']
            if evt_dict.has_key('eventExtType'):
                ename = ename + '-' + evt_dict['eventExtType']
    return ename

def nameid_format(evt_dict):
    return '\N'

def isccserver_format(evt_dict):
    if evt_dict.has_key('isCCServer'):
        return str(evt_dict['isCCServer'])
    else:
        return '\N'

evt_type_dict = {
    u'木马': '1',
    u'僵尸网络': '1',
    u'网站': '2',
    u'攻击': '3',
    }

def etype_format(evt_dict):
    etype = '4'
    if evt_dict.has_key('eventBaseType'):
        ebtype = evt_dict['eventBaseType']
        if evt_type_dict.has_key(ebtype):
            etype = evt_type_dict[ebtype]
    return etype

def begin_time_format(evt_dict):
    if evt_dict.has_key('eventTime'):
        begin_time = evt_dict['eventTime']
        begin_time = time.strptime(begin_time, "%Y%m%d %H:%M:%S")
        begin_time = time.strftime("%Y-%m-%d %H:%M:%S", begin_time)
        return begin_time
    else:
        return '\N'

def end_time_format(evt_dict):
    return begin_time_format(evt_dict)

def handledeadline_format(evt_dict):
    if evt_dict.has_key('handleDeadline'):
        #deadline = evt_dict['handleDeadline']
        #deadline = time.strptime(deadline, "%Y%m%d %H:%M:%S")
        #deadline = time.strftime("%Y-%m-%d %H:%M:%S", deadline)
        return time.strftime("%Y-%m-%d %H:%M:%S")
    else:
        return '\N'

def eventtype_format(evt_dict):
    if evt_dict.has_key('eventType'):
        return evt_dict['eventType']
    else:
        return '\N'

def eventbasetype_format(evt_dict):
    if evt_dict.has_key('eventBaseType'):
        return evt_dict['eventBaseType']
    else:
        return '\N'

def eventexttype_format(evt_dict):
    if evt_dict.has_key('eventExtType'):
        return evt_dict['eventExtType']
    else:
        return '\N'

def saddr_format(evt_dict):
    if evt_dict.has_key('eventSrcIP'):
        sip = evt_dict['eventSrcIP']
        sip = str(socket.ntohl(struct.unpack("I", socket.inet_aton(sip))[0]))
        if sip[-1] == 'L':
            sip = sip[:-1]
        return sip
    else:
        return '\N'

def daddr_format(evt_dict):
    if evt_dict.has_key('eventDstIP'):
        dip = evt_dict['eventDstIP']
        dip = str(socket.ntohl(struct.unpack("I", socket.inet_aton(dip))[0]))
        if dip[-1] == 'L':
            dip = dip[:-1]
        return dip
    else:
        return '\N'

def sport_format(evt_dict):
    if evt_dict.has_key('eventSrcPORT'):
        return str(evt_dict['eventSrcPORT'])
    else:
        return '\N'

def dport_format(evt_dict):
    if evt_dict.has_key('eventDstPORT'):
        return str(evt_dict['eventDstPORT'])
    else:
        return '\N'

def eventdetail_format(evt_dict):
    if evt_dict.has_key('eventDetail'):
        detail = evt_dict['eventDetail']
        detail = detail.replace('|', '\|')
        detail = detail.replace('"', '\\"')
        return '"' + detail + '"'
    else:
        return '\N'

def enum_format(evt_dict):
    return '1'

def risk_level_format(evt_dict):
    if evt_dict.has_key('damageIndex'):
        return str(evt_dict['damageIndex'])
    else:
        return '\N'

def query_id_format(evt_dict):
    return '\N'

def month_id_format(evt_dict):
    if evt_dict.has_key('eventTime'):
        begin_time = evt_dict['eventTime']
        begin_time = time.strptime(begin_time, "%Y%m%d %H:%M:%S")
        month_id = time.strftime("%m", begin_time)
        return month_id
    else:
        return '\N'

def day_id_format(evt_dict):
    if evt_dict.has_key('eventTime'):
        begin_time = evt_dict['eventTime']
        begin_time = time.strptime(begin_time, "%Y%m%d %H:%M:%S")
        day_id = time.strftime("%d", begin_time)
        return day_id
    else:
        return '\N'

fname_flag = False  # fname_flag 第一次对解析文件生成文件名时设为 True
fname_dict = {}     # 以天作为 key 值保存每个 json 文件生成的所有文件名
def fname_format(evt_dict, uuid):
    global fname_flag, fname_dict
    # 以每个文件第一行的时间戳作为文件名
    # 如果一个文件内的时间戳跨天，就从跨天的时间开始生成另一个文件名
    # 如果没有这个字段抛出异常
    timestamp = evt_dict['eventTime']
    timestamp = time.strptime(timestamp, "%Y%m%d %H:%M:%S")
    if fname_flag == False:
        # 说明是第一次解析文件。修改标识位和清空 fname_dict
        fname_flag = True
        fname_dict = {}
        line_date = int(time.strftime("%Y%m%d", timestamp))
        fname = time.strftime("%Y%m%d%H%M%S", timestamp) + '_' + uuid + '.ok'
        fname_dict[line_date] = fname
        return fname
    else:
        line_date = int(time.strftime("%Y%m%d", timestamp))
        if fname_dict.has_key(line_date):
            return fname_dict[line_date]
        else:
            fname = time.strftime("%Y%m%d%H%M%S", timestamp) + '_' + uuid + '.ok'
            fname_dict[line_date] = fname
            return fname

# 和数据库中字段顺序相同
dbfield_func_list = (
        #devid_format,  #uuid
        ename_format,
        nameid_format,
        isccserver_format,
        etype_format,
        begin_time_format,
        end_time_format,
        handledeadline_format,
        eventtype_format,
        eventbasetype_format,
        eventexttype_format,
        daddr_format,
        saddr_format,
        sport_format,
        dport_format,
        eventdetail_format,
        enum_format,
        risk_level_format,
        query_id_format,
        month_id_format,
        day_id_format,
        )

def transform_mtx_line(uuid, string):
    evt_dict = json.loads(string)

    retlist = [uuid]
    for func in dbfield_func_list:
        field = func(evt_dict)
        retlist.append(field)
    retstring = '|'.join(retlist) + '\n'

    etype = etype_format(evt_dict)
    fname = fname_format(evt_dict, uuid)
    if etype == '3':
        fname = os.path.join(output_dir, 'att', fname)
    #elif etype == '2':
    #    fname = os.path.join(output_dir, 'web', fname)
    else:
        fname = os.path.join(output_dir, '3rd', fname)
    #print("fname[%s] content[%s]" % (fname, retstring))
    return [fname, retstring.encode('utf-8')]

def transform_mtx_file(mtx_data, mtx_file, output_dir):
    global illegal_file_count
    global illegal_line_count
    global output_file_count
    global output_line_count
    global input_mtx_line_count

    illegal_line_flag = False
    fname_dict = {}
    line_num = 0

    # 从 timestamp_devid.ok 形式的文件名中获得设备ID
    # 云主机负责将原来的 mtx-id 替换为设备的 uuid
    # 而且保证文件内的 mtx-id 都和文件名中的 mtx-id 相同。
    # 这样只需要扔掉文件内部的 mtx-id 替换为 uuid
    uuid = os.path.splitext(os.path.basename(mtx_file))[0].split('_')[1]

    # 逐行转换 mtx 文件内存保存到 fname_dict
    for line in mtx_data.split('\n'):
        line_num += 1
        if len(str.strip(line)) > 0:
            try:
                input_mtx_line_count += 1
                fname, line = transform_mtx_line(uuid, line)
                if fname_dict.has_key(fname):
                    fname_dict[fname].append(line)
                else:
                    output_file_count += 1
                    fname_dict[fname] = []
                    fname_dict[fname].append(line)
            except Exception:
                illegal_line_count += 1
                if illegal_line_flag == False:
                    illegal_file_count += 1
                    illegal_line_flag = True
                    # 如果文件中含有错误行，转存到 backup_dir 目录
                    backupname = common.make_unique_file_name(backup_dir, os.path.split(mtx_file)[-1])
                    shutil.copy(mtx_file, backupname)
                    logger.error("mtx file[%s] format illegal. backup as file[%s]." % (mtx_file, backupname))
                logger.error("mtx file[%s] line[%d] format illegal." % (mtx_file, line_num))
                logger.error(traceback.format_exc())

    for dst_file, lines in fname_dict.items():
        try:
            data = ''.join(lines)
            zip_file = dst_file
            arc_name = os.path.split(dst_file)[-1]
            #print("zip file[%s] arc name[%s]" % (zip_file, arc_name))
            common.buftozip(zip_file, arc_name, data)
            output_line_count += len(lines)
        except Exception:
            if os.path.exists(zip_file):
                os.remove(zip_file)
            logger.error(traceback.format_exc())

last_sec = 0
def exec_info_dump(wait_sec = 60):
    global last_sec
    cur_sec = time.time()
    if cur_sec - last_sec > wait_sec:
        last_sec = cur_sec
        logger.info("---------- mtx transform info statistic ----------")
        logger.info("input zip file count = %d" % input_zip_file_count)
        logger.info("input mtx file count = %d" % input_mtx_file_count)
        logger.info("input mtx line count = %d" % input_mtx_line_count)
        logger.info("output file count = %d" % output_file_count)
        logger.info("output line count = %d" % output_line_count)
        if illegal_file_count > 0:
            logger.info("(*warning*) illegal mtx file count = %d" % illegal_file_count)
        if illegal_line_count > 0:
            logger.info("(*warning*) illegal mtx line count = %d" % illegal_line_count)
        if illegal_file_name_count > 0:
            logger.info("(*warning*) illegal mtx file name count = %d" % illegal_file_name_count)
        if rep_output_file_name_count > 0:
            logger.info("(*warning*) repeat output file name count = %d" % rep_output_file_name_count)
        if extract_file_fail_count > 0:
            logger.info("(*warning*) extract file fail count = %d" % extract_file_fail_count)
#------------------------------------------------------
#再尽量少改动原架构的基础上,把数据提供给杰斯接口目录下|
#-----------------------------------------------------
def save_mtx_js(filename,data):
    if not os.path.isdir(js_dir):
        os.mkdirs(js_dir) 
    try:
        filename = os.path.join(js_dir,os.path.basename(filename))
        with open(filename,'wb') as fp:
            json.dump(data,fp,indent=1)
        logger.info("save_mtx_js * dump into %s *" %filename)
    except Exception,ex:
        #print ex
        logger.error('%s' %traceback.format_exc())
#可以在这完成发送,但是考虑到松耦合的情况,把杰斯业务单独分离出去,
#再开一个线程去检测js_dir,进行注册、数据的封装发送等业务.     
#性能可能会有点影响.(不如直接在这发快,)
#------------------------------------------------------
#                      end!                           |
#------------------------------------------------------
def transform():
    global input_zip_file_count
    global input_mtx_file_count
    global extract_file_fail_count
    global fname_flag
    try:
        logger.info("========== mtx transform thread start. ==========")
        check_config()
        while common.is_continue():
            try:
                for zip_file in common.file_filter_bysuffix([input_dir], ['.ok']):
                    if not common.is_continue():
                        break
                    input_zip_file_count += 1
                    try:
                        fp = None
                        fp = zipfile.ZipFile(zip_file, 'r')
                        fp.setpassword('9527')
                        for mtx_file in fp.namelist():
                            input_mtx_file_count += 1
                            mtx_data = fp.read(mtx_file)
                            if comm_data.js_event_api == 'yes':
                                save_mtx_js(mtx_file,mtx_data)
                            transform_mtx_file(mtx_data, mtx_file, output_dir)
                            exec_info_dump()
                    except zipfile.BadZipfile:
                        extract_file_fail_count += 1
                        logger.error(traceback.format_exc())
                        backupname = common.make_unique_file_name(backup_dir, os.path.split(zip_file)[-1])
                        shutil.copy(zip_file, backupname)
                        logger.error("extract zipfile[%s] fail. backup as file[%s]." % (zip_file, backupname))
                    except Exception:
                        logger.error(traceback.format_exc())
                    finally:
                        if fp != None: fp.close()
                        os.remove(zip_file)
                        fname_flag = False
                exec_info_dump()
                time.sleep(0.1)
            except Exception:
                # 多数时候是因为测试时删除已有的文件。于是上面的 os.remove zip_file 失败，抛出异常。
                logger.info(traceback.format_exc())
    except Exception:
        logger.info(traceback.format_exc())
        # 如果初始化时出错，把异常抛出去，让主线程处理
        raise
    finally:
        exec_info_dump()
        logger.info("========== mtx transform thread stop. ==========")

if __name__ == '__main__':
    transform()

