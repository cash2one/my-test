# -*- coding: utf-8 -*- 
import os
import  xdrlib ,sys
import xlrd
import ConfigParser
out_file='/tmp/file.txt'
db_conf_file='/gms/conf/database.conf'
postgres_user='postgres'
db_name = None
user = None
pwd = None
host = None
port = None
table = 'att_name_info'
def open_excel(file= 'file.xls'):
    try:
        data = xlrd.open_workbook(file)
        return data
    except Exception,e:
        print str(e)
#根据索引获取Excel表格中的数据   参数:file：Excel文件路径     colnameindex：表头列名所在行的所以  ，by_index：表的索引
def excel_table_byindex(file= 'file.xls',colnameindex=0,by_index=0):
    data = open_excel(file)
    table = data.sheets()[by_index]
    nrows = table.nrows #行数
    ncols = table.ncols #列数
    colnames =  table.row_values(colnameindex) #某一行数据 
    list =[]
    for rownum in range(1,nrows):

         row = table.row_values(rownum)
         if row:
             app = []
             for i in range(len(colnames)):
                app.append(row[i]) 
             list.append(app)
    return list

#根据名称获取Excel表格中的数据   参数:file：Excel文件路径     colnameindex：表头列名所在行的所以  ，by_name：Sheet1名称
def excel_table_byname(file= 'file.xls',colnameindex=0,by_name=u'Sheet1'):
    data = open_excel(file)
    table = data.sheet_by_name(by_name)
    nrows = table.nrows #行数 
    colnames =  table.row_values(colnameindex) #某一行数据 
    list =[]
    for rownum in range(1,nrows):
         row = table.row_values(rownum)
         if row:
             app = {}
             for i in range(len(colnames)):
                app[colnames[i]] = row[i]
             list.append(app)
    return list

def make_store_file(tables):
    fname_dict={}
    for row in tables:
        one_line=""
        for valuse in row:
            one_line=one_line+valuse.replace('\t','').replace('\\','\\\\').replace('\'','')+"\t"
            #one_line=one_line+valuse.replace('\t','')+"\t"
        if not fname_dict.has_key(out_file):
            fname_dict[out_file]=[]
        fname_dict[out_file].append(one_line.strip().encode('utf-8').replace('\n','')+'\n')
    for fname, lines in fname_dict.items():
        fwritelines(fname, lines)

def fwritelines(fname, lines):
    with open(fname, 'w') as fp:
        fp.writelines(lines)
def get_db_conf():
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

def update_att_table():
    import psycopg2
    conn = psycopg2.connect(database = db_name,
                            user = user,
                            password = pwd,
                            host = host,
                            port = port)

    cur = conn.cursor()
    cur.execute("delete from %s;copy %s (threat_group,threat_name,cve_id,threat_describe,threat_harm,threat_suggest) from '%s'" % (table,table,out_file))
    conn.commit()
    cur.close()
    conn.close()

def main():
    ret=0
    if len(sys.argv) == 2:
        infname=sys.argv[1]
    else:
        print "ERROR:please input eg:%s /file.xls" % sys.argv[0]
        return 1
    get_db_conf()
    tables = excel_table_byindex(file=infname)
    make_store_file(tables)
    try:
        update_att_table()
    except Exception as ex:
        print ex
        ret=23
    os.remove(infname)
    return ret

if __name__=="__main__":
    exit(main())
