#!/usr/bin/env python
#-*- coding:utf-8 -*-

import psycopg2
# 数据库连接参数
class PgApi(object):
    def __init__(self, *args, **kargs):
        self.conn = psycopg2.connect(database="gms_db", user="gms_user", password="Rzxlszy-cGMS@456", host="127.0.0.1", port="5432")
        self.cur = self.conn.cursor()
    def insert(self):
        #self.cur.execute("INSERT INTO t_net_id(user_id, net_time, user_name, ip, email, accout) \
        #                    VALUES(%s, %s, %s, %s, %s, %s)", \
        #                              (1, '2015-1-1', 'aaa', '1.1.1.1', '1@1.com', '1'))
        #self.cur.execute("INSERT INTO t_net_id(user_id, net_time, user_name, ip, email, accout) \
        #                    VALUES(%s, %s, %s, %s, %s, %s)", \
        #                              (3, '2015-3-3', 'ccc', '3.3.3.3', '3@3.com', '3'))

        self.cur.execute("INSERT INTO t_net_id(user_id, net_time, user_name, user_ip, user_email, \
                     datacreatetype, createuser, updateuser, status) \
                            VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s)", \
                                  ("3", '2015-3-5', 'ccc', '1019207441', 'wangkun1@chanct.com', '3', "12345", "321", "1"))
        self.conn.commit()                                                                             
        return True
    def select(self):
        self.cur.execute("SELECT user_ip, user_email FROM t_net_id where status=1 ORDER BY net_time ASC;")
        rows = self.cur.fetchall()        
        #print "test..............",rows
        return dict(rows)
    def status(self):
        try:
            self.cur.execute("SELECT ovalue FROM tb_secparam where oid='sendPolicy';") 
            rows = self.cur.fetchall()
        except Exception, ex:
            logger.error(ex)
            return ""      
        if rows == []:
            return ""
        return rows[0][0]
    def close(self):
        self.cur.close()                                                                               
        self.conn.close()
        return True
if __name__ == "__main__":
    test = PgApi()
    #print test.insert()
    print test.select()
    #print test.status()
    test.close()
