#!/usr/bin/python 
#-*-coding:utf8-*-

import sys
import os
import getopt
import parse 
import enginelog
import subprocess
import engine_data
import json
import engine_oper
import atexit
import time
sys.path.append("/gms/comm/")
sys.path.append("/gms/gapi/modules/auth/")
import sys_auth
import com_to_conf
config_path = os.path.abspath('/gms/gapi/modules/engine/conf_path.json')
devid = ''
#-------------------------------------------------
# @bref 解析xml文件，把根据id ，添加新的节点及属性|
#-------------------------------------------------
def par_add_xml(dict):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    nodes = parse.find_nodes(tree,"gms_engine/apt")
    newnode = parse.create_node("subapt",{"id":dict['id'],"str":dict['str'],"iplist":dict['iplist'],"ipstyle":dict['ipstyle'],"creattime":dict['creattime'],"author":dict['author'],"authorip":dict['authorip'],"desc":dict['desc'],"domain":dict['domain']},"")  
    parse.add_child_node(nodes,newnode)
    parse.write_xml(tree,engine_data.gmsconfigxml)    

#------------------------------------------------
# @bref 解析xml文件，把根据名称 ，把相关节点属性更新|
#------------------------------------------------
def par_change_xml(dict):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    subnodes = parse.find_nodes(tree,"gms_engine/apt/subapt")
    getval = parse.get_node_by_keyvalue(subnodes,{"str":dict['modify']})
    parse.change_node_properties(getval,{"id":dict['id'],"str":dict['str'],'iplist':dict['iplist'],"ipstyle":dict['ipstyle'],"creattime":dict['creattime'],"author":dict['author'],"authorip":dict['authorip'],"desc":dict['desc'],"domain":dict['domain']})
    parse.write_xml(tree,engine_data.gmsconfigxml)    
#------------------------------------------------
# @bref 解析xml文件，把根据id ，把相关信息删除  |
#------------------------------------------------
def par_delete_xml(dict):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    subnodes = parse.find_nodes(tree,"gms_engine/apt")
    target_del_node = parse.del_node_by_tagkeyvalue(subnodes, "subapt", {"str" : dict['str']})
    parse.write_xml(tree,engine_data.gmsconfigxml)    

#------------------------------------------------------------------
# @bref 解析xml文件，域名节点修改，没有文件操作,把相关节点属性更新|
#------------------------------------------------------------------
def par_domain_change_xml(dict):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    subnodes = parse.find_nodes(tree,"gms_engine/apt/subapt")
    getval = parse.get_node_by_keyvalue(subnodes,{"str":dict['modify']})
    parse.change_node_properties(getval,{"id":dict['id'],"str":dict['str'],'iplist':dict['iplist'],"ipstyle":dict['ipstyle'],"creattime":dict['creattime'],"author":dict['author'],"authorip":dict['authorip'],"desc":dict['desc']})
    parse.write_xml(tree,engine_data.gmsconfigxml)    
#------------------------------------------------
# @bref 修改apt配置文件                          |
#-----------------------------------------------
def modify_apt_config(dict):
    print 'modify apt config ......'
    ret = delete_apt_config(dict)
    if ret == 20:
        write_config(engine_data.innerip,dict)
    elif ret == 22:
        write_config(engine_data.webserver,dict)
    elif ret == 24:
        write_config(engine_data.mailserver,dict)
    else:
        print 'delete parage fail ,to write fail.'
        #log
#------------------------------------------------
# @bref 删除apt相关信息文件                     |
#-----------------------------------------------
def delete_apt_config(dict):
    print 'delete user_config....'
    if dict['ipstyle'] == 'inner':
        p = subprocess.Popen(["sed -i '/%s/d' %s"%(dict['modify'],engine_data.innerip)],stdout=subprocess.PIPE,shell=True)
        status = p.wait()
        if status == 0:
            return 20
    elif dict['ipstyle'] == 'webserver':
        p = subprocess.Popen(["sed -i '/%s/d' %s"%(dict['modify'],engine_data.webserver)],stdout=subprocess.PIPE,shell=True)
        status = p.wait()
        if status == 0:
            return 22
    elif dict['ipstyle'] == 'mailserver':
        p = subprocess.Popen(["sed -i '/%s/d' %s"%(dict['modify'],engine_data.mailserver)],stdout=subprocess.PIPE,shell=True)
        status = p.wait()
        if status == 0:
            print 'delete succeed..'
            return 24
#------------------------------------------------
# @bref 删除apt相关信息文件                     |
#-----------------------------------------------
def delete_apt_config1(dict):
    file_list = (engine_data.innerip,engine_data.webserver,engine_data.mailserver)
    #print 'delete config....',dict['str']
    for file in file_list:
        if not os.path.exists(file):
            continue
        with open(file,'r') as fp:
            data = fp.readlines()
            for d in data:
                d = d.split()  #按照规则文件的格式匹配配置文件内容
                try:
                    if d[2] == dict['str']:
                        p = subprocess.Popen(["sed -i '/%s/d' %s"%(d[2],file)],stdout=subprocess.PIPE,shell = True)
                        status = p.wait()
                        if status == 0:
                            print 'conf delete succeed....'
                            enginelog.logger.debug("engine config[%s] is deleted succeed..."%(dict['str']))
                        else:
                            print 'conf delete fail....'
                            enginelog.logger.debug("engine config[%s] is deleted fail..."%(dict['str']))
                except Exception,ex:
                    print ex
                else:
                     print 'domain delete xml .......'
#---------------------------------------------------
# @bref 配置内网ip，按照一定的格式把ip写到配置文件中|
#---------------------------------------------------
def write_config(config,dict):
    #print '写配置文件...'
    with open(config,'a+') as fp:
        data = dict['iplist']
        data = data.strip()
        #print data,'*********'
        #fdata = data.split('-')
        #print fdata,'***********'
        fp.write(data)
        #fp.write('\t')
        #fp.write(fdata[1])
        #fp.write('\t')
        #fp.write(dict['str'])
        fp.write('\n')
    #print '配置完成.'
     
#--------------------------------------
# @bref add_apt user_config          |
#-------------------------------------- 
def add_apt_config(dict):
    print 'add user_config...'
    if dict['ipstyle'] == 'inner':
        write_config(engine_data.innerip,dict)
    elif dict['ipstyle'] == 'webserver':
        write_config(engine_data.webserver,dict)
    elif dict['ipstyle'] == 'mailserver':
        write_config(engine_data.mailserver,dict)
#--------------------------------------
# @bref operate apt_xml status        |
#-------------------------------------
def apt_xml(args):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    tnode = parse.find_nodes(tree,"gms_engine")
    nodes = parse.find_nodes(tree,"gms_engine/apt")
    if not nodes:
        newnode = parse.create_node("apt",{"status":args},"")  
        parse.add_child_node(tnode,newnode)
    else:
        parse.change_node_properties(nodes,{"status": args})
        parse.write_xml(tree,engine_data.gmsconfigxml)    
    print 'write apt status [%s] into [%s] ..... '%(args,engine_data.gmsconfigxml)
    enginelog.logger.debug("write apt status [%s] into %s ....."%(args,engine_data.gmsconfigxml))

#-----------------------------------------
#  上网行为审计配置                       |
#-----------------------------------------
audit_dict = {}
audit_list = []
def forward_xml():
    global audit_dict
    global audit_list
    def oper_config_audit_config(ip_list):
        if not os.path.exists(engine_data.iplist):
            with open(engine_data.iplist,'w') as fp:
                pass
        with open(engine_data.iplist,'w') as ff:
            ff.write(ip_list)
            ff.write('\n')
    try:
        root = parse.read_xml(engine_data.gmsconfigxml)
        nodes = parse.find_nodes(root,'gms_engine/apt/subapt')
        for node in nodes:
            if node.attrib['ipstyle'] == 'inner':
                audit_dict['iplist'] = node.attrib['iplist']
                audit_list.append(audit_dict['iplist'])            
        #enginelog.logger.debug("***** DEBUG : %s" %(audit_list))
        
        #bak_list = [audit for audit in audit_list if '-' in audit]
        bak_list = [audit for audit in audit_list]
        print '---------------',bak_list
            #enginelog.logger.debug("**** DEBUG '-' before  %s" %(audit))
        #for audit in bak_list:
        #    audit = audit.strip()
            #sip , dip = audit.split('-')
            #if sip == dip:
            #audit_list.append(audit)
            #    audit_list.remove(audit)

        enginelog.logger.debug("**** DEBUG result audit: %s" %(audit_list))

        #tmp_data = '\n'.join(audit_list)
        tmp_data = '\n'.join(bak_list)
        oper_config_audit_config(tmp_data)
        os.system('/gms/bin/flow_config')
    except Exception,ex:
        enginelog.logger.debug("%s" %(ex))

#-------------------------
# @bref 操作xml信息      |
#-------------------------
def message_xml(dict): 
    tree = parse.read_xml(engine_data.gmsconfigxml)
    subnodes = parse.find_nodes(tree,"gms_engine/apt/subapt")
    if not subnodes:
        par_add_xml(dict)
        forward_xml()
    else: #若 -M 后为'' 则表示重新创建一个新的节点；若 -M '777' 则表示查找id为777的子节点，并修改为666的属性
        if not dict['modify'] and not dict['delete']:
            #print 'start exce add_apt_config....'
            par_add_xml(dict)
            forward_xml()
            add_apt_config(dict)        
        elif dict['modify']:
            #print 'star exce modify_apt_config....'
            par_change_xml(dict)
            forward_xml()
            modify_apt_config(dict)        
        elif dict['delete']=='y' and dict['str']:
            #print 'start exce delete_apt_config....'
            par_delete_xml(dict)
            forward_xml()
            delete_apt_config1(dict) 
def read_json():
    with open(config_path,'r') as fp:
        data = fp.read()
        kwdict = json.loads(data)
        for key, value in kwdict.items():
            setattr(engine_data, key, value)
#-------------------------------
#@bref main()  entry           |
#-------------------------------
def main(argv):
    read_json() 
    enginelog.init_log(engine_data.logpath)
    opts,args = getopt.getopt(sys.argv[1:],"N:d:n:c:l:s:t:a:p:M:D:",["desc=","domain="])
    dict = {'enginename':'','command':'','id':'','str':'','ipstyle':'','creattime':'','author':'',\
            'authorip':'','iplist':'','modify':'','delete':'','desc':'','domain':''}
    #print '==== opts ====' , opts
    #print '==== args ====' , args
    for opt,arg in opts:
        if opt == '-N':
            dict['enginename'] = arg
        if opt == '-c':
            dict['command'] = arg
        if opt == '-d':
            dict['id'] = arg
        if opt == '-n':
            dict['str'] = arg.decode('utf-8') 
        if opt == '-s':
            dict['ipstyle'] = arg.decode('utf-8')
        if opt == '-t':
            dict['creattime'] = arg
        if opt == '-a':
            dict['author'] = arg.decode('utf-8')
        if opt == '-p':
            dict['authorip'] = arg
        if opt == '-l':
            dict['iplist'] = arg
        if opt == '-M':
            dict['modify'] = arg
        if opt == '-D':
            dict['delete'] = arg
        if opt == '--desc':
            dict['desc'] = arg.decode('utf-8')
            if dict['desc'] == 'empty':
                dict['desc'] = ''
        if opt == '--domain':
            dict['domain'] = arg.decode('utf-8')

    if dict['enginename'] == 'apt' and dict['command'] == 'stop':
        print 'close  apt....'
        engine_oper.apt_stop()
    elif dict['enginename'] == 'apt' and dict['command'] == 'start':
        if not dict['str'] and not dict['id'] and not dict['delete'] and not dict['modify'] and \
            not dict['ipstyle'] and not dict['creattime'] and not dict['author'] and not dict['authorip']\
            and not dict['iplist'] and not dict['desc'] and not dict['domain']:

            print 'only start apt for UI click tag...'
            engine_oper.apt_start()
        else:
            print 'start apt and operation xmltree..'
            #engine_oper.__apt_start()
            message_xml(dict)
            apt_xml('0')
    elif dict['enginename'] == 'mtx' and dict['command'] == 'stop':
        print 'close mtx ...'
        engine_oper.mtx_stop()
    elif dict['enginename'] == 'mtx' and dict['command'] == 'start':
        print 'start mtx...'
        engine_oper.mtx_start()
    elif dict['enginename'] == 'vds' and dict['command'] == 'stop':
        print 'close vds...'
        engine_oper.vds_stop()
    elif dict['enginename'] == 'vds' and dict['command'] == 'start':
        print 'start vds...'
        engine_oper.vds_start()

if __name__ == '__main__':
    if  com_to_conf.is_manger_device() == '1' and com_to_conf.read_xge_device() == '1':
        import comm_socket_main
        cmd_str="python "
        for i in range(0,len(sys.argv)):
            cmd_str=cmd_str+sys.argv[i]+" "
        atexit.register(comm_socket_main.main,cmd_str)
    exit(main(sys.argv[1:]))

