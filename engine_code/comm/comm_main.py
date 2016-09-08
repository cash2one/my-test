# -*- coding:utf-8 -*-
import os
import sys
import time
import signal
import pickle
import traceback
import comm_data
import comm_init
import comm_common
import comm_conn_manage
import comm_log_record
import file_up_down_tran
import comm_reg
#import comm_fusion_ansic 
from comm_data import thread_list
from comm_common import file_filter_bysuffix, create_thread, check_pidfile
from est.email_object import email_mtx_run, email_alarm_run
import comm_localdev

class File(object):
    def __init__(self):
        self.ISOTIMEFORMAT = "%Y-%m-%d %X"
        self.EXIST = "/gms/conf/reg.dat"
        self.ISFILE = "/gms/conf/reg.dat"
        self.ISFILELOG = "/gms/comm/isFileLog.log"
    def run(self):
        while True :
            if os.path.exists(self.EXIST) and os.path.isfile(self.ISFILE):
                timeZone = time.timezone/3600
                times = time.strftime(self.ISOTIMEFORMAT, time.localtime())
                fp = open(self.ISFILELOG, "a")
                fp.write("time zone [%s]   time [%s]" % (timeZone, times))
                fp.close()
                print "=========================="
                if int(os.path.getsize(self.ISFILELOG)/1024/1024) >= 1:
                    os.remove(self.ISFILELOG)
            else:
                pass
            time.sleep(0.1)
#------------------------------.
# @brief 主程序捕获函数        |
#------------------------------*
def sig_exit(sig, stack):
    #comm_log_record.logger.info('%s' % str(traceback.format_stack()))
    comm_log_record.logger.info("comm_main recv signal[%d]." % sig)
    comm_common.stop_process()

def check_auth():
    try:
        ret, stdout, stderr = comm_common.system(str(comm_data.auth_script))
    except Exception as ex:
        comm_log_record.logger.info('[%s].' % str(ex))
        return False
    else:
        if ret == 16:
            return True
        else:
            comm_log_record.logger.info("exec auth script[%s]: ret[%s] stdout[%s] stderr[%s]" % (comm_data.auth_script, ret, stdout, stderr))
            return False

#通讯主函数
def main():
    #配置初始化
    try:
        comm_init.init_conf()
        comm_localdev.read_postgres_devid()
    except Exception as ex:
        if comm_log_record.logger != None:
            comm_log_record.logger.warn("init conf fail. [%s]. exiting." % traceback.format_exc())
        else:
            print("init conf fail. [%s]. exiting." % traceback.format_exc())
        comm_common.stop_process()
        return

    #设备授权判断
    #if check_auth() == False:
    #    comm_log_record.logger.info("==== check auth fail. exiting. ====")
    #    comm_common.stop_process()
    #    return

    #通道建立
    try:
        comm_conn_manage.creat_comm_channel()
    except Exception, ex:
        comm_log_record.logger.info(ex)
        comm_common.stop_process()

    # 任何类型结点都要打开这个函数。
    # 它替代 mv_downfile_to_dbdir 线程的功能。
    try:
        #test = File()
        thread_list.append(create_thread(func=file_up_down_tran.send_down_file, name='SendDownFile'))
        
        if comm_data.if_is_xge_device == '1' :
            thread_list.append(create_thread(func=file_up_down_tran.reg_dat, name='RegDat'))
            thread_list.append(create_thread(func=file_up_down_tran.send_up_pcap, name='Pcap'))
            # isfile
            #thread_list.append(create_thread(func=test.run(), name='test'))
    except Exception,ex:
        comm_log_record.logger.info(ex)
        comm_common.stop_process()

    from transform import mtx, vds, apt
    #设备为监测节点所执行的操作
    if comm_data.ismonitor == '1':
        #启动格式转换线程
        try:
            thread_list.append(create_thread(func=mtx.mtx_move, name='MtxMove'))
            thread_list.append(create_thread(func=vds.vds_transform, name='VdsTransform'))
            thread_list.append(create_thread(func=apt.apt_transform, name='AptTransform'))
            # 看起来 send_down_file 可以替代这个线程。
            # 过去的实现似乎是有问题的，
            # 一个结点同时是检测点和管理点时，这两个线程会竞争。
            #thread_list.append(create_thread(func=file_up_down_tran.mv_downfile_to_dbdir, name='downfile-to-db-thread'))
        except Exception,ex:
            comm_log_record.logger.info(ex)
            comm_common.stop_process()

    #顶级设备, 设备接入云端执行的操作
    if comm_data.isroot == '1' or comm_data.link == '0' :
        try:
            thread_list.append(create_thread(func=file_up_down_tran.mv_upfile_to_dbdir, name='RootUpFileToDB'))
            thread_list.append(create_thread(func=mtx.mtx_transform, name='MtxTransform'))
        except Exception,ex:
            comm_log_record.logger.info(ex)
            comm_common.stop_process()
    else: #不接入云，则需要走通讯上传文件
        try:
            thread_list.append(create_thread(func=file_up_down_tran.send_up_file, name='SendUpFile'))
        except Exception,ex:
            comm_log_record.logger.info(ex)
            comm_common.stop_process()

    try:
        #fusion_engine = comm_fusion_ansic.EventManage()
        #thread_list.append(create_thread(func=fusion_engine.run(), name='CommFusionAnsic')) # ansice
        #thread_list.append(create_thread(func=mtx_scan_test.main_scan(), name='MtxScan')) # ansice
        thread_list.append(create_thread(func=email_mtx_run, name='emailMtx')) # 2.4gms-mtx-email
        #thread_list.append(create_thread(func=email_alarm_run, name='emailAlarm')) # 2.5gms-alarm-email
        pass 
    except Exception,ex:
        comm_log_record.logger.error(ex)
        comm_common.stop_process()

    # 现在注册统一走文件通道。任何一个结点如果没注册，都创建注册文件。
    if comm_data.dev_ip != "0.0.0.0" and comm_localdev.local_ip !=None : # 设备ip为0.0.0.0时不要注册
        comm_reg.create_local_reg_file()

def xml2dict(xmlfile):
    import xmltodict
    xmldict = None
    with file(xmlfile) as fp:
        xmldata = fp.read()
        xmldict = xmltodict.parse(xmldata)
    return xmldict


if __name__ == "__main__":
    check_pidfile()

    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)
    signal.signal(signal.SIGTERM, sig_exit)

    main()

    xmldict_org = xml2dict(comm_init.interface_conf_file)
    sec_cnt = 0
    while comm_common.is_continue():
        time.sleep(1)
        try:
            if xmldict_org != xml2dict(comm_init.interface_conf_file):
                comm_log_record.logger.info("==== config file was modified. restart. ====")
                comm_common.stop_process()
            #elif check_auth() == False:
            #    comm_log_record.logger.info("==== check auth fail. exiting. ====")
            #    comm_common.stop_process()
            else:
                sec_cnt = (sec_cnt + 1) % 60
                if sec_cnt == 0:
                    comm_data.trans_file_info()

                comm_init.read_ftp_conf()
        except:
            comm_log_record.logger.info(traceback.format_exc())

    for thread in thread_list:
        comm_log_record.logger.info('%s exiting.' % thread)
        thread.join()

    comm_data.trans_file_info()
    if os.path.exists(comm_data.comm_pid) and os.path.exists(comm_data.comm_pid):
        os.remove(comm_data.comm_pid)
        comm_log_record.logger.info("comm pid file exit.")
    comm_log_record.logger.info("comm_main exit.")

