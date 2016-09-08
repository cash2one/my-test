#!/usr/bin/evn python
#coding:utf-8

import ConfigParser

db_conf_file='/gms/conf/database.conf'
postgres_user='postgres'
db_name = None
user = None
pwd = None
host = None
port = None
table = 't_log_update'

def init_db_conf():
    global db_name, user, pwd, host, port

    with open(db_conf_file) as fp:
        cfg = ConfigParser.ConfigParser()
        cfg.readfp(fp)
        db_name = cfg.get('POSTGRESQL', 'db_name')
        user = cfg.get('POSTGRESQL', 'user')
        user = postgres_user
        pwd = cfg.get('POSTGRESQL', 'pwd')
        unix = cfg.get('POSTGRESQL', 'unix')
        if unix.upper() == 'YES':
            host = cfg.get('POSTGRESQL', 'domain_sock')
        else:
            host = cfg.get('POSTGRESQL', 'ip')
        port = cfg.get('POSTGRESQL', 'port')

    

def update_rule_table(info_dict):
    import psycopg2
    
    if not info_dict:
        print '/gms/mtx/conf/rule.conf is not modified .'
        return
    init_db_conf()
    try:
        conn = psycopg2.connect(database = db_name,
                                user = user,
                                password = pwd,
                                host = host,
                                port = port)

        cur = conn.cursor()
        cur.execute("insert into %s(utime,utype,\
                    ucontent_type,old_version,\
                    new_version,new_verinfo,\
                    ustatus,opuser,oper_ip)\
                    values(%s,%s,%s,%s,%s,'%s',%s,%s,%s)" %(table,info_dict['utime'],info_dict['utype'],\
                                                         info_dict['ucontent_type'],info_dict['old_version'],\
                                                         info_dict['new_version'],info_dict['new_verinfo'],\
                                                         info_dict['ustatus'],info_dict['opuser'],info_dict['oper_ip']))
 
        conn.commit()
    except Exceptiom ,ex:
        print ex 
        cur.close()
        conn.close()

    cur.close()
    conn.close()

if __name__ == '__main__':
    #get_db_conf()
    rule_dict = {}
    rule_dict['table'] = table
    rule_dict['utime'] = '\'2010-10-10 11:11:11\''
    rule_dict['utype'] = 2
    rule_dict['ucontent_type'] = 1
    rule_dict['old_version'] = 20000
    rule_dict['new_version'] = 10000
    rule_dict['new_verinfo'] = '\'定时升级成功\''
    rule_dict['ustatus'] = 3
    rule_dict['opuser'] = 9
    rule_dict['oper_ip'] = 2130706433

    #2,1,20000,10000,'定时升级成功',3,9,2130706433

    print rule_dict
    update_rule_table(rule_dict)

