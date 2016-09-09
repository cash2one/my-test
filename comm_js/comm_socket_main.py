# -*- coding:utf-8 -*-
import time
import comm_init
import comm_log_record
import comm_common
import comm_data
import comm_conn_manage
import comm_common
import comm_socket
import comm_file_tran
import traceback
from comm_common import  create_thread
DATA = None
thread_total=[]

def send_up_str(fd):
    global DATA
    if DATA :
        comm_socket.send_size_commcand(fd, DATA)
        comm_log_record.logger.info('create cli str comm thread. [%s:%s:%s].' % (host, port, DATA))
        #DATA = None
    else:
        pass

def cli_str_trans(fd):
    send_up_str(fd)
    #fd.close()
def cli_str_loop(host, port):
    #comm_log_record.logger.info('create cli str comm thread. [%s:%s].' % (host, port))
    # 注册成功后才连接下级节点。
    if comm_common.is_continue():
        time.sleep(1)
    else:
        return
    comm_socket._connect(host, port, cli_str_trans) 

def main(args):
        #配置初始化
        global DATA
        DATA = args
        try:
            comm_init.init_conf()
        except Exception as ex:
            if comm_log_record.logger != None:
                comm_log_record.logger.warn("init conf fail. [%s]. exiting." % traceback.format_exc())
            else:
                print("init conf fail. [%s]. exiting." % traceback.format_exc())
            comm_common.stop_process()
            return
        thread_total=[]
        for sub_dev in comm_data.sub_dev_list:
            #cli_str_loop(sub_dev.ip, int(comm_data.comm_sh_port))
            thread_total.append(create_thread(func=cli_str_loop,
                            args=(sub_dev.ip, int(comm_data.comm_sh_port)),
                            name='CDThread[%s:%s]' % (sub_dev.ip, comm_data.comm_sh_port)))
        for thread in thread_total:
            thread.join()

if __name__ == '__main__':
    main("mkdir -p /root/131")
