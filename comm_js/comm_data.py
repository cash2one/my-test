# -*- coding:utf-8 -*-

import time
import comm_log_record

ismanager = 0         #设备类型是管理节点
ismonitor = 0         #设备是监测节点
isroot = 0         #设备是顶级设备
iscloudcon = 0    #设备是否是在线模式
dev_ip = 0
uuid = 0
dev_name = 0    #设备名称
if_is_xge_device = 0    # 1:10G; 2: 1G
comm_cmd_port = '10108'
comm_file_port = '10107'
comm_sh_port = '10106'

#文件是否拷贝到入库目录
iscopytodb = True

#是否拷贝到ftp目录
iscopytoftp = False

# 具体路径(授权文件，中间转换层，传输目录，入库目录，阿里代理目录)参考 conf/comm_pre_conf.json 文件
# 所有变量名从该 json 文件读取，这里不再保存默认值

up_cmd_info = None

#测试用
class sub_dev_info:
    def __init__(self):
        self.ip = ''
        self.dev_id = ''
        self.down_cmd_info = None
        self.uuid = ''
        self.comm_cmd_port = '' #通讯的命令端口
        self.comm_file_port = '' #通讯的文件端口

sub_dev_list = []
topo = []

#------------------------------------.
#        设备注册使用                |
#------------------------------------*
#本设备父设备ID
local_pid = ""
#客户名称
custom_name = "CustomName"

# 保存接收端的 fd 和接收进度文件的映射
recv_cache_file_dict = {}

# 保存程序执行过程中创建的所有线程
thread_list = []

# 发送文件的计数器
send_up_succ_cnt = 0
send_up_fail_cnt = 0
recv_up_succ_cnt = 0
recv_up_fail_cnt = 0

send_down_succ_cnt = 0
send_down_fail_cnt = 0
recv_down_succ_cnt = 0
recv_down_fail_cnt = 0

import comm_cmd_proc

send_up_cnt = {
        comm_cmd_proc.mtx_evt_up: ['mtx', 0, 0],
        comm_cmd_proc.device_reg_up: ['reg', 0, 0],
        comm_cmd_proc.ve_evt_up: ['vds', 0, 0],
        comm_cmd_proc.ae_evt_up: ['apt', 0, 0],
        comm_cmd_proc.topo_evt_up: ['topo', 0, 0],
        comm_cmd_proc.flow_evt_up: ['flow', 0, 0],
        comm_cmd_proc.devstat_evt_up: ['stat', 0, 0],
        comm_cmd_proc.surl_evt_up: ['surl', 0, 0],
        comm_cmd_proc.query_flow_up_evt: ['query_flow', 0, 0],
        comm_cmd_proc.query_dns_up_evt: ['query_dns', 0, 0],
        comm_cmd_proc.query_url_up_evt: ['query_url', 0, 0],
        comm_cmd_proc.pcap_evt_up: ['pcap', 0, 0],
        comm_cmd_proc.sqlchop_evt_up: ['sqlchop', 0, 0],
    }

recv_up_cnt = {
        comm_cmd_proc.mtx_evt_up: ['mtx', 0, 0],
        comm_cmd_proc.device_reg_up: ['reg', 0, 0],
        comm_cmd_proc.ve_evt_up: ['vds', 0, 0],
        comm_cmd_proc.ae_evt_up: ['apt', 0, 0],
        comm_cmd_proc.topo_evt_up: ['topo', 0, 0],
        comm_cmd_proc.flow_evt_up: ['flow', 0, 0],
        comm_cmd_proc.devstat_evt_up: ['stat', 0, 0],
        comm_cmd_proc.surl_evt_up: ['surl', 0, 0],
        comm_cmd_proc.query_flow_up_evt: ['query_flow', 0, 0],
        comm_cmd_proc.query_dns_up_evt: ['query_dns', 0, 0],
        comm_cmd_proc.query_url_up_evt: ['query_url', 0, 0],
        comm_cmd_proc.pcap_evt_up: ['pcap', 0, 0],
        comm_cmd_proc.sqlchop_evt_up: ['sqlchop', 0, 0],
    }

send_down_cnt = {
        comm_cmd_proc.mtx_evt_down: ['mtx', 0, 0],
        comm_cmd_proc.device_reg_down: ['reg', 0, 0],
        comm_cmd_proc.ue_file_down: ['vds', 0, 0],
        comm_cmd_proc.re_file_down: ['rule', 0, 0],
        comm_cmd_proc.conf_file_down: ['conf', 0, 0],
        comm_cmd_proc.query_flow_down_evt: ['query_flow', 0, 0],
        comm_cmd_proc.query_dns_down_evt: ['query_dns', 0, 0],
        comm_cmd_proc.query_url_down_evt: ['query_url', 0, 0],
    }

recv_down_cnt = {
        comm_cmd_proc.mtx_evt_down: ['mtx', 0, 0],
        comm_cmd_proc.device_reg_down: ['reg', 0, 0],
        comm_cmd_proc.ue_file_down: ['vds', 0, 0],
        comm_cmd_proc.re_file_down: ['rule', 0, 0],
        comm_cmd_proc.conf_file_down: ['conf', 0, 0],
        comm_cmd_proc.query_flow_down_evt: ['query_flow', 0, 0],
        comm_cmd_proc.query_dns_down_evt: ['query_dns', 0, 0],
        comm_cmd_proc.query_url_down_evt: ['query_url', 0, 0],
    }

recv_local_cnt = {
        comm_cmd_proc.mtx_evt_down: ['mtx', 0],
        comm_cmd_proc.device_reg_down: ['reg', 0],
        comm_cmd_proc.ue_file_down: ['vds', 0],
        comm_cmd_proc.re_file_down: ['rule', 0],
        comm_cmd_proc.conf_file_down: ['conf', 0],
        comm_cmd_proc.query_flow_down_evt: ['query_flow', 0],
        comm_cmd_proc.query_dns_down_evt: ['query_dns', 0],
        comm_cmd_proc.query_url_down_evt: ['query_url', 0],
    }

begin_time = time.time()
def trans_file_info():
    comm_log_record.logger.info("dev type: root[%s] manager[%s] monitor[%s]" % (isroot, ismanager, ismonitor))
    end_time = time.time()
    comm_log_record.logger.info('[%s] -> [%s] [%ss]:' % (
        time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(begin_time)),
        time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(end_time)),
        int(end_time - begin_time)))

    comm_log_record.logger.info('send up cnt:')
    comm_log_record.logger.info('  total:	succ[%s] fail[%s]' % (send_up_succ_cnt, send_up_fail_cnt))
    for item in send_up_cnt.values():
        comm_log_record.logger.info('  %s: 	succ[%s] fail[%s]' % (item[0], item[1], item[2]))

    comm_log_record.logger.info('recv up cnt:')
    comm_log_record.logger.info('  total: 	succ[%s] fail[%s]' % (recv_up_succ_cnt, recv_up_fail_cnt))
    for item in recv_up_cnt.values():
        comm_log_record.logger.info('  %s: 	succ[%s] fail[%s]' % (item[0], item[1], item[2]))

    comm_log_record.logger.info('send down cnt:')
    comm_log_record.logger.info('  total: 	succ[%s] fail[%s]' % (send_down_succ_cnt, send_down_fail_cnt))
    for item in send_down_cnt.values():
        comm_log_record.logger.info('  %s: 	succ[%s] fail[%s]' % (item[0], item[1], item[2]))

    comm_log_record.logger.info('recv down cnt:')
    comm_log_record.logger.info('  total: 	succ[%s] fail[%s]' % (recv_down_succ_cnt, recv_down_fail_cnt))
    for item in recv_down_cnt.values():
        comm_log_record.logger.info('  %s: 	succ[%s] fail[%s]' % (item[0], item[1], item[2]))

    comm_log_record.logger.info('recv local cnt:')
    for item in recv_local_cnt.values():
        comm_log_record.logger.info('  %s: 	[%s]' % (item[0], item[1]))

def inc_send_up_succ_cnt(ftype):
    global send_up_succ_cnt
    try:
        send_up_cnt[int(ftype)][1] += 1
        send_up_succ_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown send up ftype [%s]' % ftype)

def inc_send_up_fail_cnt(ftype):
    global send_up_fail_cnt
    try:
        send_up_cnt[int(ftype)][2] += 1
        send_up_fail_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown send up ftype [%s]' % ftype)

def inc_recv_up_succ_cnt(ftype):
    global recv_up_succ_cnt
    try:
        recv_up_cnt[int(ftype)][1] += 1
        recv_up_succ_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown recv up ftype [%s]' % ftype)

def inc_recv_up_fail_cnt(ftype):
    global recv_up_fail_cnt
    try:
        recv_up_cnt[int(ftype)][2] += 1
        recv_up_fail_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown recv up ftype [%s]' % ftype)

def inc_send_down_succ_cnt(ftype):
    global send_down_succ_cnt
    try:
        send_down_cnt[int(ftype)][1] += 1
        send_down_succ_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown send down ftype [%s]' % ftype)

def inc_send_down_fail_cnt(ftype):
    global send_down_fail_cnt
    try:
        send_down_cnt[int(ftype)][2] += 1
        send_down_fail_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown send down ftype [%s]' % ftype)

def inc_recv_down_succ_cnt(ftype):
    global recv_down_succ_cnt
    try:
        recv_down_cnt[int(ftype)][1] += 1
        recv_down_succ_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown recv down ftype [%s]' % ftype)

def inc_recv_down_fail_cnt(ftype):
    global recv_down_fail_cnt
    try:
        recv_down_cnt[int(ftype)][2] += 1
        recv_down_fail_cnt += 1
    except KeyError:
        comm_log_record.logger.info('unkown recv down ftype [%s]' % ftype)

def inc_recv_local_cnt(ftype):
    try:
        recv_local_cnt[int(ftype)][1] += 1
    except KeyError:
        comm_log_record.logger.info('unkown recv local ftype [%s]' % ftype)
