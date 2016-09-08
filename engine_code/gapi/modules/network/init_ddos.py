#!/usr/bin/python
#Filename:xml_att.py
# _*_ coding: utf8 _*_
import text_xml
import sys
import os
import socket
import struct
import subprocess
from xml.etree import ElementTree
from xml.etree.ElementTree import ElementTree,Element
import shlex
import shutil


gms_path="/gms/conf/gmsconfig.xml"
gms_path_bak="/gms/conf/gmsconfigbak.xml"
path_dict={}
def iptolong(ipstr):
    return struct.unpack("!I", socket.inet_aton(ipstr))[0]
def read_ddos_xml():
    tree = text_xml.read_xml(gms_path)
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"ddos_set/ddos_switch")
    return nodes[0].attrib['open'],nodes[0].attrib['self_learn']

def read_in_threshold_xml():
    tree = text_xml.read_xml(gms_path)
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"ddos_set/in_threshold")
    return nodes[0].attrib['flow'],nodes[0].attrib['tcp_new_conn'],nodes[0].attrib['tcp_live_conn']

def read_out_threshold_xml():
    tree = text_xml.read_xml(gms_path)
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"ddos_set/out_threshold")
    return nodes[0].attrib['flow'],nodes[0].attrib['tcp_live_conn'],nodes[0].attrib['tcp_new_conn']
def check_xmlfile():
    tree = ElementTree()
    try:
        tree.parse(gms_path)
        return 1
    except:
        return 0
def exec_script(script, argv=[]):
    argv.insert(0, script)

    ext_map = {'.py': 'python', '.pyc': 'python'}
    ext_name = os.path.splitext(script)[-1]
    if ext_map.has_key(ext_name):
        argv.insert(0, ext_map[ext_name])
    print argv
    p = subprocess.Popen(argv, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    sys.stdout.write('%s' % stdout)
    sys.stdout.write('%s' % stderr)
    status = p.returncode
    if status == 0:
        shutil.copyfile(gms_path,gms_path_bak)
    if status != 0:
        if 0 == check_xmlfile():
            shutil.copyfile(gms_conf_bak,gms_conf)
    return status

def func_do():
    ddos_sw,self_learn=read_ddos_xml()
    if ddos_sw == '1':
        if self_learn == '1':
            str='-o 1 -a null -d null -l 1 --in_flow=null --in_new_conn=null --in_live_conn=null --out_flow=null --out_live_conn=null --out_new_conn=null'
        else:
            str=' -o 1 -a null -d null -l 0 --in_flow=%s --in_new_conn=%s --in_live_conn=%s ' % read_in_threshold_xml()+ '--out_flow=%s --out_live_conn=%s --out_new_conn=%s' % read_out_threshold_xml()
            
    else:
        str='-o 0 -a null -d null -l null --in_flow=null --in_new_conn=null --in_live_conn=null --out_flow=null --out_live_conn=null --out_new_conn=null'
    argv=[]
    argv=shlex.split(str)
    script = os.path.join('/gms/gapi', 'modules', 'ddos', 'ddos_main.pyc')
    return exec_script(script,argv)
if __name__ == "__main__":
    func_do() 
    exit(0)
