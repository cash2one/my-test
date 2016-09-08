#!/usr/bin/python
#Filename:flow_main.py
# _*_ coding: utf8 _*_
import sys
import os
import json
import subprocess
sys.path.append("/gms/gapi/modules/network")
sys.path.append("/gms/gapi/modules/auth")
sys.path.append("/gms/comm/")
import sys_auth
import com_to_conf
import text_xml
import atexit
json_path="/gms/gapi/modules/flow_def/conf/flow_conf.json"

def add_flow_toxml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_def_flow")
    if nodes == []:
        a=text_xml.create_node("add_flow",{"switch":tmp_dict['switch'],"start_ip":tmp_dict['start_ip'],"end_ip":tmp_dict['end_ip']}," ","\n\t")
        b=text_xml.create_node("gms_def_flow",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_def_flow")
        text_xml.add_child_node(child_nodes,a)
    else:
        c=text_xml.create_node("add_flow",{"switch":tmp_dict['switch'],"start_ip":tmp_dict['start_ip'],"end_ip":tmp_dict['end_ip']}," ","\n\t")
        text_xml.add_child_node(nodes,c)
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0
    
def del_flow_toxml(tmp_dict={}):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_def_flow")
    if nodes != []:
        text_xml.del_node_by_tagkeyvalue(nodes,"add_flow",{"start_ip":tmp_dict['start_ip'],"end_ip":tmp_dict['end_ip']})
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0

def flow_engine():
    ret=None
    tree = text_xml.read_xml('/gms/conf/gmsconfig.xml')
    nodes = text_xml.find_nodes(tree,"flow_engine/flow_engine_name")
    if nodes != []:
        ret=nodes[0].text
    return ret
def change_xml(tmp_dict={}):
    if tmp_dict['switch'] == '0':
        add_flow_toxml(tmp_dict)
    else:
        del_flow_toxml(tmp_dict)
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
def make_apt_flow():
    retval=None
    try:
        fp=open(path_dict['apt_flow_path'])
        retval=fp.read()
        fp.close()
    except:
        print "not can't open ",path_dict['apt_flow_path']
    return retval
if __name__ == "__main__":
    read_json_conf()
    if flow_engine() == "tcad":
        tmp_dict={}
        tmp_argv=[]
        text_xml.change_dict(sys.argv,tmp_dict,len(sys.argv))
        for i in range(0,len(sys.argv)):
            tmp_argv.append(sys.argv[i])
        sys.argv[1]="-f"
        sys.argv[3]="-s"
        sys.argv[5]="-e"
        argv=sys.argv[1:]
        ret=c_flow(argv)
        change_xml(tmp_dict)
        if com_to_conf.is_manger_device() == '1' and com_to_conf.read_xge_device() == '1':
            import comm_socket_main
            cmd_str="python "
            for i in range(0,len(tmp_argv)):
                cmd_str=cmd_str+tmp_argv[i]+" "
            atexit.register(comm_socket_main.main,cmd_str)
    else:
        if com_to_conf.is_manger_device() == '1' and com_to_conf.read_xge_device() == '1':
            import comm_socket_main
            cmd_tmp=make_apt_flow()
            cmd_str="echo '"+cmd_tmp.decode('gbk')+"' > "+path_dict['def_flow_path']
            atexit.register(comm_socket_main.main,cmd_str)
    exit(0)


