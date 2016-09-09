# -*- coding:utf-8 -*-

import os
import sys
import time
import errno
import socket
import struct
import traceback
import comm_log_record
import comm_common
import commands
import comm_data
import comm_file_flock
import threading
mutex = threading.Lock()
def listen(host, port, srvfunc):
    while comm_common.is_continue():
        listenfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        listenfd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            listenfd.bind((host, port))
            listenfd.listen(5)
            listenfd.settimeout(10)
            comm_log_record.logger.info('bind [%s:%s] success.' % (host, port))
        except:
            comm_log_record.logger.info('bind [%s:%s] fail. %s.' % (host, port, sys.exc_info()[1]))
            time.sleep(10)
        else:
            while comm_common.is_continue():
                try:
                    srvfd, addr = listenfd.accept()
                    srvfd.settimeout(60)
                    if comm_data.link == "0" :
                        from xml.etree.ElementTree import ElementTree                               
                        xml_path = "/gms/conf/gmsconfig.xml"                                        
                        _file = comm_file_flock.File(xml_path)
                        while True :
                            print _file.status()
                            # if _file.status()["is_opened"] == False:
                            mutex.acquire()
                            comm_log_record.logger.debug('gms config xml open number:%s' % _file.status())
                            tree = ElementTree()                                                        
                            tree.parse(xml_path)                                                        
                            p = tree.find("comm_stat")                                                  
                            p.set("link", "1")                                                          
                            tree.write(xml_path)                                                        
                            if os.path.os.path.exists("/gms/conf/reg.dat"):                             
                                os.system("mv /gms/conf/reg.dat /gms/conf/reg.dat.bak")                 
                            os.system("kill -9 0") 
                            mutex.release()
                            break
                            #else:
                            #    time.sleep(0.1) 
                except socket.timeout:
                    #comm_log_record.logger.debug('waiting for connection timeout.')
                    if comm_data.link == "1" :
                        xml_path = "/gms/conf/gmsconfig.xml"
                        _file = comm_file_flock.File(xml_path)
                        while True :
                            print _file.status()
                            comm_log_record.logger.debug('gms config xml open number:%s' % _file.status())
                            # if _file.status()["is_opened"] == False:
                            mutex.acquire()
                            from xml.etree.ElementTree import ElementTree
                            tree = ElementTree()
                            tree.parse(xml_path)
                            p = tree.find("comm_stat")
                            p.set("link", "0")
                            tree.write(xml_path)
                            if os.path.os.path.exists("/gms/conf/reg.dat"):
                                os.system("mv /gms/conf/reg.dat /gms/conf/reg.dat.bak")
                            os.system("kill -9 0")
                            mutex.release()
                            break
                            # else:
                            #    time.sleep(0.1) 
                    pass
                except socket.error as ex:
                    comm_log_record.logger.info('%s' % str(ex))
                    #if ex.errno == errno.EINTR:
                    #    # 被信号中断。可以继续 accept，不用关闭 socket。
                    #    continue
                    #else:
                    #    comm_log_record.logger.info('%s' % traceback.format_exc())
                except:
                    comm_log_record.logger.info('%s' % traceback.format_exc())
                else:
                    try:
                        comm_log_record.logger.info('connection from [%s:%s].' % (addr[0], addr[1]))
                        srvfunc(srvfd)
                    except comm_common.CommException as ex:
                        #comm_log_record.logger.info('%s' % traceback.format_exc())
                        comm_log_record.logger.info('%s. close srv fd.' % str(ex))
                    except socket.error as ex:
                        if ex.errno == errno.ECONNRESET or ex.errno == errno.ENOTCONN:
                            #error: [Errno 104] Connection reset by peer
                            #error: [Errno 107] Transport endpoint is not connected
                            comm_log_record.logger.info('%s. close srv fd.' % str(ex))
                        else:
                            comm_log_record.logger.info('%s' % traceback.format_exc())
                    except:
                        comm_log_record.logger.info('%s' % traceback.format_exc())
                        comm_log_record.logger.info('close srv fd.')
                    finally:
                        srvfd.close()
            else:
                listenfd.close()
def _connect(host, port, clifunc):
        try:
            clifd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            clifd.connect((host, port))
            clifd.settimeout(60)
        except socket.error as ex:
            if ex.errno == errno.ECONNREFUSED:
                # [Errno 111] Connection refused.
                pass
            else:
                comm_log_record.logger.info('%s.' % sys.exc_info()[1])
            time.sleep(10)
        except:
            comm_log_record.logger.info('connect [%s:%s] %s' % (host, port, traceback.format_exc()))
            time.sleep(10)
        else:
            try:
                comm_log_record.logger.info('connection [%s:%s] success.' % (host, port))
                clifunc(clifd)
            except comm_common.CommException as ex:
                #comm_log_record.logger.info('%s' % traceback.format_exc())
                comm_log_record.logger.info('%s. close cli fd.' % str(ex))
            except socket.error as ex:
                if ex.errno == errno.ECONNRESET or ex.errno == errno.ENOTCONN:
                    #error: [Errno 104] Connection reset by peer
                    #error: [Errno 107] Transport endpoint is not connected
                    comm_log_record.logger.info('%s. close cli fd.' % str(ex))
                else:
                    comm_log_record.logger.info('%s' % traceback.format_exc())
            except Exception as ex:
                comm_log_record.logger.info('%s' % traceback.format_exc())
                comm_log_record.logger.info('%s. close cli fd.' % str(ex))
            finally:
                clifd.close()

def connect(host, port, clifunc):
    while comm_common.is_continue():
        # 以下程序和_connect()函数一样，因为会有堵塞的情况
        try:
            clifd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            clifd.connect((host, port))
            clifd.settimeout(60)
        except socket.error as ex:
            if ex.errno == errno.ECONNREFUSED:
                # [Errno 111] Connection refused.
                pass
            else:
                comm_log_record.logger.info('%s.' % sys.exc_info()[1])
            time.sleep(10)
        except:
            comm_log_record.logger.info('connect [%s:%s] %s' % (host, port, traceback.format_exc()))
            time.sleep(10)
        else:
            try:
                comm_log_record.logger.info('connection [%s:%s] success.' % (host, port))
                clifunc(clifd)
            except comm_common.CommException as ex:
                #comm_log_record.logger.info('%s' % traceback.format_exc())
                comm_log_record.logger.info('%s. close cli fd.' % str(ex))
            except socket.error as ex:
                if ex.errno == errno.ECONNRESET or ex.errno == errno.ENOTCONN:
                    #error: [Errno 104] Connection reset by peer
                    #error: [Errno 107] Transport endpoint is not connected
                    comm_log_record.logger.info('%s. close cli fd.' % str(ex))
                else:
                    comm_log_record.logger.info('%s' % traceback.format_exc())
            except Exception as ex:
                comm_log_record.logger.info('%s' % traceback.format_exc())
                comm_log_record.logger.info('%s. close cli fd.' % str(ex))
            finally:
                clifd.close()

def send_size(fd, data):
    host, port = fd.getpeername()
    total_size = len(data)
    send_size = send_offset = 0
    while send_size < total_size:
        try:
            send_size += fd.send(data[send_offset:])
            send_offset = send_size
            #comm_log_record.logger.info('send to[%s:%s] %d bytes' % (host, port, send_size))
        except socket.timeout:
            # 假如对方的缓存满了(连续发对方不调用接收就会出现这种情况)。
            # 假如等到超时，写日志，抛出错误。
            comm_log_record.logger.info('send to[%s:%s] %s' % (host, port, sys.exc_info()[1]))
            raise
            #time.sleep(1)
        except socket.error as ex:
            # 对方断开链接 # error: [Errno 32] Broken pipe
            # 对方程序结束 # error: [Errno 104] Connection reset by peer
            # 信号中断 # error: [Errno 4] Interrupted system call
            comm_log_record.logger.info('send to[%s:%s] %s' % (host, port, str(ex)))
            if ex.errno == errno.EINTR:
                continue
            else:
                raise

def recv_size(fd, size):
    host, port = fd.getpeername()
    recv_data = ''
    recv_size = 0
    while recv_size < size:
        try:
            tmpdata = fd.recv(size - recv_size)
            if len(tmpdata) == 0:
                raise comm_common.CommException('[%s:%s] disconnect.' % (host, port))
            else:
                #comm_log_record.logger.info('recv from[%s:%s]: %s' % (host, port, tmpdata))
                recv_data = recv_data + tmpdata
                recv_size = len(recv_data)
        except socket.timeout:
            # 假如等到超时，写日志，抛出错误。
            comm_log_record.logger.info('recv from[%s:%s] %s' % (host, port, sys.exc_info()[1]))
            raise
        except socket.error as ex:
            if ex.errno == errno.EINTR:
                comm_log_record.logger.info('recv from[%s:%s] %s' % (host, port, str(ex)))
            else:
                comm_log_record.logger.info('%s' % traceback.format_exc())
                raise
    return recv_data

def send_size_commcand(fd, data):
    host, port = fd.getpeername()
    total_size = len(data)
    send_size = send_offset = 0
    while send_size < total_size:
        try:
            send_size += fd.send(data[send_offset:])
            send_offset = send_size
            #comm_log_record.logger.info('send to[%s:%s] %d bytes' % (host, port, send_size))
        except socket.timeout:
            # 假如对方的缓存满了(连续发对方不调用接收就会出现这种情况)。
            # 假如等到超时，写日志，抛出错误。
            comm_log_record.logger.info('send to[%s:%s] %s' % (host, port, sys.exc_info()[1]))
            raise
            #time.sleep(1)
        except socket.error as ex:
            # 对方断开链接 # error: [Errno 32] Broken pipe
            # 对方程序结束 # error: [Errno 104] Connection reset by peer
            # 信号中断 # error: [Errno 4] Interrupted system call
            comm_log_record.logger.info('send to[%s:%s] %s' % (host, port, str(ex)))
            if ex.errno == errno.EINTR:
                continue
            else:
                raise
    fd.close()

def recv_size_commcand(fd, size):
    host, port = fd.getpeername()
    recv_data = ''
    recv_size = 0
    try:
        tmpdata = fd.recv(size - recv_size)
        if len(tmpdata) == 0:
            raise comm_common.CommException('[%s:%s] disconnect.' % (host, port))
        else:
            #comm_log_record.logger.info('recv from[%s:%s]: %s' % (host, port, tmpdata))
            recv_data = recv_data + tmpdata
            recv_data = commands.getstatusoutput(recv_data) 
            if recv_data[0] :
                comm_log_record.logger.error('recv from[%s:%s]: %s' % (host, port, recv_data[1]))
            else:
                fd.send(recv_data[0])
                fd.close()
                comm_log_record.logger.info('recv from[%s:%s]: %s' % (host, port, recv_data[1]))
            recv_size = len(recv_data)
    except socket.timeout:
        # 假如等到超时，写日志，抛出错误。
        comm_log_record.logger.info('recv from[%s:%s] %s' % (host, port, sys.exc_info()[1]))
        raise
    except socket.error as ex:
        if ex.errno == errno.EINTR:
            comm_log_record.logger.info('recv from[%s:%s] %s' % (host, port, str(ex)))
        else:
            comm_log_record.logger.info('%s' % traceback.format_exc())
            raise
    return recv_data

