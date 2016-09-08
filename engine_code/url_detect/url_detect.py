#!/usr/local/bin/python
# _*_ coding: utf8 _*_

import os
import sys
import time
import socket
import signal
import shutil
import traceback
import ConfigParser
from urllib import unquote
from urlparse import urlparse

# ? 这么导入好么 ?
sys.path.append('../comm/')
sys.path.append('/gms/comm/')
import comm_common
import comm_log_record
import white_list
from sqlchop import *

url_rule_cnt = 0
rep_rule_cnt = 0
def create_url_detect_func():
    def url_add_proto_header(url_str):
        # 据说暂时都是 http 的，如果找不到协议头就在前面加 http
        for i in range(len(url_str)):
            if not url_str[i].isalpha():
                try:
                    if url_str[i] == ':' and url_str[i+1] == '/' and url_str[i+2] == '/':
                        return url_str
                except IndexError:
                    break
        return "http://" + url_str

    def url_normalize(url_str):
        # 看来无法识别一个url是否合法。即使看起来不是一个合法的 url ，也不会抛出异常。
        url = urlparse(unquote(url_add_proto_header(url_str)))
        # !!! 这里端口号被过滤掉了。考虑是不是需要加上。 #print url
        return "%s://%s%s;%s?%s#%s" % (url.scheme,
                                        url.hostname,
                                        url.path,
                                        url.params,
                                        url.query,
                                        url.fragment)

    def url_match(url):
        #print("normalize url = [%s]" % url_normalize(url))
        #for k, v in url_dict.items():
        #    if k.find("gg46") != -1:
        #        print("k =  [%s]" % k)
        if url_dict.has_key(url_normalize(url)):
            return True
        else:
            return False

    url_dict = {}
    rule_dir = global_config['rule_dir']
    rule_files = comm_common.file_filter_bysuffix([rule_dir], ['.rule'])
    if len(rule_files) == 0:
        global_logger.warn("warning. can't find rule file.")
    else:
        global url_rule_cnt
        url_rule_cnt = 0
        for rule_file in rule_files:
            for line in open(rule_file):
                url = str.strip(line)
                if len(url) > 0:
                    if url_dict.has_key(url_normalize(url)):
                        global rep_rule_cnt
                        rep_rule_cnt += 1
                    else:
                        url_dict[url_normalize(url)] = True
                        url_rule_cnt += 1

    return url_match

def url_line_parse(url_line):
    def make_url(host, url, param):
        #print("host[%s] url[%s] param[%s]" % (host, url, param))
        if host == '\N': host = ''
        if url == '\N': url = ''

        if param == '\N':
            return "http://%s%s" % (host, url)
        else:
            return "http://%s%s;%s" % (host, url, param)

    def fassign(field_str, func=lambda x:x):
        if field_str == '' or field_str.upper() == 'NULL':
            return '\N'
        else:
            return func(field_str)

    #url_line = url_line.decode('utf-8')
    # 如果在这里转为 utf-8。strip 时会报错。strip 要求的对象为 str 类型。
    # 行格式: sip dip sport dport url method host user_agent param referer timenow
    #         sip_str dip_str time_str
    # 字段之间以"\t"分割，其中IP、TIME为整型输出
    flist = map(str.strip, url_line.split('\t'))
    fdict = {}

    fdict['sip'] = fassign(flist[0], lambda ip: str(socket.ntohl(int(ip, 10))))
    fdict['dip'] = fassign(flist[1], lambda ip: str(socket.ntohl(int(ip, 10))))
    fdict['sport'] = fassign(flist[2])
    fdict['dport'] = fassign(flist[3])
    # 源端口和目的端口已经是字符串 80
    #fdict['sport'] = fassign(flist[2], lambda n: str(socket.ntohs(int(n, 10))))
    #fdict['dport'] = fassign(flist[3], lambda n: str(socket.ntohs(int(n, 10))))

    fdict['url'] = fassign(flist[4])
    fdict['src_url'] = fassign(flist[4])
    fdict['method'] = fassign(flist[5])

    # 现在看到的Host格式: Host:\20x.x.x.x
    if ':' in flist[6]:
        fdict['host'] = fassign(flist[6], lambda h: h.split(':')[1].replace('\\20', '').strip())
    else:
        fdict['host'] = fassign(flist[6])

    # 现在看到的User-Agent格式: User-Agent:\20......
    if ':' in flist[7]:
        fdict['user_agent'] = fassign(flist[7], lambda u: u.split(':')[1].replace('\\20', ' ').strip())
    else:
        fdict['user_agent'] = fassign(flist[7])

    # 有时候没有值，也不是 NULL
    fdict['param'] = fassign(flist[8])
    fdict['referer'] = fassign(flist[9])
    fdict['timenow'] = fassign(flist[10])
    fdict['sip_str'] = fassign(flist[11])
    fdict['dip_str'] = fassign(flist[12])
    fdict['time_str'] = fassign(flist[13])

    fdict['url'] = make_url(fdict['host'], fdict['url'], fdict['param'])
    return fdict
#add by zdw
def sqlchop_detect(url):
    #return True
    resoult=global_detector.classify({'urlpath': url }, True)
    if resoult:
        if [w["score"] for w in resoult["payloads"] if w["score"] > float(global_config['sqlchop_score'])] :
            global_logger.info("sql thread[monitor suc]: %s" % url)
            return True
        else:
            global_logger.info("sql thread[monitor safe]: %s" % url)
            return False
def url_file_detect(url_file):
    def make_file_name(url_file, timenow,file_path):
        # 这里每隔一分钟创建一个文件。并截取原文件名结尾的线程ID作为文件名的一部分。
        #dst_dir = global_config['output_dir']
        dst_dir = file_path
        # 过去的文件名是 20140403145800.00。其中 00 是线程 ID。
        # 现在的文件名中没有线程 ID
        timestamp = time.strftime("%Y%m%d%H%M00", time.localtime(timenow))
        fname = os.path.join(dst_dir, "%s_%s.ok" % (timestamp, global_devid))
        return fname
    def get_md_from_time(timenow):
        struct_time=time.localtime(timenow)
        m='%02d' % struct_time.tm_mon
        d='%02d' % struct_time.tm_mday
        return m,d

    global total_line_cnt, error_line_cnt, error_file_cnt, surl_cnt
    fname_dict = {}
    fname_sql_dict = {}
    error_line_flag = False
    line_num = 0
    for line in open(url_file):
        line_num += 1
        total_line_cnt += 1
        try:
            field_dict = url_line_parse(line)
            #print field_dict

            sip = field_dict['sip']
            dip = field_dict['dip']
            #print("url detect ---------------:%s %s" % ( sip, dip))
            if sip == '\N':
                sip = 0
            else:
                sip = int(sip)
            if dip == '\N':
                dip = 0
            else:
                dip = int(dip)
            #print("url detect ---------------:%d %d" % ( sip, dip))

            if white_list.enable_white_list():
                if white_list.lookup_white_list(sip) or white_list.lookup_white_list(dip):
                    #print("skip linum %d" % line_num)
                    continue

            if global_detect_func(field_dict['url']):
                fname = make_file_name(url_file, int(field_dict['timenow']),global_config['output_dir'])
                data = "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n" % (global_devid,
                                                    field_dict['sip'], field_dict['dip'],
                                                    field_dict['url'], field_dict['method'],
                                                    field_dict['host'], field_dict['user_agent'],
                                                    field_dict['param'], field_dict['time_str'],
                                                    field_dict['sport'], field_dict['dport'],
                                                    field_dict['sip_str'], field_dict['dip_str'],
                                                    field_dict['referer'])
                if not fname_dict.has_key(fname):
                    fname_dict[fname] = []
                fname_dict[fname].append(data.encode('utf-8'))
                surl_cnt += 1
            if global_config['sqlchop_switch'] == '1' and sqlchop_detect(field_dict['src_url']):
                fname = make_file_name(url_file, int(field_dict['timenow']),global_config['sql_out_dir'])
                m,d=get_md_from_time(int(field_dict['timenow']))
                data = "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n" % (global_devid,global_store['ename'],
                                                    global_store['nameid'],
                                                    global_store['isccserver'],
                                                    global_store['etype'],
                                                    field_dict['time_str'],field_dict['time_str'],
                                                    field_dict['time_str'],global_store['eventtype'],
                                                    global_store['eventbasetype'],global_store['eventexttype'],
                                                    #field_dict['dip'], field_dict['sip'],
                                                    #field_dict['sport'], field_dict['dport'],
                                                    field_dict['sip'], field_dict['dip'],
                                                    field_dict['dport'], field_dict['sport'],
                                                    "host="+field_dict['host']+";url="+field_dict['src_url']+";method="+field_dict['method']+";referer="+field_dict['referer']+";user_agent="+field_dict['user_agent'],
                                                    global_store['enum'],global_store['risk_level'],'\\N',
                                                    m,d)
                if not fname_sql_dict.has_key(fname):
                    fname_sql_dict[fname] = []
                fname_sql_dict[fname].append(data.encode('utf-8'))
                global_logger.info("sql_file___________________%s" % fname)
                
        except:
            if error_line_flag == False:
                error_line_flag = True
                error_file_cnt += 1
                shutil.copy(url_file, global_config['backup_dir'])
            error_line_cnt += 1
            global_logger.warn("url file[%s] line[%d] format ERROR." % (url_file, line_num))
            global_logger.info("%s" % traceback.format_exc())

    for fname, lines in fname_dict.items():
        fwritelines(fname, lines)
    for fname, lines in fname_sql_dict.items():
        fwritelines(fname, lines)

def fwritelines(fname, lines):
    def make_unique_file_name(dname, fname):
        name = os.path.join(dname, fname)
        if os.path.exists(name):
            # 这个文件名不应该存在，如果现在的处理有问题，应该调整流程
            cnt = 1
            basename, extname = os.path.splitext(name)
            while True:
                newname = "%s_(%d)%s" % (basename, cnt, extname)
                if os.path.exists(newname):
                    cnt += 1
                    continue
                else:
                    name = newname
                    break
        return name

    if os.path.exists(fname) and os.path.isfile(fname):
        dname, basename = os.path.split(fname)
        fname = make_unique_file_name(dname, basename)

    # 先写入该目录下的临时文件，后缀为 .tmp，写完后改名
    tmpname = fname + '.tmp'
    with open(tmpname, 'w') as fp:
        fp.writelines(lines)
    os.rename(tmpname, fname)

global_config = {}
global_logger = None
global_detect_func = None
global_devid = None
global_detector = None
global_store = {}
def init_config():
    def get_config_file():
        default_config_file = '/gms/url_detect/conf/url_detect.conf'
        if len(sys.argv) == 2:
            config_file = sys.argv[1]
        else:
            config_file = default_config_file

        if os.path.exists(config_file):
            return config_file
        else:
            raise(IOError("config file[%s] no exist." % config_file))

    def parse_config():
        global global_config
        cf = ConfigParser.ConfigParser()
        cf.read(get_config_file())
        global_config['input_dir'] = cf.get('global', 'input_dir')
        global_config['output_dir'] = cf.get('global', 'output_dir')
        global_config['backup_dir'] = cf.get('global', 'backup_dir')
        global_config['restore_dir'] = cf.get('global', 'restore_dir')
        global_config['rule_dir'] = cf.get('global', 'rule_dir')
        global_config['log_dir'] = cf.get('global', 'log_dir')
        global_config['sql_out_dir'] = cf.get('sqlchop', 'output_dir')
        global_config['sqlchop_score'] = cf.get('sqlchop', 'score')
        global_config['sqlchop_switch'] = cf.get('sqlchop', 'sqlchop_switch')
        #global_config['uuid_conf_file'] = cf.get('global', 'uuid_conf_file')

    def check_config():
        #uuid_conf_file = global_config['uuid_conf_file']
        #if not os.path.exists(uuid_conf_file) or not os.path.isfile(uuid_conf_file):
        #    raise(IOError("uuid conf file[%s] no exist." % uuid_conf_file))
        comm_common.raise_if_dir_notexist(global_config['rule_dir'])
        comm_common.raise_if_dir_notexist(global_config['input_dir'])
        comm_common.creat_if_dir_notexist(global_config['output_dir'])
        comm_common.creat_if_dir_notexist(global_config['backup_dir'])
        comm_common.creat_if_dir_notexist(global_config['restore_dir'])
        comm_common.creat_if_dir_notexist(global_config['log_dir'])
        comm_common.creat_if_dir_notexist(global_config['sql_out_dir'])

    def init_logger():
        global global_logger
        class URLDetectFileLogger(comm_log_record.FileLogger):
            def logfilebasename(self, timestamp):
                date = time.strftime("%Y%m%d", time.localtime(timestamp))
                return date + '.log'

        LOG_FORMAT = '==%(asctime)s *%(levelname)s* [%(funcName)s:%(lineno)d]== %(message)s'
        global_logger = URLDetectFileLogger(path = global_config['log_dir'], format = LOG_FORMAT)

    def init_detect():
        global global_detect_func
        global_detect_func = create_url_detect_func()

    def read_uuid():
        try:
            ret, stdout, stderr = comm_common.system(str('/gms/gapi/modules/auth/genkey.pyc'))
        except Exception as ex:
            raise comm_common.CommException('read uuid fail. [%s].' % str(ex))
        else:
            if ret == 0:
                return stdout
            else:
                raise comm_common.CommException("exec uuid script[%s]: ret[%s] stdout[%s] stderr[%s]" % (comm_data.uuid_script, ret, stdout, stderr))
    def init_devid():
        global global_devid
        #fixed by zdw
        #with open(global_config['uuid_conf_file']) as fp:
        #    global_devid = fp.read().strip()
        global_devid=read_uuid()
    def init_sqlchop():
        global global_detector
        global_detector = SQLChop()
    def init_sqlchop():
        global global_detector
        global_detector = SQLChop()
    def init_sqlchop_store():
        global global_store
        global_store['ename']=u'攻击-网站-sql注入'
        global_store['nameid']='\\N'
        global_store['isccserver']='0'
        global_store['etype']='3'
        #time
        global_store['eventtype']=u'网站'
        global_store['eventbasetype']=u'攻击'
        global_store['eventexttype']=u'sql注入'
        global_store['risk_level']='4'
        global_store['enum']='1'




    parse_config()
    check_config()
    init_logger()
    init_detect()
    init_devid()
    init_sqlchop()
    init_sqlchop_store()

error_conf_file_cnt = 0
def reload_config():
    global global_config, global_logger, global_detect_func, global_devid, error_conf_file_cnt
    tmp_config = global_config
    tmp_logger = global_logger
    tmp_detect_func = global_detect_func
    tmp_devid = global_devid
    try:
        init_config()
        global_logger.info("reload config file success.")
    except:
        global_config = tmp_config
        global_logger = tmp_logger
        global_detect_func = tmp_detect_func
        global_devid = tmp_devid
        error_conf_file_cnt += 1
        global_logger.warn("%s" % traceback.format_exc())
        global_logger.warn("reload config file fail.")

signal_hup_cnt = 0
def init_signal():
    def signal_handle(sig, stack):
        if sig == signal.SIGINT:
            global stop_flag
            stop_flag = True
            global_logger.info("recv signal[INT].")
        elif sig == signal.SIGHUP:
            global reload_flag, signal_hup_cnt
            reload_flag = True
            signal_hup_cnt += 1
            global_logger.info("recv signal[HUP].")
        else:
            global_logger.info("recv signal[%d]." % sig)

    signal.signal(signal.SIGINT, signal_handle)
    signal.signal(signal.SIGHUP, signal_handle)

reload_flag = False
def isreload():
    global reload_flag
    if reload_flag:
        reload_flag = False
        return True
    else:
        return False

stop_flag = False
def iscontinue():
    if stop_flag:
        return False
    else:
        return True

busy_sec = 0
begin_time = 0
total_file_cnt = 0
total_line_cnt = 0
error_file_cnt = 0
error_line_cnt = 0
surl_cnt = 0

last_dump_sec = 0
def exec_info_dump(delay_sec = 60, dump_delay = True):
    global last_dump_sec
    if dump_delay:
        cur_sec = time.time()
        if cur_sec - last_dump_sec < delay_sec:
            return
        else:
            last_dump_sec = cur_sec

    global_logger.info("******* URL DETECT EXEC INFO *******")
    global_logger.info("detect [%d] line spend [%d] seconds." % (total_line_cnt - error_file_cnt,
                                                                busy_sec))
    global_logger.info("surl cnt = %d" % surl_cnt)
    global_logger.info("total url file cnt = %d" % total_file_cnt)
    global_logger.info("total url line cnt = %d" % total_line_cnt)
    if error_file_cnt > 0:
        global_logger.info("error url file cnt = %d" % error_file_cnt)
    if error_line_cnt > 0:
        global_logger.info("error url line cnt = %d" % error_line_cnt)
    if signal_hup_cnt > 0:
        global_logger.info("reload signal cnt = %d" % signal_hup_cnt)
        if error_conf_file_cnt > 0:
            global_logger.info("error conf file cnt = %d" % error_conf_file_cnt)
    # 把更新白名单放到这里。每隔60秒更新一次。

    if white_list.enable_white_list():
        try:
            wlist = white_list.update_white_list()
            global_logger.info("update white list. cur white list length: [%d]" % len(wlist))
        except:
            global_logger.info(traceback.format_exc())

def url_file_list():
    def url_dir_expired(dname):
        dir_date = int(os.path.split(dname)[-1])
        cur_date = int(time.strftime("%Y%m%d", time.localtime(time.time())))
        if cur_date > dir_date:
            return True
        else:
            return False

    def url_dir_list():
        src_dir = global_config['input_dir']
        dname_list = os.listdir(src_dir)
        dname_list.sort()
        for dname in dname_list:
            dname = os.path.join(src_dir, dname)
            if os.path.isdir(dname) and os.path.basename(dname).isdigit():
                #print("+ %s" % dname)
                yield dname
                if url_dir_expired(dname) and os.listdir(dname) == []:
                    os.rmdir(dname)

    for url_dir in url_dir_list():
        for flag_fname in comm_common.file_filter_bysuffix(url_dir, ['.ok']):
            #data_fname = flag_fname[:-3]#fixed by zdw
            data_fname = flag_fname
            if os.path.exists(data_fname):
                #print("- %s" % data_fname)
                yield data_fname
                os.remove(flag_fname)
                #os.remove(data_fname)#fixed by zdw
            else:
                global_logger.warn("data file[%s] not found." % data_fname)
                #os.remove(flag_fname)#fixed by zdw

def url_file_restore(url_file):
    restore_dir = global_config['restore_dir']
    date = os.path.basename(url_file)[:8]
    year = date[:4]
    mon = date[4:6]
    day = date[6:]
    dst_dir = os.path.join(restore_dir, year, mon, day)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    shutil.copy(url_file, dst_dir)

def main():
    global begin_time, busy_sec, total_file_cnt
    begin_time = time.time()

    init_config()
    init_signal()
    global_logger.info("******* URL DETECT PROCESS BEGIN *******")
    global_logger.info("url dir     = [%s]" % global_config['input_dir'])
    global_logger.info("surl dir    = [%s]" % global_config['output_dir'])
    global_logger.info("backup dir  = [%s]" % global_config['backup_dir'])
    global_logger.info("restore dir = [%s]" % global_config['restore_dir'])
    global_logger.info("rule dir    = [%s]" % global_config['rule_dir'])
    global_logger.info("log dir     = [%s]" % global_config['log_dir'])
    #global_logger.info("uuid conf file = [%s]" % global_config['uuid_conf_file'])
    global_logger.info("url rule cnt   = [%d]" % url_rule_cnt)
    global_logger.info("rep rule cnt   = [%d]" % rep_rule_cnt)

    if white_list.enable_white_list():
        global_logger.info('enable white list.')
        try:
            white_list.create_white_list()
        except:
            global_logger.info(traceback.format_exc())
    else:
        global_logger.info('disable white list.')

    while iscontinue():
        try:
            begin_sec = time.time()
            if isreload(): reload_config()

            for url_file in url_file_list():
                global_logger.info('url_file= %s' % url_file)
                if iscontinue():
                    total_file_cnt += 1
                    url_file_detect(url_file)
                    url_file_restore(url_file)
                else:
                    break
        except:
            global_logger.info(traceback.format_exc())
        finally:
            busy_sec += time.time() - begin_sec
            exec_info_dump()
            time.sleep(0.5)

    global_logger.info("******* URL DETECT PROCESS EXIT *******")
    exec_info_dump(dump_delay = False)

if __name__ == '__main__':
    main()
