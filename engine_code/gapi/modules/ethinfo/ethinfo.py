#!/usr/bin/python
#_*_coding:utf8_*_

import parse
import sys
import getopt
import subprocess
import traceback
import os
import re
import ethinfo_data
import json
import ethinfolog

dev_type_file = os.path.abspath('/cfcard/chanct_conf/machine_type.conf') 
config_path = os.path.abspath('/gms/gapi/modules/ethinfo/conf_path.json')

#---------------------------------
# @bref read ifconfig message    |
#---------------------------------
def read_ifconfig(): 
    p = subprocess.Popen('ifconfig -a', stdout=subprocess.PIPE, shell=True) 
    return p.communicate()[0]


#---------------------------------------
# @bref detect eth link status         |
#---------------------------------------
def link_status(data):
    p = subprocess.Popen('ethtool %s|grep detected'%data, stdout=subprocess.PIPE, shell=True) 
    if p.wait() == 0:
        data =  p.communicate()[0]
        if data.split(':')[-1].split()[-1] == 'yes':
            return 'ok'
        elif data.split(':')[-1].split()[-1] == 'no':
            return 'no'
        else:
            print 'invale No such device!!'
    
#--------------------------------------------
# @bref get ip MAC eth message             |
#-------------------------------------------
def get_ipinfo(data): 
    data = (i for i in data.split('\n\n') if i and not i.startswith('lo')) 
    ip_info = [] 
    ifname = re.compile(r'(eth[\d:]*|wlan[\d:]*)') 
    ipaddr = re.compile(r'(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-9]{1,2})(\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[0-9]{1,2})){3}') 
    macaddr = re.compile(r'[A-F0-9a-f:]{17}') 
    for i in data: 
        x = {} 
        if ifname.match(i): 
            device = ifname.match(i).group() 
            x['Adapter'] = device 
        if macaddr.search(i): 
            mac = macaddr.search(i).group() 
            x['MAC'] = mac 
        if ipaddr.search(i): 
            ip = ipaddr.search(i).group() 
            x['IP'] = ip 
        else: 
            x['IP'] = None
        #x['Ethflow'] = get_flow(x['Adapter'])
        #x['Linkstatus'] = get_link_status(x['Adapter'])
        get_flow(x['Adapter'])
        ip_info.append(x) 
    return ip_info 
#-------------------------------------------------------
# @bref oprate open_ethname_list and close_ethname_list |
#-------------------------------------------------------
def open_pag_conf(open_ethnames):
    for num in range(len(open_ethnames)):
        pagconflist = (ethinfo_data.aptpagconf,ethinfo_data.mtxpagconf,ethinfo_data.vdspagconf)
        for conf_file in pagconflist:
            p = subprocess.Popen(["sed -i '/%s/d' %s"%(open_ethnames[num],conf_file)],stdout=subprocess.PIPE,shell = True) 
            status = p.wait()
            if status == 0:
                res =subprocess.Popen(["sed -i '1a dev_name = %s' %s"%(open_ethnames[num],conf_file)],stdout=subprocess.PIPE,shell=True) 
                status = res.wait()
                if status == 0:
                    print 'open [%s]-->[%s] succeed..' %(conf_file,open_ethnames[num])
                    ethinfolog.logger.debug("open [%s]-->[%s] succeed.." %(conf_file,open_ethnames[num]))
            else:
                return status
def close_pag_conf(close_ethnames):
    for num in range(len(close_ethnames)):
        pagconflist = (ethinfo_data.aptpagconf,ethinfo_data.mtxpagconf,ethinfo_data.vdspagconf)
        for conf_file in pagconflist:
            try:
                p = subprocess.Popen(["sed -i '/%s/d' %s"%(close_ethnames[num],conf_file)],stdout=subprocess.PIPE,shell = True) 
                status = p.wait()
                if status == 0:
                    print 'close [%s]-->[%s] succeed .' %(conf_file,close_ethnames[num])
                    ethinfolog.logger.debug("close [%s]-->[%s] succeed.." %(conf_file,close_ethnames[num]))
            except:
                continue
#--------------------------------
# @ modify mtx apt vds pag.conf |
#-------------------------------
def oprate_pag_conf(open_ethname_list,close_ethname_list):
    print 'modify conf and restart engine'
    if open_ethname_list != 'n' and close_ethname_list != 'n':
        open_ethnames = open_ethname_list.split(":")
        close_ethnames = close_ethname_list.split(":")
        open_pag_conf(open_ethnames)
        close_pag_conf(close_ethnames)
        restart_engines()
    elif open_ethname_list == 'n' and close_ethname_list != 'n':
        close_ethnames = close_ethname_list.split(":")
        close_pag_conf(close_ethnames)
        restart_engines()
    elif open_ethname_list != 'n' and close_ethname_list == 'n':
        open_ethnames = open_ethname_list.split(":")
        open_pag_conf(open_ethnames)
        restart_engines()
    else:
        pass

#---------------------------------
# @bref restart engines function |
#--------------------------------
def restart_engines():
    os.system("killall -9 mtx tcad_gms apt dd dm apc")
    os.system("/bin/sh /gms/gapi/modules/auth/gms_logmon.sh >/dev/null 2>&1 &")
    print 'restart engine succeed.'
#-------------------------------------------
def change_eth_status(ethname):
    tree = parse.read_xml(xml_file)
    subnode = parse.find_nodes(tree,"gms_detect/ethdev")
    res_node = parse.get_node_by_keyvalue(subnode, {"name":ethname}) 
    parse.change_node_properties(res_node, {"setcap": "1"})
    parse.write_xml(tree,xml_file)

#--------------------------------------
# @bref oprate xml tree  to flush flow |
#---------------------------------------
def opr_xml(dict):
    tree = parse.read_xml(xml_file)    
    tnode = parse.find_nodes(tree,"gms_detect")
    subnode = parse.find_nodes(tree,"gms_detect/ethdev")
    rnodes = parse.get_node_by_keyvalue(subnode, {"name":dict['Adapter']})#按照属性定位节点，以免下次调用还添加
    if not rnodes:
        newnode = parse.create_node("ethdev",{"name":dict['Adapter'],"mac":dict['MAC'],"setcap":"0"},"","")
        parse.add_child_node(tnode,newnode)
    else: 
        for sub in subnode:
            parse.change_node_properties(rnodes, {"mac":dict['MAC']}) 
    parse.write_xml(tree,xml_file)

def read_json():
    with open(config_path,'r') as fp:
        data = fp.read()
        kwdict = json.loads(data)
        for key, value in kwdict.items():
            setattr(ethinfo_data, key, value)

def read_dev_type(dev_type_file):
    ret = os.path.exists(dev_type_file)
    if ret:
        fp = open(dev_type_file,'r')
        lines = fp.readline().split() 
        fp.close()
        if lines[0] == '1':
            return 1
        elif lines[0] == '2':
            return 2
        else:
            return 1
def get_flow(ethname):
    if read_dev_type(dev_type_file) == '1':
        if ethname == 'lo' or ethname == 'eth0':
            return
    if read_dev_type(dev_type_file) == '2':
        if ethname == 'lo' or ethname == 'eth0' and ethname == 'eth1':
            return
    try:
        p=subprocess.Popen(['ifconfig %s up' %(ethname)],stdout=subprocess.PIPE,shell=True)
        if p.wait() == 0:
            print 'up [%s] succeed . ' % (ethname)
            ethinfolog.logger.debug("up [%s] succeed." %(ethname))
    except:
        print traceback.format_exc()
        ethinfolog.logger.debug(" [%s] ." %(traceback.format_exc()))
        return
def change_open_and_close_ethname_status(open_ethnames,close_ethnames):
    tree = parse.read_xml(xml_file)    
    tnode = parse.find_nodes(tree,"gms_detect")
    subnode = parse.find_nodes(tree,"gms_detect/ethdev")
    for num_open in range(len(open_ethnames)):
        rnodes = parse.get_node_by_keyvalue(subnode, {"name":open_ethnames[num_open]})   #按照属性定位节点
        parse.change_node_properties(rnodes, {"setcap": "1"}) 

    for num_close in range(len(close_ethnames)):  
        rnodes = parse.get_node_by_keyvalue(subnode, {"name":close_ethnames[num_close]})  #按照属性定位节点
        parse.change_node_properties(rnodes, {"setcap": "0"}) 

    parse.write_xml(tree,xml_file)
     
def change_open_ethname_status(open_ethnames):
    tree = parse.read_xml(xml_file)    
    tnode = parse.find_nodes(tree,"gms_detect")
    subnode = parse.find_nodes(tree,"gms_detect/ethdev")
    for num in range(len(open_ethnames)):
        rnodes = parse.get_node_by_keyvalue(subnode, {"name":open_ethnames[num]})   #按照属性定位节点
        parse.change_node_properties(rnodes, {"setcap": "1"}) 
    parse.write_xml(tree,xml_file)
    
def change_close_ethname_status(close_ethnames):
    print 'close_ethname [%s]' %(close_ethnames)
    tree = parse.read_xml(xml_file)    
    tnode = parse.find_nodes(tree,"gms_detect")
    subnode = parse.find_nodes(tree,"gms_detect/ethdev")
    for num in range(len(close_ethnames)):  
        rnodes = parse.get_node_by_keyvalue(subnode, {"name":close_ethnames[num]})  #按照属性定位节点
        parse.change_node_properties(rnodes, {"setcap": "0"}) 
    parse.write_xml(tree,xml_file)
    
def oprate_xml(open_ethname_list,close_ethname_list):
    if open_ethname_list != 'n' and close_ethname_list != 'n':
        open_ethnames = open_ethname_list.split(":")
        close_ethnames = close_ethname_list.split(":")
        change_open_and_close_ethname_status(open_ethnames,close_ethnames)
    elif open_ethname_list != 'n' and close_ethname_list == 'n':
        open_ethnames = open_ethname_list.split(":")
        change_open_ethname_status(open_ethnames)
        
    elif open_ethname_list == 'n' and close_ethname_list != 'n':
        close_ethnames = close_ethname_list.split(":")
        change_close_ethname_status(close_ethnames)  
    else:
        pass
     
#-------------------------------   
# @bref main process entry     |
#------------------------------
xml_file = ''
def main(argv):
    list = []
    ethlist = [] #过滤网卡，以免重新添加新的网卡
    global xml_file
    read_json()
    xml_file = ethinfo_data.gmsconfigxml

    ethinfolog.init_log(ethinfo_data.logpath)


    opts,args = getopt.getopt(sys.argv[1:],"",[]) 
    if args == []:
        list = get_ipinfo(read_ifconfig())
        #print 'list is %s' %(list)
        ethinfolog.logger.debug("list [%s] ." %(list))
        for i in range(len(list)):
            if  not list[i]['IP'] and list[i]['Adapter'] != 'eth0' and read_dev_type(dev_type_file) == 1:
                opr_xml(list[i])
                ethlist.append(list[i]['Adapter'])
            if  not list[i]['IP'] and list[i]['Adapter'] != 'eth0' and list[i]['Adapter'] != 'eth1' and read_dev_type(dev_type_file) == 2:
                opr_xml(list[i])
                ethlist.append(list[i]['Adapter'])   #当网卡被拔掉时，初始化时把本地网卡信息记录到文件中.
        ret_data = ';'.join(ethlist)
        with open(ethinfo_data.eth_record,'w') as fp:
            fp.write(ret_data) 
    else:
        if args[0] != 'n' and args[1] != 'n':     #界面传参数为:  eth0:eth1:eth2 eth3:eth4:eth5
            oprate_xml(args[0],args[1])
            oprate_pag_conf(args[0],args[1])
        elif args[0] != 'n' and args[1] == 'n':    #界面传参数为：eth0:eth1:eth2 n
            oprate_xml(args[0],args[1])
            oprate_pag_conf(args[0],args[1])
        elif args[0] == 'n' and args[1] != 'n':    #界面传参数为：n eth0:eth1:eth2
            oprate_xml(args[0],args[1])
            oprate_pag_conf(args[0],args[1])
        else:
            pass
        
if __name__ == '__main__':
    #exit(main())
    exit(main(sys.argv[1:]))
