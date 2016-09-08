#!/usr/bin/env  python
#-*-coding:utf8-*-
import os
import sys
import getopt
import time
import parse
import traceback
import atexit
import wtl_log
import subprocess


sys.path.append("/gms/comm/")
sys.path.append("/gms/gapi/modules/auth/")
import sys_auth
import com_to_conf

dict = {}
xml_file = '/gms/conf/gmsconfig.xml'

mtx_conf = '/gms/mtx/conf/wtl.conf'
apt_conf = '/gms/apt/user_config/ip_white.conf'
vds_conf = '/vds/conf/u_device.cnf'

def par_add_xml(dict):
    tree = parse.read_xml(xml_file)
    nodes = parse.find_nodes(tree,"gms_wtl")
    if nodes == []:
        root = tree.getroot()
        b=parse.create_node("gms_wtl",{},"\n\t")
        root.append(b)
    newnode = parse.create_node("subwtl",{"name":dict['name'],"src_ip":dict['src_ip'],"end_ip":dict['end_ip'],"dec":dict['dec'],"oper":dict['oper']},"")  
    parse.add_child_node(nodes,newnode)
    parse.write_xml(tree,xml_file)    

def par_change_xml(dict):
    tree = parse.read_xml(xml_file)
    subnodes = parse.find_nodes(tree,"gms_wtl/subwtl")
    getval = parse.get_node_by_keyvalue(subnodes,{"name":dict['name']})
    parse.change_node_properties(getval,{"name":dict['name'],'src_ip':dict['src_ip'],"end_ip":dict['end_ip'],"dec":dict['dec'],"oper":dict['oper']})
    parse.write_xml(tree,xml_file)

def par_delete_xml(dict):
    tree = parse.read_xml(xml_file)
    subnodes = parse.find_nodes(tree,"gms_wtl")
    target_del_node = parse.del_node_by_tagkeyvalue(subnodes, "subwtl", {"name" : dict['name']})
    parse.write_xml(tree,xml_file)

list = []
def forward_xml_add(xmlfile):
    global list
    tree = parse.read_xml(xmlfile)
    nodes = parse.find_nodes(tree, "gms_wtl/subwtl") 
    for children in nodes:
        src_ip , end_ip ,name = children.attrib['src_ip'] , children.attrib['end_ip'] ,children.attrib['name']
        dict['src_ip'] ,dict['end_ip'],dict['name'] = src_ip , end_ip ,name
        tup = (dict['src_ip'] ,dict['end_ip'],dict['name'])
        list.append(tup)
    produce_conf(list)

def stdump(conf_file , str):
    with open(conf_file,'a+') as fp:
        fp.write( str ) 


def produce_conf(list):
    os.system("mkdir -p "+os.path.split(apt_conf)[0])
    print 'produce_conf .......',list
    tupi = (mtx_conf , apt_conf ,vds_conf)
    for i in tupi:
        with open(i,'w') as fd:
            pass
    for src , dst ,name in list:
        wtl_str = '(ip.src>=%s && ip.src<=%s)||(ip.dst>=%s && ip.dst<=%s)\n' %(src ,dst ,src, dst)
        apt_str = "%s\t%s\t%s\n" % ( src , dst ,name)
        vds_str = '{\n\t\t"device":[\n\t\t\t\t{"dtype":0,"stype":8,"ip":"%s"}\n\n\t\t"device":[\n\t\t\t\t{"dtype":0,"stype":8,"ip":"%s"}\n}' % (src,dst)    

        tupl = ((mtx_conf , wtl_str), (apt_conf , apt_str ), (vds_conf ,vds_str))
        for elem , str in tupl:
            stdump(elem , str.encode('gb2312') ) 

def restart_engine():
    os.system('killall -9  mtx apt dd dm apc')
    os.system("/bin/sh /gms/gapi/modules/auth/gms_logmon.sh >/dev/null 2>&1 &")

def main(argv):
    global dict
    dict = {'name':'','src_ip':'','end_ip':'','dec':'','oper':''}
    opts , args = getopt.getopt(sys.argv[1:],"n:s:e:d:c:",[])
    for opt , arg in opts:
        if opt == '-n':
            dict['name'] = arg 
        if opt == '-s':
            dict['src_ip'] = arg 
        if opt == '-e':
            dict['end_ip'] = arg 
        if opt == '-d':
            dict['dec'] = arg 
        if opt == '-c':
            dict['oper'] = arg 

    if dict['oper'] == '1':
        par_add_xml(dict)
        forward_xml_add(xml_file)
        restart_engine()
    if dict['oper'] == '2':
        par_delete_xml(dict)#delete
        forward_xml_add(xml_file) 
        restart_engine()
    if dict['oper'] == '3':
        par_change_xml(dict)#change
        forward_xml_add(xml_file)
        restart_engine()

if __name__ == '__main__':
    logger = wtl_log.record_log_init()
    try:
        if sys_auth.read_auth_flag() == '0' and com_to_conf.read_xge_device() == '1':
            import comm_socket_main
            cmd_str="python "
            for i in range(0,len(sys.argv)):
                cmd_str=cmd_str+sys.argv[i]+" "
            print cmd_str
            atexit.register(comm_socket_main.main,cmd_str)
    except Exception,ex:
        sys.stdout.write("%s" % traceback.format_exc()) 
        logger.warn("%s" % traceback.format_exc())
    exit(main(sys.argv[1:]))

