#!/usr/bin/env python
# _*_ coding: utf8 _*_

import sys
import ConfigParser
import socket
import struct

local_ip = None
db_name = None
user = None
pwd = None
host = None
port = None

db_conf_file = "/gms/conf/database.conf"
# 这个配置文件名也可以写到 comm_pre_conf.json 中，然后通过 comm_data.db_conf_file 获得。
# 但在 comm_data 中的值只有在 comm_init 后才能使用。但在 url_detect 中不方便调用这个函数。

def get_db_conf():
    global db_name, user, pwd, host, port
    with open(db_conf_file) as fp:
        cfg = ConfigParser.ConfigParser()
        cfg.readfp(fp)
        db_name = cfg.get('POSTGRESQL', 'db_name')
        user = cfg.get('POSTGRESQL', 'user')
        pwd = cfg.get('POSTGRESQL', 'pwd')
        unix = cfg.get('POSTGRESQL', 'unix')
        if unix.upper() == 'YES':
            host = cfg.get('POSTGRESQL', 'domain_sock')
        else:
            host = cfg.get('POSTGRESQL', 'ip')
        port = cfg.get('POSTGRESQL', 'port')

def read_postgres_devid():
        get_db_conf()
        update_postgres_devid()

def update_postgres_devid():
    global local_ip
    import psycopg2
    conn = psycopg2.connect(database = db_name,
                            user = user,
                            password = pwd,
                            host = host,
                            port = port)

    cur = conn.cursor()
    cur.execute("select ip from t_conf_localdev")
    rows = cur.fetchall()
    cur.close()
    conn.close()
    for row in rows:
        local_ip = socket.inet_ntoa(struct.pack('I',socket.htonl(int(row[0]))))
    return local_ip


if __name__ == '__main__':
    print read_postgres_devid()
    #tmp=update_white_list()
    

