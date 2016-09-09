# -*- coding:utf-8 -*-
import comm_data
import comm_cmd_proc
import comm_common
import time
import os
import struct
import shutil
import traceback
import comm_reg
import comm_socket
import device_manage
import comm_log_record
import file_up_down_tran
from comm_common import CommException

##########################SOCKET##################################
def get_send_up_str():
    if comm_data.up_shell_info == "":
        return ""
    else:
        return comm_data.up_shell_info

def recv_up_str(fd):
	comm_socket.recv_size_commcand(fd, 4096) 
        
##########################传送文件内容接口######################################
def send_heartbeat(fd):
    sid = comm_common.get_serialno()
    comm_cmd_proc.send_cmd_size(fd, comm_cmd_proc.CMD_HEADER_SIZE)
    comm_cmd_proc.send_cmd_header(fd, sid, comm_cmd_proc.CMD_HEARTBEAT, comm_cmd_proc.CMD_REQUEST)

def send_up_files(fd):
    last_sec = time.time()
    while comm_common.is_continue():
        ftype, fname = get_send_up_file()
        if ftype == None:
            if time.time() - last_sec > 30:
                # 如果三十秒没发包了。发送一个心跳包。
                send_heartbeat(fd)
            else:
                time.sleep(0.1)
        else:
            if os.path.exists(fname):
                # 可能这个文件在重传的时候已经被发送走了
                last_sec = time.time()
                send_up_file(fd, ftype, fname)
            else:
                comm_log_record.logger.warn("file[%s] already send up." % fname)
            comm_data.up_cmd_info = None

def recv_up_files(fd):
    child_ip, _ = fd.getpeername()
    while comm_common.is_continue():
        cmd_size = comm_cmd_proc.recv_cmd_size(fd)
        sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
        if type1 == comm_cmd_proc.CMD_HEARTBEAT:
            continue
        elif type1 == comm_cmd_proc.CMD_FILE and type2 == comm_cmd_proc.CMD_REQUEST:
            ftype, fname, fsize, foffset, md5 = recv_file_header(fd)

            # 如果是 topo 文件要存到另个目录.源目录是self，目标目录是other
            if ftype == comm_cmd_proc.topo_evt_up:
                tailname = os.path.split(fname)[-1]
                fname = os.path.join(comm_data.comm_topo_up_dst_path, tailname)

            tmpname = fname + '.dat'
            recv_file(fd, ftype, tmpname, fsize, foffset, md5)
            if comm_common.md5cmp(md5, tmpname):
                if foffset > 0:
                    comm_log_record.logger.info('recv up file file[%s] success. size[%s] offset[%s].' % (fname, fsize, foffset))
                comm_cmd_proc.send_cmd_size(fd, comm_cmd_proc.CMD_HEADER_SIZE)
                comm_cmd_proc.send_cmd_header(fd, sid, comm_cmd_proc.CMD_FILE, comm_cmd_proc.CMD_RESPONSE_SUCCESS)
                if ftype == comm_cmd_proc.device_reg_up:
                    # 如果收到注册文件。要根据情况修改注册文件的父UUID，并更新本地拓扑
                    reg_cmd = comm_reg.get_regbuf_from_file(tmpname)
                    comm_log_record.logger.info("==reg==> rcv dev reg file. cmd [%s]" % reg_cmd)

                    reg_fields = reg_cmd.split('|')
                    pid = reg_fields[1]
                    uuid = reg_fields[2]
                    if pid == '\N':
                        reg_fields[1] = comm_data.uuid
                        reg_cmd = '|'.join(reg_fields)
                        comm_log_record.logger.info("==reg==> replace reg cmd pid. new cmd [%s]" % reg_cmd)

                        # 写新的 reg_cmd 到文件
                        with open(tmpname, 'w') as fp:
                            fp.write(reg_cmd)

                    # 建立拓扑
                    device_manage.creat_comm_topo(uuid, child_ip, reg_cmd)
                    os.rename(tmpname, fname)
                else:
                    os.rename(tmpname, fname)
                comm_data.inc_recv_up_succ_cnt(ftype)
            else:
                # 如果 md5 错误，大概是接收哪里出错了。
                comm_cmd_proc.send_cmd_size(fd , comm_cmd_proc.CMD_HEADER_SIZE)
                comm_cmd_proc.send_cmd_header(fd, sid, comm_cmd_proc.CMD_FILE, comm_cmd_proc.CMD_RESPONSE_FAIL)
                comm_log_record.logger.error('recv up file[%s] md5[%s] error.' % (fname, md5))
                os.rename(tmpname, tmpname + '.md5.error')
                comm_data.inc_recv_up_fail_cnt(ftype)
        else:
            raise CommException('recv unknown file request [%s:%s:%s]' % (sid, type1, type2))

def send_down_files(fd):
    last_sec = time.time()
    child_ip, child_port = fd.getpeername()
    while comm_common.is_continue():
        ftype, fname = get_send_down_file(child_ip)
        if ftype == None:
            if time.time() - last_sec > 30:
               # 如果三十秒没发包了。发送一个心跳包。
                send_heartbeat(fd)
            else:
                time.sleep(1)
        else:
            # 这里有这样一种情况。
            # 假如只有一个文件，send_down_file 的 while 里找到这个文件，下发。
            # 但 while 继续循环，反复得到这个文件名，
            # 直到某次，取到文件名后，这里清 down_cmd_info 标识位，那边构造命令。
            # 但这时，文件已经被删除。所以要在这里判断文件是否存在。
            # 另一种情况是这个文件在重传时已经发送走了
            if os.path.exists(fname):
                last_sec = time.time()
                send_down_file(fd, ftype, fname)
            else:
                comm_log_record.logger.info("file[%s] already send down." % fname)
            for i in comm_data.sub_dev_list:
                if i.ip == child_ip:
                    i.down_cmd_info = None
                    break

def recv_down_files(fd):
    while comm_common.is_continue():
        cmd_size = comm_cmd_proc.recv_cmd_size(fd)
        sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
        if type1 == comm_cmd_proc.CMD_HEARTBEAT:
            continue
        elif type1 == comm_cmd_proc.CMD_FILE and type2 == comm_cmd_proc.CMD_REQUEST:
            ftype, fname, fsize, foffset, md5 = recv_file_header(fd)

            # 上面的 cmd_size 没用上，是依赖 fsize 和 foffset 来判断内容长度的
            tmpname = fname + '.dat'
            recv_file(fd, ftype, tmpname, fsize, foffset, md5)
            if comm_common.md5cmp(md5, tmpname):
                if foffset > 0:
                    comm_log_record.logger.info('recv down file[%s] success. size[%s] offset[%s].' % (fname, fsize, foffset))
                comm_cmd_proc.send_cmd_size(fd , comm_cmd_proc.CMD_HEADER_SIZE)
                comm_cmd_proc.send_cmd_header(fd, sid, comm_cmd_proc.CMD_FILE, comm_cmd_proc.CMD_RESPONSE_SUCCESS)
                os.rename(tmpname, fname)
                comm_data.inc_recv_down_succ_cnt(ftype)
            else:
                comm_cmd_proc.send_cmd_size(fd , comm_cmd_proc.CMD_HEADER_SIZE)
                comm_cmd_proc.send_cmd_header(fd, sid, comm_cmd_proc.CMD_FILE, comm_cmd_proc.CMD_RESPONSE_FAIL)
                comm_log_record.logger.error('recv down file[%s] md5[%s] error.' % (fname, md5))
                os.rename(tmpname, tmpname + '.md5.error')
                comm_data.inc_recv_down_fail_cnt(ftype)
        else:
            raise CommException('recv unknown file requese [%s:%s:%s]' % (sid, type1, type2))

def send_up_file(fd, ftype, fname, foffset=0):
    # 大概只有在 md5 校验出错时会返回 False
    # 如果返回 False，这里没有清 up_cmd_info，
    # 然后 get_send_up_file 会取到同一个文件重传
    if send_file(fd, ftype, fname, foffset) == True:
        file_up_down_tran.send_up_file_seccuss_action(fname)
        comm_data.inc_send_up_succ_cnt(ftype)
    else:
        comm_data.inc_send_up_fail_cnt(ftype)

def send_down_file(fd, ftype, fname, foffset=0):
    if send_file(fd, ftype, fname, foffset) == True:
        file_up_down_tran.send_down_file_success_action(fname)
        comm_data.inc_send_down_succ_cnt(ftype)
    else:
        comm_data.inc_send_down_fail_cnt(ftype)

def resend_up_file(fd):
    send_sid = comm_common.get_serialno()
    comm_cmd_proc.send_cmd_size(fd, comm_cmd_proc.CMD_HEADER_SIZE)
    comm_cmd_proc.send_cmd_header(fd, send_sid, comm_cmd_proc.CMD_RESEND, comm_cmd_proc.CMD_REQUEST)

    cmd_size = comm_cmd_proc.recv_cmd_size(fd)
    recv_sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
    if send_sid == recv_sid and type1 == comm_cmd_proc.CMD_RESEND and type2 == comm_cmd_proc.CMD_RESPONSE_SUCCESS:
        ftype, fname, fsize, foffset, md5 = recv_file_header(fd)
        if fname == '':
            # 即没有需要重转的文件
            return

        # 检查文件是否存在，md5，大小是否正确
        if os.path.exists(fname) and os.path.getsize(fname) == fsize and comm_common.md5cmp(md5, fname):
            send_up_file(fd, ftype, fname, foffset)
        else:
            if not os.path.exists(fname):
                comm_log_record.logger.warn('resend up file[%s] not found.' % fname)
            elif not (os.path.getsize(fname) == fsize):
                comm_log_record.logger.warn('resend up file[%s] size[%s] != [%s].' % (fname, os.path.getsize(fname), fsize))
            else:
                comm_log_record.logger.warn('resend up file[%s] md5[%s] != [%s].' % (fname, comm_common.md5sum(fname), md5))
    else:
        raise CommException('recv unknown resend response [%s:%s:%s]' % (recv_sid, type1, type2))

def resend_down_file(fd):
    send_sid = comm_common.get_serialno()
    comm_cmd_proc.send_cmd_size(fd, comm_cmd_proc.CMD_HEADER_SIZE)
    comm_cmd_proc.send_cmd_header(fd, send_sid, comm_cmd_proc.CMD_RESEND, comm_cmd_proc.CMD_REQUEST)

    cmd_size = comm_cmd_proc.recv_cmd_size(fd)
    recv_sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
    if send_sid == recv_sid and type1 == comm_cmd_proc.CMD_RESEND and type2 == comm_cmd_proc.CMD_RESPONSE_SUCCESS:
        ftype, fname, fsize, foffset, md5sum = recv_file_header(fd)
        if fname == '':
            return

        if os.path.exists(fname) and os.path.getsize(fname) == fsize and comm_common.md5cmp(md5sum, fname):
            send_down_file(fd, ftype, fname, foffset)
        else:
            if not os.path.exists(fname):
                comm_log_record.logger.warn('resend up file[%s] not found.' % fname)
            elif not (os.path.getsize(fname) == fsize):
                comm_log_record.logger.warn('resend up file[%s] size[%s] != [%s].' % (fname, os.path.getsize(fname), fsize))
            else:
                comm_log_record.logger.warn('resend up file[%s] md5[%s] != [%s].' % (fname, comm_common.md5sum(fname), md5))
    else:
        raise CommException('recv unknown resend response [%s:%s:%s]' % (recv_sid, type1, type2))

def recv_resend_request(fd):
    # 这里只对重传请求进行应答
    cmd_size = comm_cmd_proc.recv_cmd_size(fd)
    recv_sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
    if type1 == comm_cmd_proc.CMD_RESEND and type2 == comm_cmd_proc.CMD_REQUEST:
        # 找到接收进度文件，
        recv_cache_file = comm_data.recv_cache_file_dict[fd]
        with open(recv_cache_file, 'r') as fp:
            cache_info = fp.read()

        if cache_info == '':
            fname = ''
            ftype = fsize = foffset = 0
            md5 = 32 * '0'
        else:
            cache_fields = cache_info.split('|')
            ftype = int(cache_fields[0])
            fname = cache_fields[1]
            fsize = int(cache_fields[2])
            foffset = int(cache_fields[3])
            md5 = cache_fields[4]
            if fsize == foffset:
                # 可能有上一次的进度文件没有被清除
                fname = ''
                ftype = fsize = foffset = 0
                md5 = 32 * '0'

        cmd_header = comm_cmd_proc.make_cmd_header(recv_sid, comm_cmd_proc.CMD_RESEND, comm_cmd_proc.CMD_RESPONSE_SUCCESS)
        file_header = make_file_header2(ftype, fname, fsize, foffset, md5)
        comm_cmd_proc.send_cmd_size(fd, len(cmd_header) + len(file_header))
        comm_socket.send_size(fd, cmd_header)
        comm_socket.send_size(fd, file_header)
    else:
        raise CommException('recv unknown resend request [%s:%s:%s]' % (recv_sid, type1, type2))

def make_file_header(ftype, fname, foffset=0):
    fname_size = len(fname)
    fsize = os.path.getsize(fname)
    md5 = comm_common.md5sum(fname)
    file_header_fmt = '!BI%ssII32s' % fname_size
    file_header = struct.pack(file_header_fmt, int(ftype), fname_size, str(fname), fsize, foffset, md5)
    return file_header

def make_file_header2(ftype, fname, fsize, foffset, md5):
    fname_size = len(fname)
    if fname_size == 0:
        file_header_fmt = '!BIII32s'
        file_header = struct.pack(file_header_fmt, int(ftype), fname_size, fsize, foffset, md5)
    else:
        file_header_fmt = '!BI%ssII32s' % fname_size
        # 这里 fname 可能是 <type 'unicode'> 类型，所以需要转一下
        file_header = struct.pack(file_header_fmt, int(ftype), fname_size, str(fname), fsize, foffset, md5)
    return file_header

def recv_file_header(fd):
    ftype = struct.unpack('B', comm_socket.recv_size(fd, 1))[0]
    fname_size = struct.unpack('!I', comm_socket.recv_size(fd, 4))[0]
    if fname_size == 0:
        fname = ''
    else:
        fname = struct.unpack('%ss' % fname_size, comm_socket.recv_size(fd, fname_size))[0]
    fsize = struct.unpack('!I', comm_socket.recv_size(fd, 4))[0]
    foffset = struct.unpack('!I', comm_socket.recv_size(fd, 4))[0]
    md5 = struct.unpack('32s', comm_socket.recv_size(fd, 32))[0]
    return (ftype, fname, fsize, foffset, md5)

def send_file(fd, ftype, fname, foffset):
    send_sid = comm_common.get_serialno()

    cmd_header = comm_cmd_proc.make_cmd_header(send_sid, comm_cmd_proc.CMD_FILE, comm_cmd_proc.CMD_REQUEST)
    file_header = make_file_header(ftype, fname, foffset)

    file_size = os.path.getsize(fname)

    # 长度需要减去已经发送的 offset 的长度
    comm_cmd_proc.send_cmd_size(fd, len(cmd_header) + len(file_header) + (file_size - foffset))
    comm_socket.send_size(fd, cmd_header)
    comm_socket.send_size(fd, file_header)

    send_size = foffset
    with open(fname, 'r') as fp:
        fp.seek(foffset)
        data = fp.read(4096)
        while data:
            comm_socket.send_size(fd, data)
            send_size += len(data)
            data = fp.read(4096)

    if send_size != file_size:
        raise CommException('file[%s] send size[%s] != file size[%s]' % (fname, send_size, file_size))

    cmd_size = comm_cmd_proc.recv_cmd_size(fd)
    if cmd_size != comm_cmd_proc.CMD_HEADER_SIZE:
        raise CommException('file recv response size[%s] != CMD_HEADER_SIZE[%s]' % (cmd_size, comm_cmd_proc.CMD_HEADER_SIZE))

    recv_sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
    if send_sid != recv_sid:
        raise CommException('file send sid[%s] != recv sid[%s].' % (send_sid, recv_sid))

    if type1 != comm_cmd_proc.CMD_FILE:
        raise CommException('file recv response[%s] != comm_cmd_proc.CMD_FILE[%s]' % (type1, comm_cmd_proc.CMD_FILE))

    if type2 == comm_cmd_proc.CMD_RESPONSE_SUCCESS:
        if foffset > 0:
            comm_log_record.logger.info('send file[%s] success. size[%s] offset[%s]' % (fname, file_size, foffset))
        return True
    else:
        comm_log_record.logger.info('send file[%s] fail.' % (fname))
        return False

def recv_file(fd, ftype, fname, fsize, foffset, md5):
    if os.path.exists(fname):
        tmpfile_size = os.path.getsize(fname)
        if tmpfile_size < foffset:
            raise CommException('recv resend request. but tmp file[%s] size[%s] < foffset[%s]' % (fname, tmpfile_size, foffset))
    else:
        if foffset > 0:
            with open(recv_cache_file, 'w') as cache_fp:
            # 如果没有找到临时文件，说明哪里出了问题，清掉进度文件
                cache_fp.truncate(0)
            raise CommException('recv resend request. but not found tmp file[%s].' % (fname))

    recv_cache_file = comm_data.recv_cache_file_dict[fd]
    with open(recv_cache_file, 'w') as cache_fp:
        # 如果文件不存在，先创建，这里应该还有其它方法吧。
        # w 会覆盖掉缓冲文件。rw 和 rw+ 在文件不存在会报错，不会创建文件。
        # wa 会创建文件，但网络上的解释，会定位到文件尾，使用 seek 也回不来。
        if not os.path.exists(fname):
            fp = open(fname, 'w')
            fp.close()

        with open(fname, 'rw+') as recv_fp:
            recv_fp.seek(foffset)
            recv_size = foffset

            while recv_size < fsize:
                if fsize - recv_size < 4096:
                    data = comm_socket.recv_size(fd, fsize - recv_size)
                else:
                    data = comm_socket.recv_size(fd, 4096)
                recv_size += len(data)
                recv_fp.write(data)

                if recv_size > 1024 * 1024:
                    recv_fp.flush()
                    # 把文件名后面的.dat切下来作为保存的文件名
                    cache_info = '%s|%s|%s|%s|%s' % (ftype, os.path.splitext(fname)[0], fsize, recv_size, md5)
                    # 看起来如果这里不seek 0，即使truncate也会一直往后写
                    cache_fp.seek(0)
                    cache_fp.truncate(0)
                    cache_fp.write(cache_info)
                    cache_fp.flush()

def get_send_up_file():
    ''' 返回值是 (ftype, fname) 的形式. 如果没有值就返回 ()'''
    if comm_data.up_cmd_info == None:
        return (None, None)
    else:
        cmd_c = comm_data.up_cmd_info.split(comm_cmd_proc.SPLIT)
        ftype = cmd_c[5]
        fname = cmd_c[7]
        return (ftype, fname)

def get_send_down_file(child_ip):
    for i in comm_data.sub_dev_list:
        if i.ip == child_ip and i.down_cmd_info != None:
            cmd_c = i.down_cmd_info.split(comm_cmd_proc.SPLIT)
            ftype = cmd_c[5]
            fname = cmd_c[7]
            return (ftype, fname)
    return (None, None)

################################################################################
