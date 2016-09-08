#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import sys
import getopt
from xml.etree import ElementTree as ET

from log.init_logger import init_logger 
logger = init_logger()
#logger.debug('This is debug message')

# 引入email信息发送功能 
sys.path.append('/gms/comm/est/')
from email_object import email_test

def transcoder_email(args):
    if isinstance(args, unicode): 
        return args.encode('gb2312') 
    else: 
        return args.decode('utf-8').encode('gb2312')

# xml操作类 
class OperateXml(object):
    def __init__(self):
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        self.root = ET.parse(xml_file)

# email配置文件的信息修改 
class Email(object):
    def __init__(self, *args, **kargs):
        self.args = args[0]
        self.kargs = kargs
        self.tag = 0
    def run(self):
        print self.args
        xml_file = os.path.abspath("/gms/conf/gmsconfig.xml")
        xml_tree = ET.parse(xml_file)
        dom = xml_tree.find("email")
        # 配置成默认email发件邮箱的发送 
        if str(self.args.get("-y")) == "1" and str(self.args.get("-w")) == "1" :
            dom.set("switch", '1')
            dom.set("head", transcoder_email(str(self.args.get('-h'))))
            xml_tree.write(xml_file)
            logger.info('This is info message')
            print "restart"
        # 配置成第三方邮箱发送 
        elif str(self.args.get("-y")) == "1" and str(self.args.get("-w")) == "2" :
            sen_oth = dom.find("send_other")
            sargs = {'mail': self.args['-a'], 'head':transcoder_email(self.args['-h']), 'password': self.args['-p'],\
                     'mail_server': self.args['-s'], 'port': self.args['-t']}
            logger.debug(sargs)
            if self.args.get('-a', None ) and not email_test(str(self.args.get('-a')), sargs):
                sys.exit(201)
            elif not self.args.get('-a', None ) and not email_test(str(sen_oth.find("mail").text), sargs):
                sys.exit(201)
            else:
                sen_oth.find("mail").text = str(self.args.get('-a'))
                sen_oth.find("pw").text = str(self.args.get('-p')) 
                sen_oth.find("mail_server").text = str(self.args.get('-s')) 
                sen_oth.find("port").text = str(self.args.get('-t'))
                dom.set("switch", '2')
                dom.set("head", transcoder_email(str(self.args.get('-h'))))
                xml_tree.write(xml_file)
                #logger.debug('This is debug message')
            print "restart"
        # 配置收件人信息 
        elif str(self.args.get("-y")) == "2":
            # 添加收件人信息 
            if self.args["-o"] == "c":
                #if not email_test(str(self.args["-r"])):
                #    sys.exit(201)
                #else:
                cont = dom.find("container")
                if not self.args.get("-l"):
                    self.args["-l"] = "1"
                cont.append(ET.Element("rec", 
                    {"event1":str(self.args["--g1"]), "id": str(self.args["--id"]), "event2": str(self.args["--g2"]), "level": str(self.args["-l"]), "mail":str(self.args["-r"]), "reci": str(self.args.get("-m")), "switch": str(self.args["-d"]) }))
                xml_tree.write(xml_file)
                print "restart"
                #logger.debug('This is debug message')
            #event1="1" event2="0" level="1" mail="xxx@126.com" reci="zhangsan" switch="1"
            # 修改收件人信息 
            elif self.args["-o"] == "u":
                cont =  dom.find("container")
                for child in cont.getchildren():
                    if child.get("id") == str(self.args["--id"]):
                        if not self.args.get("-l"):
                            self.args["-l"] = "1"
                        #if not email_test(str(self.args["-r"])):
                        #    sys.exit(201)
                        #    self.args["-l"] = "1"
                        #else:
                        child.set("id",str(self.args["--id"]))
                        child.set("event1",str(self.args["--g1"]))
                        child.set("event2",str(self.args["--g2"])) 
                        child.set("level",str(self.args["-l"])) 
                        child.set("mail",str(self.args["-r"])) 
                        child.set("reci",str(self.args.get("-m")))
                        child.set("switch",str(self.args["-d"]))
                        xml_tree.write(xml_file)
            # 删除收件人 
            elif self.args["-o"] == "d":
                cont = dom.find("container")
                d_mail_list = str(self.args["-r"]).split(";") 
                #####################################
                for d_mail in d_mail_list:
                    for child in cont.getchildren():
                        if child.get("mail") == d_mail :
                            cont.remove(child)
                ######################################
                xml_tree.write(xml_file)
                print "restart"
                #logger.debug('This is debug message')
            else:
                #logger.debug('This is debug message')
                print "error"
                self.tag = 1
                sys.exit(120)
        else:
            #logger.debug('This is debug message')
            print "error"
            self.tag = 1
            sys.exit(120)
        return self.tag
            

def main():
    opts,args = getopt.getopt(sys.argv[1:],"o:w:y:a:p:h:s:t:d:m:r:g:l:",["g1=", "g2=", "id="])    
    print opts, args
    logger.warning('This is user\'s opear message for email: %s' % opts)
    email = Email(dict(opts), args = args)
    return email.run()
if __name__ == '__main__':
    exit(main())
