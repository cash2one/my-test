# _*_ coding: utf8 _*_

import os
import sys
import time
import types
import socket
from transform.common import StmtParse, uuid
from vds_path_conf import output_dir
from vds_logger import logger

def convert_ip(IP):
    if IP in [None, '']:
        return '\N'
    else:
        return str(socket.ntohl(int(IP, 16)))

def convert_port(PORT):
    if PORT in [None, '']:
        return '\N'
    else:
        return str(socket.ntohs(int(PORT, 16)))

def convert_mac(MAC):
    if MAC in [None, '']:
        return '\N'
    else:
        return MAC

def get_port_from_url(URL):
    ''' 返回的端口列表元素是字符串，不是整形。'''
    HTTP_DEFAULT_PORT = ['80', '8080', '8081']
    import urllib, urlparse
    def add_http_header(url_str):
        for i in range(len(url_str)):
            if not url_str[i].isalpha():
                try:
                    if url_str[i] == ':' and url_str[i+1] == '/' and url_str[i+2] == '/':
                        return url_str
                except IndexError:
                    break
        return "http://" + url_str

    host = urlparse.urlparse(urllib.unquote(add_http_header(URL))).netloc
    port_index = host.find(':')
    if port_index == -1:
        return HTTP_DEFAULT_PORT
    else:
        port = ''
        for c in host[port_index+1:]:
            if c.isdigit():
                port += c
            else:
                break
        else:
            if len(port) > 0:
                return [port]
            else:
                return HTTP_DEFAULT_PORT

# 生成文件名的字段
db_file_name_desc = ['TS']

# 输出文件每行字段名。顺序和将来生成的行的顺序相关。
# 可以根据数据库字段名生成部分函数模板
# 对应的数据库表 t_event_av
db_field_name_desc = [
        # 数据库字段名  文件字段名
        ['devid',   []],
        ['ts',      ['TS']],
        ['proc',    ['PROC']],
        ['vxid',    ['VXID']],
        ['vxname',  ['VXName']],
        ['engine',  ['ENName']],
        ['sip',     ['SIP', 'DIP', 'SPORT', 'DPORT', 'TDIR', 'URL']],
        ['dip',     ['SIP', 'DIP', 'SPORT', 'DPORT', 'TDIR', 'URL']],
        ['sport',   ['SPORT', 'DPORT', 'TDIR', 'URL']],
        ['dport',   ['SPORT', 'DPORT', 'TDIR', 'URL']],
        ['pid',     ['PID']],
        ['tdir',    ['TDIR']],
        ['adir',    ['ADIR']],
        ['cid',     ['CID']],
        ['risk',    ['RISK']],
        ['idir',    ['IDIR']],
        ['fid',     ['FID']],
        ['etype',   ['EType']],
        ['btype',   ['BType']],
        ['stype',   ['SType']],
        ['smac',    ['SMAC', 'DMAC', 'SPORT', 'DPORT', 'TDIR', 'URL']],
        ['dmac',    ['SMAC', 'DMAC', 'SPORT', 'DPORT', 'TDIR', 'URL']],
        ['apn',     ['APN']],
        ['imsi',    ['IMSI']],
        ['msisdn',  ['MSISDN']],
        ['imei',    ['IME']],
        ['ext_ptr', ['URL']],
        ]

class VdsStmtParse(StmtParse):
    def __init__(self, fname):
        self.fname = fname
        self.db_file_name_desc = db_file_name_desc
        self.db_field_name_desc = db_field_name_desc
        StmtParse.__init__(self)

    def db_file_name(self, TS):
        #ts = time.localtime(int(TS, 16))
        ts = time.localtime(socket.ntohl(int(TS, 16)))
        timedata = time.strftime("%Y%m%d%H%M00", ts)
        fname = timedata + '_' + str(uuid) + '.ok'
        fname = os.path.join(output_dir, fname)
        #print("file_name: ", fname)
        return fname

    def db_field_devid(self):
        #print("devid: %d", uuid)
        return str(uuid)

    def db_field_ts(self, TS):
        #print("ts %s" % TS)
        #ts = time.localtime(int(TS, 16))
        ts = time.localtime(socket.ntohl(int(TS, 16)))
        return time.strftime("%Y-%m-%d %H:%M:%S", ts)

    # ? 这里需要转换成十进制整数么?
    def db_field_proc(self, PROC):
        #print("proc %s" % PROC)
        if PROC == '':
            return '\N'
        else:
            return str(int(PROC, 16))

    def db_field_vxid(self, VXID):
        #print("vxid %s" % VXID)
        if VXID == '':
            return '\N'
        else:
            return VXID

    def db_field_vxname(self, VXName):
        #print("vxname %s" % VXName)
        if VXName == '':
            return '\N'
        else:
            return VXName

    def db_field_engine(self, ENName):
        #print("engine %s" % ENName)
        if ENName == '':
            return '\N'
        else:
            return ENName

    # 原来的字段是十六进制字符串，需要转换成十进制字符串
    def db_field_sip(self, SIP, DIP, SPORT, DPORT, TDIR, URL):
        #print("sip %s" % SIP)
        if TDIR in [None, '']:
            return convert_ip(SIP)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                # 等于 0 ，调换源地址和目的地址
                return convert_ip(DIP)
            elif tdir == 1:
                # 等于 1 ，不调换源地址和目的地址
                return convert_ip(SIP)
            elif tdir == 2:
                logger.info('SIP[%s] DIP[%s] SPORT[%s] DPORT[%s] TDIR[%s] URL[%s]' % (SIP, DIP, SPORT, DPORT, TDIR, URL))
                # 等于 2 , 查找 url 中出现的端口为源端口
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)

                if sport in ports:
                    if dport in ports:
                        # 源端口和目标端口都在。
                        logger.warn('SPORT[%s] and DPORT[%s] in URL PORTS[%s]' % (SPORT, DPORT, ports))
                    return convert_ip(SIP)
                if dport in ports:
                    return convert_ip(DIP)

                # 源端口和目的端口都不在。都在，都不在，都返回源地址。
                logger.warn('SPORT[%s] and DPORT[%s] not in URL PORTS[%s]' % (SPORT, DPORT, ports))
                return convert_ip(SIP)
            else:
                return convert_ip(SIP)

    def db_field_dip(self, SIP, DIP, SPORT, DPORT, TDIR, URL):
        #print("dip %s" % DIP)
        if TDIR in [None, '']:
            return convert_ip(DIP)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                return convert_ip(SIP)
            elif tdir == 1:
                return convert_ip(DIP)
            elif tdir == 2:
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)

                if sport in ports:
                    if dport in ports: pass
                    return convert_ip(DIP)
                if dport in ports:
                    return convert_ip(SIP)

                return convert_ip(DIP)
            else:
                return convert_ip(DIP)

    def db_field_sport(self, SPORT, DPORT, TDIR, URL):
        #print("sport %s" % SPORT)
        if TDIR in [None, '']:
            return convert_port(SPORT)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                return convert_port(DPORT)
            elif tdir == 1:
                return convert_port(SPORT)
            elif tdir == 2:
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)
                if sport in ports:
                    if dport in ports: pass
                    return convert_port(SPORT)
                if dport in ports:
                    return convert_port(DPORT)
                return convert_port(SPORT)
            else:
                return convert_port(SPORT)

    def db_field_dport(self, SPORT, DPORT, TDIR, URL):
        #print("dport %s" % DPORT)
        if TDIR in [None, '']:
            return convert_port(DPORT)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                return convert_port(SPORT)
            elif tdir == 1:
                return convert_port(DPORT)
            elif tdir == 2:
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)
                if sport in ports:
                    if dport in ports: pass
                    return convert_port(DPORT)
                if dport in ports:
                    return convert_port(SPORT)
                return convert_port(DPORT)
            else:
                return convert_port(DPORT)

    def db_field_pid(self, PID):
        #print("pid %s" % PID)
        if PID == '':
            return '\N'
        else:
            return str(socket.ntohs(int(PID, 16)))

    def db_field_tdir(self, TDIR):
        #print("tdir %s" % TDIR)
        if TDIR == '':
            return '\N'
        else:
            return str(int(TDIR, 16))

    def db_field_adir(self, ADIR):
        #print("adir %s" % ADIR)
        if ADIR == '':
            return '\N'
        else:
            return str(int(ADIR, 16))

    def db_field_cid(self, CID):
        #print("cid %s" % CID)
        if CID == '':
            return '\N'
        else:
            return str(int(CID, 16))

    def db_field_risk(self, RISK):
        #print("risk %s" % RISK)
        if RISK == '':
            return '\N'
        else:
            return str(int(RISK, 16))

    def db_field_idir(self, IDIR):
        #print("idir %s" % IDIR)
        if IDIR == '':
            return '\N'
        else:
            return str(int(IDIR, 16))

    def db_field_fid(self, FID):
        #print("fid %s" % FID)
        if FID == '':
            return '\N'
        else:
            return str(int(FID, 16))

    def db_field_etype(self, EType):
        #print("etype %s" % EType)
        # select * from t_data_eventtype;
        # 看起来是其中的恶意代码传播事件。类型值为 6
        return '6'

    def db_field_btype(self, BType):
        #print("btype %s" % BType)
        if BType == '':
            return '\N'
        else:
            return str(int(BType, 16))

    def db_field_stype(self, SType):
        #print("stype %s" % SType)
        if SType == '':
            return '\N'
        else:
            return str(int(SType, 16))

    def db_field_smac(self, SMAC, DMAC, SPORT, DPORT, TDIR, URL):
        #print("smac %s" % SMAC)
        if TDIR in [None, '']:
            return convert_mac(SMAC)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                return convert_mac(DMAC)
            elif tdir == 1:
                return convert_mac(SMAC)
            elif tdir == 2:
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)

                if sport in ports:
                    if dport in ports: pass
                    return convert_mac(SMAC)
                if dport in ports:
                    return convert_mac(DMAC)

                return convert_mac(SMAC)
            else:
                return convert_mac(DMAC)

    def db_field_dmac(self, SMAC, DMAC, SPORT, DPORT, TDIR, URL):
        #print("dmac %s" % DMAC)
        if TDIR in [None, '']:
            return convert_mac(DMAC)
        else:
            tdir = int(TDIR, 16)
            if tdir == 0:
                return convert_mac(SMAC)
            elif tdir == 1:
                return convert_mac(DMAC)
            elif tdir == 2:
                sport = convert_port(SPORT)
                dport = convert_port(DPORT)
                ports = get_port_from_url(URL)

                if sport in ports:
                    if dport in ports: pass
                    return convert_mac(DMAC)
                if dport in ports:
                    return convert_mac(SMAC)

                return convert_mac(DMAC)
            else:
                return convert_mac(SMAC)

    def db_field_apn(self, APN):
        #print("apn %s" % APN)
        if APN == '':
            return '\N'
        else:
            return str(int(APN, 16))

    def db_field_imsi(self, IMSI):
        #print("imsi %s" % IMSI)
        if IMSI == '':
            return '\N'
        else:
            return IMSI

    def db_field_msisdn(self, MSISDN):
        #print("msisdn %s" % MSISDN)
        if MSISDN == '':
            return '\N'
        else:
            return MSISDN

    def db_field_imei(self, IME):
        #print("imei %s" % IME)
        if IME == '':
            return '\N'
        else:
            return IME

    def db_field_ext_ptr(self, URL):
        #print("ext_ptr %s" % URL)
        if URL == '':
            return '\N'
        else:
            return '"' + URL.replace('|', '\|') + '"'

