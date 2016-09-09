#!/usr/bin/env python
#_*_coding:utf-8_*_

import httplib,urllib,urllib2
import json,os,time,traceback
import comm_data
from  js_log import record_log_init

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

#def format(host,url,username,LUID):
#    res_status,res_data = Reg_from_server(host,url,username,LUID)
#    data = json.loads(res_data)
#    data_1 = data["ClientUID"]
#    return data_1 

def dump2reg(dicts):
    if not dicts.get('ClientUID'):
        print 'Recv ClientUID is Null.'
        return
    with open('/gms/conf/js.reg','wb') as fp:
        fp.writelines(dicts.get("ClientUID"))
    fp.close()

def detectData():
    global logger
    logger = record_log_init()
    try:
        url  = 'https://o1.t1.xiseg.com/api/o1/register'
        LUID = '1903338b-7557-4d83-9920-12eb9120d369'
        host = "o1.t1.xiseg.com"
        username = "test"
        path = '/gms/conf/'
        logFormat = {"ComUID":LUID,"ClientUID":"","log":[]}
        event_type = {u'木马': '1',u'僵尸网络': '1',u'网站': '2',u'攻击': '3',u'移动互联网恶意代码': '4'}
    
        if not os.path.isfile(path + 'js.reg'):
            rstatus,contents = Reg_from_server(host,url,username,LUID)
            if rstatus != 200:
                print 'No connection to hosts .'
                logger.error('%s' %traceback.format_exec())
                return
            content = json.loads(contents)
            dump2reg(content) 
            logFormat["ClientUID"] = content["ClientUID"]
        else:
            with open(path + "js.reg","rb") as fp:
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
        print traceback.print_exc()

def SendLOG(logFormat,filename):
    port = 443
    interval = 30
    method = 'POST'
    url ='https://o1.t1.xiseg.com/api/o1/ntl'
    host = "o1.t1.xiseg.com"
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
            os.remove(filename)
    except Exception, e:
        logger.error('%s' %traceback.format_exec())
        print e
    finally:
        if httpsClient:
            httpsClient.close()
                 
#def transform():
#    filename = '/home/linux/Templates/20151204143041.817382_R6KR-B42C-HHVW.ok'
#    with open(filename,'rb') as fp:
#        datas = fp.readlines()
#        print type(datas),len(datas),type(datas[0])
#        datas = json.loads(datas[0])
#    print datas
def main():
    #Reg2server()
    #transform()
    detectData()

if __name__ == "__main__":
    main()
