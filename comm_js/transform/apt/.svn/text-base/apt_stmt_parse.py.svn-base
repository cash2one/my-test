# _*_ coding: utf8 _*_

import os
import sys
import time
import types
import socket
from transform.common import StmtParse, uuid, make_unique_file_name
from apt_path_conf import output_dir

# 生成文件名的字段
db_file_name_desc = ['timedata']

# 输出文件每行字段名。顺序和将来生成的行的顺序相关。
# 可以根据数据库字段名生成部分函数模板
db_field_name_desc = [
        # 数据库字段名  文件字段名
        ['devid',   []],
        ['etype',   []],
        ['sip',     ['sip']],
        ['dip',     ['dip']],
        ['iplen',   ['iplen']],
        ['sport',   ['sport']],
        ['dport',   ['dport']],
        ['proto',   ['proto']],
        ['domain',  ['domain']],
        ['dnsip',   ['dnsip']],
        ['timenow', ['timenow']],
        ['year',    ['year']],
        ['month',   ['month']],
        ['day',     ['day']],
        ['hour',    ['hour']],
        ['min',     ['min']],
        ['sec',     ['sec']],
        ['timedata',['timedata']],
        ['ruleid',  ['ruleid']],
        ['rulename',['rulename']],
        ['sip_str', ['sip_str']],
        ['dip_str', ['dip_str']],
        ['payload', ['payload_str']],
        ['reserved1', ['reserved1']],
        ['reserved2', ['reserved2']],
        ['reserved3', ['reserved3']],
        ['reserved4', ['reserved4']],
        ['reserved5', ['reserved5']],
        ['reserved6', ['reserved6']],
        ]

# 在最早的版本中，空字符串 或者 null 表示字段为空。
# 在某次更新后，发现开始使用 None 表示字段为空。
def is_nil_field(field):
    if field == None:
        return True
    else:
        field = field.upper()
        if field in ['', 'NULL', 'NONE']:
            return True
        else:
            return False

class AptStmtParse(StmtParse):
    def __init__(self, fname):
        self.fname = fname
        self.db_file_name_desc = db_file_name_desc
        self.db_field_name_desc = db_field_name_desc
        StmtParse.__init__(self)

    def db_file_name(self, timedata):
        # 过去是精确到秒产生文件名。现在改为精确到分钟。
        # 配置文件中现在是 120 秒，所以可能存在同名文件。
        timedata = time.strftime("%Y%m%d%H%M00", time.localtime(float(timedata)))
        fname = timedata + '_' + str(uuid) + '.ok'
        fname = make_unique_file_name(output_dir, fname)
        #fname = os.path.join(output_dir, fname)
        #print("file_name: ", fname)
        return fname

    def db_field_devid(self):
        #print("devid: %d", uuid)
        return str(uuid)

    def db_field_etype(self):
        # select * from t_data_eventtype;
        # 看起来是其中的通信行为异常事件。类型值为 5
        #print("etype: 5")
        return '5'

    def db_field_timenow(self, timenow):
        #timenow = time.strftime("%Y-%m-%d %H:%M:%S", timedata)
        timenow = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(float(timenow)))
        #print("timenow: %s" % timenow)
        return timenow

    def db_field_year(self, year):
        #print("year: %s" % year)
        return str(year)

    def db_field_month(self, month):
        #print("month: %s" % month)
        return str(month)

    def db_field_day(self, day):
        #print("day: %s" % day)
        return str(day)

    def db_field_hour(self, hour):
        #print("hour: %s" % hour)
        return str(hour)

    def db_field_min(self, min):
        #print("min: %s" % min)
        return str(min)

    def db_field_sec(self, sec):
        #print("sec: %s" % sec)
        return str(sec)

    def db_field_sip(self, sip):
        #print("sip %s" % sip)
        return str(socket.ntohl(int(sip, 10)))

    def db_field_dip(self, dip):
        #print("dip %s" % dip)
        return str(socket.ntohl(int(dip, 10)))

    def db_field_iplen(self, iplen):
        #print("iplen %s" % iplen)
        return str(iplen)

    def db_field_sport(self, sport):
        #print("sport: %s" % sport)
        return str(sport)

    def db_field_dport(self, dport):
        #print("dport: %s" % dport)
        return str(dport)

    def db_field_proto(self, proto):
        #print("proto: %s" % proto)
        return str(proto)

    def db_field_domain(self, domain):
        #print("domain: %s" % domain)
        if is_nil_field(domain):
            return '\N'
        else:
            return str(domain)

    def db_field_dnsip(self, dnsip):
        #print("dnsip %s" % dnsip)
        return str(dnsip)

    def db_field_timedata(self, timedata):
        #timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(timedata))
        timedata = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(float(timedata)))
        #print("timedata: %s" % timedata)
        return timedata

    def db_field_ruleid(self, ruleid):
        #print("ruleid %s" % ruleid)
        return str(ruleid)

    def db_field_rulename(self, rulename):
        #print("rulename %s" % rulename)
        # rulename 字段可能是中文
        rulename = rulename.decode('utf-8')
        return rulename

    def db_field_sip_str(self, sip_str):
        if is_nil_field(sip_str):
            return '\N'
        else:
            return str(sip_str)

    def db_field_dip_str(self, dip_str):
        if is_nil_field(dip_str):
            return '\N'
        else:
            return str(dip_str)

    def db_field_payload(self, payload_str):
        def normalize(payload):
            tmplist = ['"']

            if payload[0] == '"' and payload[-1] == '"':
                # 如果开头结尾都是双引号，就认为这个负载已经自带双引号。
                payload = payload[1:-1]

            i = 0
            length = len(payload)
            escape = False
            while i < length:
                c = payload[i]
                if escape == True:
                    # 如果发现反斜线则无条件把反斜线和后面紧跟的字符拷贝过去
                    # 如果发现 | " ' 则在前面添加反斜线
                    escape = False
                else:
                    if c == '\\':
                        escape = True
                    else:
                        if c in ['|', '"', "'"]:
                            tmplist.append('\\')
                tmplist.append(c)
                i = i + 1

            # 可能有一个反斜杠出现在末尾。再追加一个。
            if escape == True:
                tmplist.append('\\')

            tmplist.append('"')
            return ''.join(tmplist)

        if is_nil_field(payload_str):
            return '\N'
        else:
            return normalize(payload_str)

    def db_field_reserved1(self, reserved1):
        if is_nil_field(reserved1):
            return '\N'
        else:
            return str(reserved1)

    def db_field_reserved2(self, reserved2):
        if is_nil_field(reserved2):
            return '\N'
        else:
            return str(reserved2)

    def db_field_reserved3(self, reserved3):
        if is_nil_field(reserved3):
            return '\N'
        else:
            return str(reserved3)

    def db_field_reserved4(self, reserved4):
        if is_nil_field(reserved4):
            return '\N'
        else:
            return str(reserved4)

    def db_field_reserved5(self, reserved5):
        if is_nil_field(reserved5):
            return '\N'
        else:
            return str(reserved5)

    def db_field_reserved6(self, reserved6):
        if is_nil_field(reserved6):
            return '\N'
        else:
            return str(reserved6)

