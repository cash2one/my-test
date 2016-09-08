#!/usr/bin/python
#Filename:pro_restore_main.py
# _*_ coding: utf8 _*_
import sys
import os
import json
import getopt
import subprocess
sys.path.append("/gms/gapi/modules/network")
import text_xml
json_path="/gms/gapi/modules/device/conf/set_sw240.json"

def mod_monitor_toxml(tmp_dict=[]):
    tree = text_xml.read_xml(path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"neighbour")
    if nodes != []:
        sub_node=text_xml.get_node_by_keyvalue(text_xml.find_nodes(tree,"neighbour/device"),{"ip":tmp_dict[0]})
        c=text_xml.change_node_properties(sub_node,{"status":tmp_dict[1]},"")
    text_xml.write_xml(tree, path_dict['gmsconfig'])  
    return 0

def change_xmltree(tmp_dict=[]):
    arg_buf=['192.168.10.2','1']
    if tmp_dict[0] != "n":
        ret_buf=tmp_dict[0].split(":")
        for i in range(0,len(ret_buf)):
            arg_buf[0]=ret_buf[i]
            arg_buf[1]="1"
            mod_monitor_toxml(arg_buf)
    if tmp_dict[1] != "n":
        ret_buf=tmp_dict[1].split(":")
        for i in range(0,len(ret_buf)):
            arg_buf[0]=ret_buf[i]
            arg_buf[1]="0"
            mod_monitor_toxml(arg_buf)
    return 0

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
def shell_cmd(argv):
    return exec_script(path_dict['shellcmd'], argv)
if __name__ == "__main__":
    read_json_conf()
    if len(sys.argv) < 2:
        exit(1)
    if len(sys.argv[1]) < 1:
        exit(1)   
    argv=sys.argv[1:]
    ret=shell_cmd(argv)
    change_xmltree(sys.argv[1:])
    exit(0)


