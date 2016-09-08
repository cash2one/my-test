#!/usr/bin/env python
# _*_ coding: utf8 _*_

import sys
import traceback
import ConfigParser

sys.path.append('/gms/comm/')
import comm_data
import comm_log_record

db_name = None
user = None
pwd = None
host = None
port = None
table = 't_conf_whitelist'

white_list = []
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

def create_white_list():
    if enable_white_list():
        get_db_conf()
        update_white_list()

def update_white_list():
    if enable_white_list():
        global white_list
        import psycopg2
        conn = psycopg2.connect(database = db_name,
                                user = user,
                                password = pwd,
                                host = host,
                                port = port)

        cur = conn.cursor()
        cur.execute("select start_ip, end_ip from %s;" % table)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        tmplist = []
        for row in rows:
            tmplist.append((int(row[0]), int(row[1])))
        white_list = tmplist
        #print white_list
        return white_list
    else:
        return {}

# 如果在白名单列表的则不上报
def lookup_white_list(ip):
    for sip, eip in white_list:
        if ip >= sip and ip <= eip:
           return True
    return False

def enable_white_list():
    try:
        eflag = comm_data.enable_white_list
    except AttributeError:
        import comm_init
        comm_init.read_pre_json()
        eflag = comm_data.enable_white_list
    except:
        eflag = 'NO'
    finally:
        if eflag == '0' or eflag == 0 or eflag.upper() == 'NO':
            return False
        else:
            return True

if __name__ == '__main__':
    create_white_list()
    update_white_list()
    print lookup_white_list(123)
    print lookup_white_list(3232235521)
    print lookup_white_list(3232235522)
    print lookup_white_list(3232235720)
    print lookup_white_list(3232235721)

    import psycopg2
    conn = psycopg2.connect(database=db_name,
                            user=user,
                            password=pwd,
                            host=host,
                            port=port)
    cur = conn.cursor()
    cur.execute("INSERT INTO %s(start_ip, end_ip) VALUES(%d, %d)" % (table, 100, 200))
    cur.execute("INSERT INTO %s(start_ip, end_ip) VALUES(%d, %d)" % (table, 300, 400))
    cur.execute("select start_ip, end_ip from %s;" % table)
    rows = cur.fetchall()
    tmplist = []
    for row in rows:
        tmplist.append((int(row[0]), int(row[1])))
    conn.commit()
    cur.close()
    conn.close()

    update_white_list()
    print lookup_white_list(123)
    print lookup_white_list(3232235521)
    print lookup_white_list(3232235522)
    print lookup_white_list(3232235720)
    print lookup_white_list(3232235721)

