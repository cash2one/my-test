#!/usr/bin/python
#Filename:pro_restore_main.py
# _*_ coding: utf8 _*_
import sys
import os
import json
import getopt
import subprocess
sys.path.append("/gms/gapi/modules/network")
sys.path.append("/gms/gapi/modules/auth")
sys.path.append("/gms/gapi/modules/flow_def")
sys.path.append("/gms/comm/")
import flow_def_main
import text_xml
import com_to_conf
import sys_auth
import atexit
json_path="/gms/gapi/modules/pro_restore/conf/restore_conf.json"

def add_flow_toxml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_flow_restore")
    if nodes == []:
        a=text_xml.create_node("subflow",{"name":tmp_dict['name'],"udp":tmp_dict['udp'],"tcp":tmp_dict['tcp']}," ","\n\t")
        b=text_xml.create_node("gms_flow_restore",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_flow_restore")
        text_xml.add_child_node(child_nodes,a)
    else:
        c=text_xml.create_node("subflow",{"name":tmp_dict['name'],"udp":tmp_dict['udp'],"tcp":tmp_dict['tcp']}," ","\n\t")
        text_xml.add_child_node(nodes,c)
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0
def mod_flow_toxml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_flow_restore")
    if nodes == []:
        a=text_xml.create_node("subflow",{"name":tmp_dict['name'],"udp":tmp_dict['udp'],"tcp":tmp_dict['tcp']}," ","\n\t")
        b=text_xml.create_node("gms_flow_restore",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_flow_restore")
        text_xml.add_child_node(child_nodes,a)
    else:
        sub_node=text_xml.get_node_by_keyvalue(text_xml.find_nodes(tree,"gms_flow_restore/subflow"),{"name":tmp_dict['name']})
        c=text_xml.change_node_properties(sub_node,{"name":tmp_dict['name'],"udp":tmp_dict['udp'],"tcp":tmp_dict['tcp']},"")
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0
    
def del_flow_toxml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_flow_restore")
    if nodes != []:
        text_xml.del_node_by_tagkeyvalue(nodes,"subflow",{"name":tmp_dict['name']})
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0

def change_xml(tmp_dict={}):
    if tmp_dict['command'] == '1':
        add_flow_toxml(tmp_dict)
    elif tmp_dict['command'] == '2':
        del_flow_toxml(tmp_dict)
    else:
        mod_flow_toxml(tmp_dict)
    return 0;

def read_json_conf():
    global path_dict
    if os.path.exists(json_path) == False:
        sys.exit(13)
    fp= open(json_path,"r")
    path_dict = json.load(fp)
    fp.close()
    return 0
def exec_script(script, argv=[]):
    argv.insert(0, script)
    p = subprocess.Popen(argv, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    sys.stdout.write('%s' % stdout)
    sys.stdout.write('%s' % stderr)
    status = p.returncode
    return status;
def c_flow(argv):
    return exec_script(path_dict['def_flow_path'], argv)
if __name__ == "__main__":
    if flow_def_main.flow_engine() == "tcad":
        tmp_dict={}
        read_json_conf() 
        tmp_argv=sys.argv
        opts,args = getopt.getopt(sys.argv[1:],"c:n:t:u:",[])
        for opt,arg in opts:
            if opt == '-c':
                tmp_dict['command'] = arg
            if opt == '-n':
                tmp_dict['name'] = arg.decode('utf-8')
            if opt == '-t':
                tmp_dict['tcp'] = arg
            if opt == '-u':
                tmp_dict['udp'] = arg 
        argv=sys.argv[1:]
        ret=c_flow(argv)
        change_xml(tmp_dict)
        if com_to_conf.is_manger_device() == '1' and com_to_conf.read_xge_device() == '1':
            import comm_socket_main
            cmd_str="python "
            for i in range(0,len(tmp_argv)):
                cmd_str=cmd_str+tmp_argv[i]+" "
            atexit.register(comm_socket_main.main,cmd_str)
    exit(0)


