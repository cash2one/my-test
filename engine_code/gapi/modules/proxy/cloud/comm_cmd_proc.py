# -*- coding:utf-8 -*-
import os
import struct
import comm_data
import comm_socket
import comm_common
import comm_log_record

CMD_AUTH = 1
CMD_FILE = 2
CMD_RESEND = 3
CMD_HEARTBEAT = 4
CMD_REQUEST = 1
CMD_RESPONSE_SUCCESS = 2
CMD_RESPONSE_FAIL = 3

CMD_HEADER_SIZE = 6

#信息类型取值 com_type
cmd_type = 1
file_type = 2
#mod_type 内容类型
config = 1
event = 2
update = 3
#子类型 sub_type1的取值
mtx_evt_up = 6
mtx_evt_down = 7
device_reg_up = 11
device_reg_down = 12
ve_evt_up = 13
ae_evt_up = 15
topo_evt_up = 16
flow_evt_up = 17
devstat_evt_up = 18
surl_evt_up = 22
ue_file_down = 19
re_file_down = 20
conf_file_down = 21
query_flow_up_evt = 28
query_dns_up_evt = 23
query_url_up_evt = 24
query_flow_down_evt = 25
query_dns_down_evt = 26
query_url_down_evt = 27

#子类型 sub_type2的取值
notify = '0'
confirm = '1'
success = '0'
fail = '1'

SPLIT = "|**|"

def event_type_name(event_type):
    event_type_dict = {mtx_evt_up : 'mtx',
                        ve_evt_up : 'vds',
                        ae_evt_up : 'abb',
                        devstat_evt_up : 'dev-stat',
                        topo_evt_up : 'topo',
                        flow_evt_up : 'flow',
                        surl_evt_up : 'surl',
                        mtx_evt_down : 'mtx',
                        ue_file_down : 'update',
                        re_file_down : 'rule',
                        device_reg_up : 'reg',
                        device_reg_down : 'reg',
                        conf_file_down : 'conf',
                        query_flow_up_evt: 'query_flow_up',
                        query_dns_up_evt: 'query_dns_up',
                        query_url_up_evt: 'query_url_up',
                        query_flow_down_evt: 'query_flow_down',
                        query_dns_down_evt: 'query_dns_down',
                        query_url_down_evt: 'query_url_down',}
    if event_type_dict.has_key(event_type):
        return event_type_dict[event_type]
    else:
        return 'unknown'

#构造发送命令的信息, 构造命令各子信息以'|'分割，在对端接收时根据'|'来读取各部分信息
def comm_make_cmdinfo(sub_type1, sub_type2, para):
    if sub_type1 in [device_reg_up, device_reg_down]:
        cmd_info = "sdev11" + SPLIT + "ddev22" + SPLIT + "time" + SPLIT + str(cmd_type) + SPLIT \
            + str(config) + SPLIT + str(sub_type1) + SPLIT + str(sub_type2) + SPLIT + para
    else:
        cmd_info = "sdev11" + SPLIT + "ddev22" + SPLIT + "time" + SPLIT + str(cmd_type) + SPLIT \
            + str(event) + SPLIT + str(sub_type1) + SPLIT + str(sub_type2) + SPLIT + para

    return cmd_info

def send_cmd_size(fd, cmd_size):
    comm_socket.send_size(fd, 'cnct' + struct.pack('!I', cmd_size))

def recv_cmd_size(fd):
    ''' 返回命令的长度 '''
    magic = comm_socket.recv_size(fd, 4)
    if magic == 'cnct':
        data = comm_socket.recv_size(fd, 4)
        size = struct.unpack('!I', data)[0]
        return size
    else:
        comm_common.CommException('cmd magic [%s] != cnct' % magic)

def make_cmd_header(sid, type1, type2):
    return struct.pack('!IBB', sid, type1, type2)

def send_cmd_header(fd, sid, type1, type2):
    comm_socket.send_size(fd, struct.pack('!IBB', sid, type1, type2))

def recv_cmd_header(fd):
    #return (sid, type1, type2)
    return struct.unpack('!IBB', comm_socket.recv_size(fd, CMD_HEADER_SIZE))

