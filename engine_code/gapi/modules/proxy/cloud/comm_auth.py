# -*- coding:utf-8 -*-

import os
import traceback
import comm_data
import comm_socket
import comm_log_record
import comm_cmd_proc
import comm_common
import Crypto
from Crypto.Cipher import AES
from binascii import b2a_hex, a2b_hex

def des_encrypt_auth_body(data):
    import pyDes
    key = pyDes.des("chGMS123", pyDes.CBC, "\0\0\0\0\0\0\0\0")
    return key.encrypt(data, "*")

def des_decrypt_auth_body(data):
    import pyDes
    key = pyDes.des("chGMS123", pyDes.CBC, "\0\0\0\0\0\0\0\0")
    return key.decrypt(data, "*")

# a class encrypt and decrypt containing encrypt_handle(),decrypt_handle()
class prpcrypt():
    def __init__(self,key):    #__init__: key and mode
        self.key = key
        self.mode = AES.MODE_CBC

#encrypt_handle() AES-256,
#if len(data) < 32,polish  remainder
#else remove excess part
    def encrypt(self,text):
        cryptor = AES.new(self.key,self.mode)
        length = 32
        count = len(text)
        if count < length:
             add = (length-count)
             text = text + (' ' * add)
        elif count > length:
            add = (length-(count % length))
            text = text + ('\0' * add)
        self.ciphertext = cryptor.encrypt(text)
        return b2a_hex(self.ciphertext)

#decrypt_handle()
    def decrypt(self,text):
        cryptor = AES.new(self.key,self.mode)
        plain_text  = cryptor.decrypt(a2b_hex(text))
        return plain_text.rstrip('\0')

def encrypt_auth_body(data):
    # AES key must be either 16, 24, or 32 bytes long
    pc = prpcrypt('chGMS123chGMS123')
    return pc.encrypt(data)

def decrypt_auth_body(body):
    # AES key must be either 16, 24, or 32 bytes long
    pc = prpcrypt('chGMS123chGMS123')
    return pc.decrypt(body)

def send_auth(fd):
    sid = comm_common.get_serialno()
    cmd_header = comm_cmd_proc.make_cmd_header(sid, comm_cmd_proc.CMD_AUTH, comm_cmd_proc.CMD_REQUEST)
    auth_body = encrypt_auth_body('%s|%s' % (comm_data.uuid, comm_data.dev_ip))
    cmd = cmd_header + auth_body
    comm_cmd_proc.send_cmd_size(fd, len(cmd))
    comm_socket.send_size(fd, cmd)
    return sid

def recv_auth(fd):
    # 暂时忽略 SID 的作用.
    # 因为只要收到的UUID和IP正确,就可以认为对方正确.
    cmd_size = comm_cmd_proc.recv_cmd_size(fd)
    sid, type1, type2 = comm_cmd_proc.recv_cmd_header(fd)
    if type1 == comm_cmd_proc.CMD_AUTH and type2 == comm_cmd_proc.CMD_REQUEST:
        auth_body = comm_socket.recv_size(fd, cmd_size - comm_cmd_proc.CMD_HEADER_SIZE)
        auth_body = decrypt_auth_body(auth_body)
        auth_uuid, auth_ip = auth_body.split('|')
        peerip, peerport = fd.getpeername()
        if auth_ip == peerip:
            comm_log_record.logger.info('remote host[%s:%s:%s] auth success.' % (peerip, peerport, auth_uuid))
            return auth_uuid, auth_ip
        else:
            raise comm_common.CommException('recv error auth request[%s:%s:%s]. peer ip[%s] !=  auth ip[%s].' % (sid, type1, type2, peerip, auth_ip))
    else:
        raise comm_common.CommException('recv unkown auth request[%s:%s:%s]' % (sid, type1, type2))

if __name__ == '__main__':
        pc = prpcrypt('1234567890123456')
        import sys
        e = pc.encrypt("186")
        d = pc.decrypt(e)
        print "encrypt:",e
        print "decrypt:",d

