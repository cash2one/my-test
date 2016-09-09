#!/usr/bin/env python
# -*- coding: UTF-8 -*-

'''
@auther: wangkun
@date: March, 13, 2015
'''

import smtplib, types
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from collections import namedtuple

import os, shutil
import sys, socket, struct;
reload(sys);
import getopt;
import logging;
import time, datetime
sys.setdefaultencoding('utf8');

import ConfigParser
config = ConfigParser.ConfigParser()
config.read("../conf/email_object.ini")

# 引入日志模块一边记录信息 
from init_config import InitConfig as init_config
from attachment import EmailConTemplate as opera
#pera.init_send()
#pera.init_recv("event1")
#pera.init_security()
#pera.init_threshold()
sys.path.append("/gms/gapi/modules/est/log/")
sys.path.append("/gms/comm/")
import comm_data
import comm_common
from init_logger import init_logger
logger = init_logger()

# 事件分类，每一条事件指向了不同的目录  
# 根据不同的事件处理不同的附件信息 
ATTACHFILLE = {
               # "event1":'"/root/events_"+str(int(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))-5)+"-"+str(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))+".csv"', 
               "event1":'"/data/tmpdata/comm/attachment/events_{1}-"+str(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))+".csv"', 
               "event2":"",
               # "event3":'"/data/tmpdata/comm/alarm/events_"+str(int(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))-5)+"-"+str(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))+".csv"', 
               "event3":'"/data/tmpdata/comm/alarm/events_{1}-"+str(datetime.datetime.strftime(datetime.datetime.now(), "%Y%m%d%H%M"))+".csv"', 
               }
ATTACHSRC = {"event1":"/data/tmpdata/comm/mtx/email", 
             "event2":"",
             "event3":"/data/tmpdata/comm/alarm"
            }
MTXEmailPath = os.path.abspath("/gms/comm/conf/mtx_email.txt")
SysEmailPath = os.path.abspath("/gms/comm/conf/sys_email.txt")
AlarmEmailPath = os.path.abspath("/gms/comm/conf/alarm_email.txt")
AlarmEvent = os.path.abspath("/data/tmpdata/comm/alarm_event")
FileStyle = ["ok"]

# 日志内容格式转换函数
# 把传递过来的数据转换成gb2312 
def transcoder_email(args):
    if isinstance(args, unicode): 
        return args.encode('gb2312') 
    else: 
        return args.decode('utf-8').encode('gb2312')

# 开发时临时日志写的日志系统的日志目录生成函数  
def mk_folder(args):
    folder = args + time.strftime('%Y/%m/%d', time.gmtime())
    if not os.path.exists(folder):os.makedirs(folder)
    return folder

# 临时日志系统
logging.basicConfig(level=logging.DEBUG,  
                    format='%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s',  
                    datefmt='%a, %d %b %Y %H:%M:%S',  
                    filename='%s/%s.log' % (mk_folder("./"), logging.DEBUG) ,  
                    filemode='a')

# email参数处理的类
# 由于对各种参数的处理，保证email邮件功能的完整性   
class OptEmail(object):
    def __init__(self, args = None):
        self.args = args
        pass
    def run(self, opts):
        try:
            #opts,args = getopt.getopt(sys.argv[1:], "r:l:c:o:", ["help",]);
            print opts
            #opts =  dict(opts)
            if "-l" in opts and "-c" in opts and "-r" in opts:
		logging.info(opts["-r"]) 
                emailObject = EmailAction(self.args)
                mailto_list = []
                opts_list = opts["-r"].split(";")

                mailto_list += opts_list
                if emailObject.send_mail(mailto_list,opts["-l"],
					transcoder_email(opts["-c"]),opts["-e"]):  
                    print "send email success!"
                    return True
                else:  
                    print "send email failed!"
                    return False
            else:
                print self._usage();
            return True
        except getopt.GetoptError, e:
            print("\n"+str(e));
            print self._usage();
            sys.exit(1);
            return False

    # 命令行参数传递时，参数说明，类似help方法 
    def _usage(self):
        return '''
    ***********************************************************************************
    -r, -l and -c mast be present!!!
    
    -r   respien   
	 the respien's mailbox
    -l   headline translation 
         For the user to fill in the sending of the message subject;
    -c   context
         For the specific content of users to send mail;
   
    eg:  python email_object.pyc -l headline -c content -r 'xxx@chanct.com;xxx@163.com'
         (Mailbox name must be separated by semicolon)
    ***********************************************************************************
           '''

# email类，实现邮件功能的发送  
class EmailAction(object):
    def __init__(self, pera_dict=None):
        self.pera_dict = pera_dict
        if pera_dict == None:
            self.pera_dict = init_config().init_send()
        self.mail_host = transcoder_email(self.pera_dict["mail_server"])    #config.get("sender", "mail_host")
        self.mail_port = self.pera_dict["port"]                             #config.get("sender", "mail_port")
        self.mail_name = self.pera_dict["mail"].split("@")[0]               #config.get("sender", "mail_name")
        self.mail_user =  transcoder_email(self.pera_dict["head"])          #config.get("sender", "mail_user")
        self.mail_pass =  transcoder_email(str(self.pera_dict["password"])) #config.get("sender", "mail_pass")
        self.mail_postfix = self.pera_dict["mail"].split("@")[1]            #config.get("sender", "mail_postfix")
    # 邮件附件生成函数，
    # 调用了附件生成的模块，把附件生成在特定的目录下 
    def _create_att(self, eve_tag):
        if eve_tag == "event1":
            key = "event1"
        elif eve_tag == "event3":
            key = "event3"
        else:
            return False
        file_dict = opera().file_list(ATTACHSRC[key], ".ok")
        return opera().merge_file(file_dict, eval(ATTACHFILLE[key]), eve_tag)
        
    # 邮件附件载入函数，把邮件载入到MIMEMultipart对象中 
    def attachments(self, eve_tag):
        attachs = self._create_att(eve_tag)
        msg = MIMEMultipart()
        if attachs :
            atta = MIMEText(open(attachs[0], 'rb').read(), 'base64', 'gb2312')
            atta["Content-Type"] = 'application/octet-stream'
            atta["Content-Disposition"] = 'attachment; filename="'+os.path.basename(attachs[0])+'"'  
            msg.attach(atta)
            os.remove(attachs[0])
            data_time = attachs[1]
        else:
            data_time = ""
            pass
        return (msg,data_time)
    
    # 邮件发送函数，发送邮件（可以是带附件的邮件，也可以是不带附件的邮件） 
    def send_mail(self, to_list, sub, content, eve_tag):  
        try:
            sub = transcoder_email(self.pera_dict["head"]) + sub
            me=self.mail_user+"<"+ self.mail_name+"@"+self.mail_postfix+">"  
            msg = self.attachments(eve_tag)
            msg, data_time = msg
            if data_time:
                content = content.format(data_time, \
                          time.strftime("%Y%m%d%H%M%S", time.strptime(data_time, "%Y-%m-%d %H:%M:%S")))
            msg.attach( MIMEText(content,_subtype='plain',_charset='gb2312'))  
            msg['Subject'] = sub  
            msg['From'] = me
            msg['To'] = ";".join(to_list)  
            server = smtplib.SMTP( timeout = 10 )  
            server.connect(self.mail_host, self.mail_port)  
            server.login(self.mail_name,self.mail_pass)  
            server.sendmail(me, to_list, msg.as_string())  
            server.close()  
            # shutil.rmtree("/data/tmpdata/comm/attachment/") 
            # if os.path.isdir("/data/tmpdata/comm/attachment/"):
            #     os.mkdir("/data/tmpdata/comm/attachment/")
            return True  
        except Exception, e:  
            print str(e)  
            logging.error(str(e)) 
            return False  

#python email_object.pyc -l headline -c content -r 'xxx@chanct.com;xxx@163.com'
#opts,args = getopt.getopt(sys.argv[1:], "r:l:c:o:", ["help",]);
# mtx安全事件发送函数，发送安全事件的信息给客户
def email_mtx(content="content"):
    cont = ""
    # 深度报文检测系统 
    for i in init_config().init_recv("event1"):
        cont += i[0]+";"
    args = {"-l":"", "-c": content, "-r": cont, "-e": "event1"}
    opt_email = OptEmail()
    return opt_email.run(args)

# 获取ip和设备名称 
def get_sys_parameter():
    from xml.etree import ElementTree as ET
    try:
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        xml_tree = ET.parse(xml_file) 
        dom = xml_tree.find("gms_device/attr")
        ISOTIMEFORMAT = '%Y-%m-%d %H:%M:%S' 
        now = time.strftime( ISOTIMEFORMAT, time.localtime(time.time()))
        #logger.debug("==========get ip and name OK!===========")
        result = [dom.get("ip"), dom.get("name"), now]
    except Exception, ex:
        logger.error(ex)
        result = ["ip", "name", "now"]
    finally:
        return result

# 磁盘预警函数，用于发送磁盘预警信息 
# args = u"硬盘阈值达到" + str(args)+ "%"
def email_threshold(args=80):
    cont = ""
    for i in init_config().init_recv("event2"):
        cont += i[0]+";"
    with open(SysEmailPath) as fp:
        sysEmailFile = fp.read()
    content = sysEmailFile % tuple(get_sys_parameter()+[str(args)])
    args = {"-l":"", "-c": content, "-r": cont, "-e":"event2"}
    # 磁盘预警保护 
    # 0: 报警 (80,90) and psize ==0    护预警 >90 and psize==1 ，并置0; 3: 不做任何操作
    tag = init_config().init_threshold().get("psize", 3) 
    opt_email = OptEmail()
    return opt_email.run(args)

# 邮件功能添加时，发送的邮件 
def email_test(cont, Opera = None, content = u"this test is OK!"):
    cont = cont
    args = {"-l":"", "-c": content, "-r": cont, "-e": "test"}
    opt_email = OptEmail(Opera)                                                                               
    return opt_email.run(args)

# 
def get_path_size(argsStr):
    if not os.path.exists(argsStr):
        return 0
    if os.path.isfile(argsStr):
        return os.path.getsize(argsStr)

    totalSize = 0L
    for root, dirs, files in os.walk(argsStr):
        totalSize += sum([os.path.getsize(os.path.join(root, name)) for name in files])
    return totalSize

def mtx_data(event = "event1"):
    file_dict = opera().file_list(ATTACHSRC[event], ".ok")
    count = 0
    for file in file_dict[ATTACHSRC[event]]:
        with open(ATTACHSRC[event] + "/" + file , "rU") as fp:
             count += len(fp.readlines())
    sys_args = get_sys_parameter()
    if event == "event3":
        sys_args[0], sys_args[1] = ("", "")
    time_start = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(int(time.time()) -  int(comm_data.mtx_mail_warning)))
    if event == "event3" or event == "event1":
        time_start = "{0}"
    sys_args.insert(-1, time_start) 
    sys_args.append(str(count))
    return sys_args

def email_mtx_run():
    while comm_common.is_continue(): 
        try:
            # os.system("cp -rf " + ATTACHSRC["event1"] + " " + AlarmEvent)
            # 添加给学生发邮件的功能
            comm_common.creat_if_dir_notexist(AlarmEvent + "/email")
            comm_common.creat_if_dir_notexist("/data/tmpdata/comm/attachment")

            mtx_data_list = mtx_data()
            if get_path_size(ATTACHSRC["event1"]) and mtx_data_list[-1] != "0":
                from init_config import InitConfig as init_config
                with open(MTXEmailPath) as fp:
                    mtxEmailFile = fp.read()
                mtx_data_list.append(os.path.basename(eval(ATTACHFILLE["event1"])))
                mtxEmailFile = mtxEmailFile % tuple(mtx_data_list) 
                email_mtx(str(mtxEmailFile))
                logger.debug("==========email_mtx_run OK!===========")
                # 添加给学生发邮件的功能
                email_alarm_run()
        except Exception, ex:
            print "==========email mtx run error!==========="
            logger.error(ex)
        finally:
            time.sleep(int(comm_data.mtx_mail_warning))

def email_alarm(content="content", event = "event3", cont=""):
    # 深度报文检测系统 
    #for i in init_config().init_recv(event):
    #    cont += i[0]+";"
    args = {"-l":"", "-c": content, "-r": cont, "-e": event}
    opt_email = OptEmail()
    return opt_email.run(args)

# an ge ge shi jian te zheng pai cheng list[],
# chuang jian xue sheng wen jian , 
# xun huan xie ru, 
# mei you jiu chuang jian 
# chu li wan zai email_alarm_run li yi dong mu lu 
def alarm_event(event_file):
    if event_file == "event3":
        tmp_dict = {}
        tmp_list = [AlarmEvent + "/" + "email"]
        alarm_event_list = os.walk(AlarmEvent)
        for i in alarm_event_list:
            if i[0] == AlarmEvent + "/" + "email" or i[0] == AlarmEvent + "\\" + "email":
              for q in i[2]:
                #print q
                if os.path.splitext(q)[1][1:] == FileStyle[0]:
                    with open(i[0] + "/" + q) as fp:
                        data_out = fp.readlines()
                    if data_out: 
                        try:
                            tmp_dict = {}
                            for j in data_out:
                                data_list = j.split("|")
                                try:
                                    if long(data_list[6]):
                                        if tmp_dict.has_key(data_list[6]):
                                            tmp_dict[data_list[6]].append(data_list)
                                        else:
                                            tmp_dict.update({data_list[6]:[data_list]})
                                        if tmp_dict.has_key(data_list[7]):  
                                            tmp_dict[data_list[7]].append(data_list)
                                        else:
                                            tmp_dict.update({data_list[7]:[data_list]})
                                except:
                                    if tmp_dict.has_key(data_list[11]):            
                                        tmp_dict[data_list[11]].append(data_list)                          
                                    else:                                                                  
                                        tmp_dict.update({data_list[11]:[data_list]})                         
                                    if tmp_dict.has_key(data_list[12]):            
                                        tmp_dict[data_list[12]].append(data_list)                          
                                    else:                                                                  
                                        tmp_dict.update({data_list[12]:[data_list]})
                            for j, k in tmp_dict.items():
                                try:
                                    with open(i[0] + "/" + str(j), "a") as fp:
                                        for e in k:
                                            fp.write("|".join(e))
                                except Exception, ex:
                                    logger.error(ex)
                                    print ex
                                    #fp.write("|".join(k))
                                    #for e in k:
                                    #    fp.write("|".join(e))
                        except Exception, ex:
                            logger.error(ex)
                    if os.path.exists(i[0] + "/" + q):os.remove(i[0] + "/" + q)
        return (tmp_dict.keys(), tmp_list)
    else:
        return False

def alarm_merge(event):
    for i in event.file_list:
        os.system("mv " + event.path_list[0]+ "/" + "i" + " " +  ATTACHSRC[event.name])
    return True

def email_alarm_run():
        comm_common.creat_if_dir_notexist(ATTACHSRC["event3"])
        # while comm_common.is_continue(): 
        from pgapis import PgApi as pgapi
        try:
            if pgapi().status() != "" and pgapi().status() != "0":
                alarm_events = alarm_event("event3")
                if alarm_events :
                    user_dict = pgapi().select()
                    Event=namedtuple('Event','name file_list path_list')
                    alarm_events = Event(name="event3", file_list=alarm_events[0], path_list=alarm_events[1])
                    # alarm_merge(alarm_events)
                    for i in alarm_events.file_list:
                       if user_dict.get(long(i), None) and os.path.exists(alarm_events.path_list[0] + "/" + i):
                           os.system("cp -a " + alarm_events.path_list[0]+ "/" + i + " " + ATTACHSRC[alarm_events.name]+"/"+i+".ok")
                           alarm_data_list = mtx_data("event3")
                           if get_path_size(ATTACHSRC["event3"]) and alarm_data_list[-1] != "0":
                               from init_config import InitConfig as init_config
                               with open(AlarmEmailPath) as fp:
                                   alarmEmailFile = fp.read()
                               alarm_data_list.append(os.path.basename(eval(ATTACHFILLE["event3"])))
                               alarmEmailFile = alarmEmailFile % tuple(alarm_data_list) 
                               #print user_dict.get(long(i), None)
                               email_alarm(str(alarmEmailFile), event = "event3", cont=user_dict.get(long(i), None))
                       else:
                           pass
                       if os.path.exists(alarm_events.path_list[0] + "/" + i):
                           os.remove(alarm_events.path_list[0]+ "/" + i)
                    #logger.debug("==========email_alarm_run OK!===========")
        except Exception, ex:
            logger.error(ex)
            print "==========email alarm run error!==========="
        finally:
            # time.sleep(int(comm_data.alarm_mail_warning))
            pgapi().close()

if __name__ == '__main__':  
    # print email_threshold()
    # print email_mtx()
    # print email_test("wangkun1@chanct.com", content="ok!")
    # print mtx_data()
    # print email_alarm()
    print alarm_event("event3")
    #print email_alarm_run()
