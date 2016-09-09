#!/usr/bin/env python
#_*_coding:utf-8_*_

import httplib,urllib,urllib2
import json,os,time,traceback
import comm_data
from  js_log import record_log_init
import ConfigParser

'''
You can also use the following if you are using json.
json.dumps(data)
Remember: urlencode can encode a dict, but not a string. The output of json.dumps is a string.
'''
logger = ''
def Reg_from_server(host,url,username, LUID):
    port = 443
    interval = 30
    method = 'POST'
    httpClient = None
    try:
        UID = {"hostname":username,"ComUID":LUID} 
        parm = json.dumps(UID)
        httpsClient = httplib.HTTPSConnection(host,port,timeout=interval)
        httpsClient.request(method,url,parm)
        response = httpsClient.getresponse()
        return (response.status,response.read())
    except Exception, e:
        print e
    finally:
        if httpClient:
            httpClient.close()    

def dump2reg(dicts):
    if not dicts.get('ClientUID'):
        print 'Recv ClientUID is Null.'
        return
    with open('/gms/conf/js.reg','wb') as fp:
        fp.writelines(dicts.get("ClientUID"))
    fp.close()

g_dict = {}
def init_conf():
    global logger
    global g_dict

    logger = record_log_init()

    configHandler = ConfigParser.ConfigParser()
    configHandler.read(os.path.join(os.path.dirname(os.path.abspath(__file__)),'js.conf'))
    g_dict['url'] = configHandler.get('g_js','url') 
    g_dict['host'] = configHandler.get('g_js','host') 
    g_dict['luid'] = configHandler.get('g_js','LUID') 
    g_dict['username'] = configHandler.get('g_js','username') 
    g_dict['regfile'] = configHandler.get('reg','regfile') 

def detectData():
    try:
        init_conf()
    except ConfigParser.NoOptionError:
        logger.error('%s' %traceback.format_exec())
    try:
        url  = g_dict['url']
        LUID = g_dict['luid'] 
        host = g_dict['host'] 
        username = g_dict['username'] 
        regfile = g_dict['regfile'] 
        logFormat = {"ComUID":LUID,"ClientUID":"","log":[]}
        event_type = {u'木马': '1',u'僵尸网络': '1',u'网站': '2',u'攻击': '3',u'移动互联网恶意代码': '4'}
    
        if not os.path.isfile(regfile):
            rstatus,contents = Reg_from_server(host,url,username,LUID)
            if rstatus != 200:
                print 'No connection to hosts .'
                logger.error('%s' %traceback.format_exec())
                return
            content = json.loads(contents)
            dump2reg(content) 
            logFormat["ClientUID"] = content["ClientUID"]
        else:
            with open(regfile,"rb") as fp:
                datas = fp.readlines()
                logFormat["ClientUID"] = datas[0]
        #print logFormat
    
        if not os.path.isdir(comm_data.comm_js_path):
            os.mkdirs(comm_data.comm_js_path)

        listfile = os.listdir(comm_data.comm_js_path)

        for js_file in listfile:
            js_file = os.path.join(comm_data.comm_js_path,js_file)
            if os.path.splitext(js_file)[-1] != ".ok":
                os.remove(js_file)
                continue
            with open(js_file,'rb') as fp:
                data_list = fp.readlines()
                datas = json.loads(data_list[0])
                data_map_list = datas.split("\n")
                log_list = []
                for i in range(0,len(data_map_list) - 1):
                    data_map = json.loads(data_map_list[i])
                    data = {"t":"","a":"","si":"","di":"","dp":""}
                    data["si"] = data_map["eventSrcIP"]
                    data["di"] = data_map["eventDstIP"]
                    data["dp"] = data_map["eventDetail"]
                    time_a = data_map["eventTime"]
                    time_event = time.mktime(time.strptime(time_a,'%Y%m%d %H:%M:%S'))
                    data["t"] = time_event
                    eventBaseType = data_map["eventBaseType"]
                    eventType = data_map["eventType"]
                    eventExtType = data_map["eventExtType"]

                    if event_type.has_key(eventBaseType):
                        num = event_type.get(eventBaseType)
                        data["a"] = str(num) + "-" + eventBaseType + "-" + eventType + "-" + eventExtType
                    else:
                        num = 5
                        data["a"] = str(num) + "-" + eventBaseType + "-" + eventType + "-" + eventExtType
                    log_list.append(data)
                logFormat["log"] = log_list
            SendLOG(logFormat,js_file)
    except:
        logger.error(traceback.print_exc())

def SendLOG(logFormat,filename):
    port = 443
    interval = 30
    method = 'POST'
    url = g_dict['url']
    host = g_dict['host']
    httpsClient = None
    try:
        params = json.dumps(logFormat)
        headers = {"Content-type": "application/json", "Accept": "text/plain"}
        httpsClient = httplib.HTTPSConnection(host, port, timeout = interval)
        httpsClient.request(method, url, params,headers)
     
        response = httpsClient.getresponse()
        if response.status != 200:
            logger.error('Send log fail,please check server .')
            raise 'Send log fail,please check server .'
        else:
            print response.status
            logger.error('Response status not 200,remove file.')
            os.remove(filename)
    except Exception, e:
        logger.error('%s' %traceback.format_exec())
        print e
    finally:
        if httpsClient:
            httpsClient.close()
                 
def main():
    detectData()

if __name__ == "__main__":
    main()
