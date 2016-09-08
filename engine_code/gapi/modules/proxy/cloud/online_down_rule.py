#!/usr/bin/env python
#coding:utf-8


import cloud_log
import comm_data
import print_color

import os
import re
import sys
import json
import subprocess
import xml.dom.minidom
import time

from comm_common import *
from db import *

from oss.oss_api import *
from oss.osscloudapi import *

'''
单独分离出取规则模块,为了启动定时执行
'''

pre_conf_file = '/gms/gapi/modules/proxy/cloud/conf/comm_pre_conf.json'
Rule_Path = '/gms/mtx/conf/rule.conf'
g_dict = {}    


class InitFun():
    def __init__(self):
        self.comm_cloud_logger = ''
        self.cloud_log_path = ''
        #self.comm_data = ''

    def read_pre_json(self):
        if not os.path.exists(pre_conf_file):
            print '%s no exists .' %(pre_conf_file)
            return
        with open(pre_conf_file) as fp:
            data = fp.read()
            kwdict = json.loads(data)
            #print kwdict
            for key, value in kwdict.items():
                if key == 'iscopytodb':
                    if value.upper() == 'YES':
                        value = True
                    else:
                        value = False
                setattr(comm_data, key, value)

    def log_init(self):
        self.read_pre_json()
        if not os.path.exists(comm_data.cloud_rule_log_path):
            os.makedirs(comm_data.cloud_rule_log_path)
        self.comm_cloud_logger = cloud_log.CloudLogger(path = comm_data.cloud_rule_log_path,level = '',\
                            format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')

class GainRule(InitFun):

    def __init__(self):
        self.log_init()
        self.uuid = ''
        self.mtx_fname_list = ''
        self.OSS_ALIYUNCS_COM = comm_data.OSS_ALIYUNCS_COM
        self.OSS_ID = comm_data.OSS_ID
        self.OSS_KEY = comm_data.OSS_KEY 
        self.OSS_DOWN_BUCKET = comm_data.OSS_DOWN_BUCKET 

    def Gain_UUID(self):
        p = subprocess.Popen(['python /gms/gapi/modules/auth/genkey.pyc'],\
                             stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
        stdout , stderr = p.communicate()
        self.uuid = stdout.strip()

    def printf(self):
        #self.Gain_UUID()
        print self.comm_cloud_logger

    def write_buf_to_file(self,buf, file_path):
        try:
            tmp_file = open(file_path, 'w')
        except IOError, e:
            print e
            self.comm_cloud_logger.debug("[%s]" %e)
            tmp_file.close()
        else:
            if type(buf) == int:
                tmp_file.write(str(buf))
            else:
                tmp_file.write(buf)
            tmp_file.close()

    def get_cloud_query_xml_fname(self,xml_path, prefix_path):
        dom = xml.dom.minidom.parse(xml_path)
        root = dom.documentElement
        content_list = root.getElementsByTagName('Contents')
    
        list = []
        for file_info in content_list:
            keynode = file_info.getElementsByTagName("Key")[0]
            fname = keynode.childNodes[0].nodeValue
            dir_names = fname.split("/")
            try:
                dir_name = dir_names[1]+"/"+dir_names[2]
                if (dir_name == prefix_path):
                    list.append(keynode.childNodes[0].nodeValue)
            except IndexError:
                continue
        #print list
        return list

    def Gain_version(self):
        with open(Rule_Path,'r') as fp:
            datas = fp.read()

        r = re.compile(r'^#[a-zA-Z]+:\d+')
        ret = r.findall(datas)
        return ret[0].split(':')[1]

    def Gain_db_message(self):
        global g_dict
            
        g_dict['utime'] = time.strftime('%Y-%m-%d %H:%M:%S',\
                                 time.localtime(time.time())) 
        g_dict['utime'] = '\'%s\'' %(g_dict['utime'])
        g_dict['utype'] = 2
        g_dict['ucontent_type'] = 1
        #g_dict['old_version'] = self.Gain_version()
        #After download rule.conf
        #g_dict['new_version'] =  
        #g_dict['new_verinfo'] = 
        #g_dict['ustatus'] =  3 or 4
        g_dict['opuser'] = 9
        g_dict['oper_ip'] = 2130706433
    
    def cloud_down_file(self,oss, bucket, flist, path):
        global g_dict
        for fname in flist:
            if not os.path.basename(fname):
                continue
            down_fname = ''
            final_name = path + os.path.basename(fname)
            down_fname = final_name + ".tmp"
            t = time.time()
            try:
                res = oss.get_object_to_file(bucket, fname, down_fname)
                #res.read()
            except Exception, ex:
                #print ex
                continue
            if res.status != 200:
                print_color.print_download(fname, final_name, "FAIL")
                self.comm_cloud_logger.debug("CLOUD[%s] ->FILE[%s]FILE FAIL" %(fname,final_name))
                self.comm_cloud_logger.debug("[%s]" %(res.read()))
                # 从阿里云上获取规则文件失败的情况
                break
            else:
                print_color.print_download(fname, final_name, "SUCC")
                self.comm_cloud_logger.debug("[%s]  CLOUD[%s] ->FILE[%s]SUCC ." %(bucket,fname,final_name))
                self.comm_cloud_logger.debug("-->spend [%f] second(s). file size[%d] ."\
                         %(time.time()-t, os.path.getsize(down_fname)))
                #先将下载完成的文件进行改名
                try:
                    os.rename(down_fname, final_name)
                except Exception, ex:
                    #print ex
                    #self.comm_cloud_logger.debug("[%s]" %ex)
                    continue
                #  下载成功 删除云端对应的文件
                try:
                    res = oss.delete_object(bucket, fname)
                    res.read()
                except Exception, ex:
                    print ex
                    self.comm_cloud_logger.debug("[%s]" %ex)
                    continue
                if (res.status != 204):
                    print_color.print_delete(fname, "FAIL")
                    self.comm_cloud_logger.debug("CLOUD[%s] FIAL  " %(fname))
                    print res.read()
                    self.comm_cloud_logger.warn("[%s]" %(res.read()))
                else:
                    print_color.print_delete(fname, "SUCC")
                    self.comm_cloud_logger.debug("CLOUD[%s] SUCC " %(fname))

    def store_postgres(self):
        print "=== Start Insert Knowledge into Postgresql . ==="
        self.comm_cloud_logger.debug("=== Start Insert Knowledge into Postgresql . ===")
        import subprocess
        path = comm_data.comm_mtx_kwonledge_down_path
        list = map(lambda file:os.path.join(path,file) , \
                   [file for file in os.listdir(path) \
                    if os.path.isfile(os.path.join(path,file))])        

        if not list:
            print "%s is Null" %(path)
            self.comm_cloud_logger.debug("%s is Null" %(path))
            return False
         
        print list
        for file in list:
            if not os.path.getsize(file):
                continue
            print file 
            try:
                p = subprocess.Popen(['python /gms/bin/excel_main.py %s' %(file)],stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
                p.wait()
            except Exception as ex:
                print ex
                self.comm_cloud_logger.warn('%s' %(ex))
                continue

            if p.returncode != 0:
                print ' Knowledge insert into Postgresql Fail .'
                self.comm_cloud_logger.debug('Knowledge insert into Postgresql Fail .') 
                continue

            print 'Knowledge insert into Postgresql Succeed .'
            self.comm_cloud_logger.debug('Knowledge insert into Postgresql Succeed .')
        return True
        
        
    def mtx_down_busi(self,flag):
        global g_dict
        self.Gain_UUID()
        #print self.uuid
        if flag == 'rule' and comm_data.if_rule_flag == 'no':
            print "----------------  MTX CONF FLAG {close} BUSINESS -------------"
            comm_cloud_logger.debug("------ MTX CONF FLAG {close} BUSINESS -------------")
            return
        if flag == 'rule':
            print "-------------------- Begin MTX CONF online DOWN BUSINESS -------------"
            self.comm_cloud_logger.debug("-------------------- Begin MTX CONF online DOWN BUSINESS -------------")
        if flag == 'knowledge':
            print "-------------------- Begin KNOWLEDGE online DOWN BUSINESS -------------"
            self.comm_cloud_logger.debug("-------------------- Begin KNOWLEDGE online DOWN BUSINESS -------------")

        oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
        bucket = comm_data.OSS_DOWN_BUCKET
        tmp_file = '/tmp/mtx_rule_down.xml'

        #while True:
        # 1. 读取云库中目前都有哪些文件
        
        try:
            flag_online = False
            res = oss.get_bucket(bucket, prefix=self.uuid)
            #print res.read()
        except Exception, ex:
            oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))
            #continue
        else:
            if res.status != 200:
                print "list %s bucket FAIL." %(bucket)
                self.comm_cloud_logger.debug("list %s bucket FAIL." %(bucket))
                print res.status, res.read()
                self.comm_cloud_logger.debug("res.status=[%s], res.read()=[%s]" %(res.status,res.read()))
                flag_online = True
                #continue

        # 2. 将得到的xml文件存在本地
        try :
            self.write_buf_to_file(res.read(), tmp_file)
        except Exception, ex:
            print ex
            self.comm_cloud_logger.warn("[%s]"%ex)
            #continue

        # --> mtx规则库 下载事件
        if flag == 'rule':
            tmpPath = comm_data.MTX_RULE_FILE
        if flag == 'knowledge':
            tmpPath = comm_data.MTX_KNOW_FILE
        try:
            self.mtx_fname_list = self.get_cloud_query_xml_fname(tmp_file, tmpPath)
        except Exception,ex:
            os.remove(tmp_file)
            print ex
            self.comm_cloud_logger.warn("%s ,delete %s" %(ex , tmp_file))
            #continue
        if flag == 'rule':
            self.Gain_db_message()
            # 下载前获取规则文件信息 old_version 字段
            g_dict['old_version'] = self.Gain_version()
            # 记录文件修改时间
            t_time_before = os.stat(Rule_Path).st_mtime
            print 'check old_version : %s \n check before_time : %s\n' %(g_dict['old_version'],t_time_before)
            self.comm_cloud_logger.debug('check old_version : %s\ncheck before_time : %s\n'%(g_dict['old_version'],\
                                        t_time_before))

            localFile = comm_data.comm_mtx_rule_down_path
            creat_if_dir_notexist(localFile) 
            self.cloud_down_file(oss, bucket, self.mtx_fname_list, localFile)
            time.sleep(15)
            # 下载后获取规则文件信息 new_version 字段
            g_dict['new_version'] = self.Gain_version()
            t_time_after = os.stat(Rule_Path).st_mtime

            print 'check new_version : %s\ncheck after_time : %s\n' %(g_dict['new_version'],t_time_after)
            self.comm_cloud_logger.debug('check new_version : %s\ncheck after_time : %s\n'%(g_dict['new_version'],\
                                        t_time_after))
            if g_dict['old_version'] != g_dict['new_version']:
                g_dict['new_verinfo'] = '定时升级成功'
                g_dict['new_verinfo'] = '%s' %(g_dict['new_verinfo'])
                g_dict['ustatus'] = 3
            else:   # 版本相同
                if t_time_before != t_time_after:   #时间不同要求人为下发
                    g_dict['new_verinfo'] = '定时升级成功'
                    g_dict['new_verinfo'] = '%s' %(g_dict['new_verinfo'])
                    g_dict['ustatus'] = 3
                else:
                    if flag_online: #通过标记为判断是否失败
                        g_dict['new_version'] = '\'-\''
                        g_dict['new_verinfo'] = '定时升级失败'
                        g_dict['new_verinfo'] = '%s' %(g_dict['new_verinfo'])
                        g_dict['ustatus'] = 4
                    else:
                        g_dict = {}
            try:
                update_rule_table(g_dict)
            except Exception,ex:
                self.comm_cloud_logger.debug(' %s ' %(ex))
                
            print 'insert into db succeed .'
            self.comm_cloud_logger.debug('insert into db succeed .')
        if flag == 'knowledge':
            localFile = comm_data.comm_mtx_kwonledge_down_path
            creat_if_dir_notexist(localFile) 
            self.cloud_down_file(oss, bucket, self.mtx_fname_list, localFile)

        try:
            os.remove(tmp_file)
        except Exception,ex:
            pass

if __name__ == '__main__':    
    reload(sys)
    sys.setdefaultencoding('utf-8')
    obj = GainRule()
    obj.mtx_down_busi('rule')
    obj.mtx_down_busi('knowledge')
    obj.store_postgres()
    


