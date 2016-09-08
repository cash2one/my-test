# -*- coding:utf-8 -*-
# 连接管理模块，命令、文件通道建立
import os
import comm_data
import comm_reg
import comm_auth
import comm_cmd_proc
import device_manage
import comm_log_record
import traceback
import time
import file_up_down_tran
import comm_file_tran
from comm_data import thread_list
from comm_common import create_thread
import comm_common
import comm_socket

def srv_up_trans(fd):
    comm_auth.send_auth(fd)
    auth_uuid, auth_ip = comm_auth.recv_auth(fd)

    if comm_reg.check_parent_changed(auth_ip, auth_uuid) == 1:
        # 只有在有上级结点时才会被调用。也就是说不会在root中被调用
        # 如果父结点改变了.创建本机和下级的所有注册文件
        comm_log_record.logger.info('parent changed. create all reg files.')
        comm_reg.remove_reg_file()
        restart_py ='kill -9 0'
        try :
            os.popen(restart_py)
        except exception, e :
            comm_log_record.logger.error("kill comm_main failed because of '%s'" % e)
        comm_reg.create_topo_reg_file()

    comm_file_tran.resend_up_file(fd)
    comm_file_tran.send_up_files(fd)

def cli_up_trans(fd):
    auth_uuid, auth_ip = comm_auth.recv_auth(fd)
    comm_auth.send_auth(fd)
    #comm_data.parent_uuid, comm_data.parent_ip = comm_auth.recv_auth(fd)

    recv_cache_file = os.path.join(comm_data.recv_cache_path, '%s-%s.up' % (auth_uuid, auth_ip))
    comm_data.recv_cache_file_dict[fd] = recv_cache_file
    # 如果没有这个文件，创建
    if not os.path.exists(recv_cache_file):
        fp = open(recv_cache_file, 'w')
        fp.close()

    # 即使自己没注册成功,也会接收下级的文件
    # 假如收到一个下级的注册文件会缓存到本机,不上发
    comm_file_tran.recv_resend_request(fd)
    comm_file_tran.recv_up_files(fd)

def srv_down_trans(fd):
    auth_uuid, auth_ip = comm_auth.recv_auth(fd)
    comm_auth.send_auth(fd)

    recv_cache_file = os.path.join(comm_data.recv_cache_path, '%s-%s.down' % (auth_uuid, auth_ip))
    comm_data.recv_cache_file_dict[fd] = recv_cache_file
    if not os.path.exists(recv_cache_file):
        fp = open(recv_cache_file, 'w')
        fp.close()

    comm_file_tran.recv_resend_request(fd)
    comm_file_tran.recv_down_files(fd)

def cli_down_trans(fd):
    comm_auth.send_auth(fd)
    comm_auth.recv_auth(fd)

    # 如果没注册成功，压根不会连接下级。
    comm_file_tran.resend_down_file(fd)
    comm_file_tran.send_down_files(fd)

def srv_up_loop(host, port):
    #comm_log_record.logger.info('create srv up comm thread.')
    comm_socket.listen(host, port, srv_up_trans)

def cli_up_loop(host, port):
    comm_log_record.logger.info('create cli up comm thread. [%s:%s].' % (host, port))
    while comm_reg.is_reg_done() == 0:
        # 注册成功后才连接下级节点。
        if comm_common.is_continue():
            time.sleep(1)
        else:
            return
    comm_socket.connect(host, port, cli_up_trans)

def srv_down_loop(host, port):
    #comm_log_record.logger.info('create srv down comm thread.')
    comm_socket.listen(host, port, srv_down_trans)

def cli_down_loop(host, port):
    comm_log_record.logger.info('create cli down comm thread. [%s:%s].' % (host, port))
    while comm_reg.is_reg_done() == 0:
        # 注册成功后才连接下级节点。
        if comm_common.is_continue():
            time.sleep(1)
        else:
            return
    comm_socket.connect(host, port, cli_down_trans)
#socket
def srv_str_trans(fd):
    comm_file_tran.recv_up_str(fd)


def srv_str_loop(host, port):
    comm_socket.listen(host, port, srv_str_trans)

#连接维护线程
def creat_comm_channel():
    if comm_data.isroot == '0':
        # 如果不是 root 都需要连接上级
        # 原来的命令通道上传文件
        # server 线程名都是本地 IP，因为不知道谁会连接。client 线程名是对端 IP。
        thread_list.append(create_thread(func=srv_up_loop,
                        args=(comm_data.dev_ip, comm_data.comm_cmd_port),
                        name='SUThread[%s:%s]' % (comm_data.dev_ip, comm_data.comm_cmd_port)))
        # 原来的文件通道下传文件
        thread_list.append(create_thread(func=srv_down_loop,
                        args=(comm_data.dev_ip, comm_data.comm_file_port),
                        name='SDThread[%s:%s]' % (comm_data.dev_ip, comm_data.comm_file_port)))
        if comm_data.if_is_xge_device == '1' :
            thread_list.append(create_thread(func=srv_str_loop,
                            args=(comm_data.dev_ip, int(comm_data.comm_sh_port)),
                            name='SDThread[%s:%s]' % (comm_data.dev_ip, comm_data.comm_sh_port)))

    if comm_data.ismanager == '1' or comm_data.isroot == '1':
        # 通常 manager root 都是同时配置的。但也发现某些文件只配置了 root。
        for sub_dev in comm_data.sub_dev_list:
            thread_list.append(create_thread(func=cli_up_loop,
                            args=(sub_dev.ip, sub_dev.comm_cmd_port),
                            name='CUThread[%s:%s]' % (sub_dev.ip, sub_dev.comm_cmd_port)))
            thread_list.append(create_thread(func=cli_down_loop,
                            args=(sub_dev.ip, sub_dev.comm_file_port),
                            name='CDThread[%s:%s]' % (sub_dev.ip, sub_dev.comm_file_port)))

