#!/usr/bin/python
#Filename:xml_att.py
import os

import sys
sys.path.append("/gms/gapi/modules/network/")
sys.path.append("/gms/gapi/modules/auth/")
sys.path.append("/gms/comm/")
import sys_auth
import text_xml
import com_to_conf
import string
import comm_socket_main
import atexit
import time
time_dict={
           "1":"cp /usr/share/zoneinfo/Etc/GMT-12 /etc/localtime",
           "2":"cp /usr/share/zoneinfo/US/Samoa /etc/localtime",
           "3":"cp /usr/share/zoneinfo/US/Hawaii /etc/localtime",
           "4":"cp /usr/share/zoneinfo/US/Alaska /etc/localtime",
           "5":"cp /usr/share/zoneinfo/US/Pacific /etc/localtime",
           "6":"cp /usr/share/zoneinfo/US/Mountain /etc/localtime",
           "7":"cp /usr/share/zoneinfo/US/Central /etc/localtime",
           "8":"cp /usr/share/zoneinfo/US/East-Indiana /etc/localtime",
           "9":"cp /usr/share/zoneinfo/America/Caracas /etc/localtime",
           "10":"cp /usr/share/zoneinfo/America/Blanc-Sablon /etc/localtime",
           "11":"cp /usr/share/zoneinfo/Canada/Newfoundland /etc/localtime",
           "12":"cp /usr/share/zoneinfo/America/Montevideo /etc/localtime",
           "13":"cp /usr/share/zoneinfo/America/Montevideo /etc/localtime",
           "14":"cp /usr/share/zoneinfo/America/Scoresbysund /etc/localtime",
           "15":"cp /usr/share/zoneinfo/America/Danmarkshavn /etc/localtime",
           "16":"cp /usr/share/zoneinfo/Europe/Berlin /etc/localtime",
           "17":"cp /usr/share/zoneinfo/Asia/Amman /etc/localtime",
           "18":"cp /usr/share/zoneinfo/Asia/Baghdad /etc/localtime",
           "19":"cp /usr/share/zoneinfo/Asia/Tehran /etc/localtime",
           "20":"cp /usr/share/zoneinfo/Asia/Yerevan /etc/localtime",
           "21":"cp /usr/share/zoneinfo/Asia/Kabul /etc/localtime",
           "22":"cp /usr/share/zoneinfo/Asia/Karachi /etc/localtime",
           "23":"cp /usr/share/zoneinfo/Asia/Karachi /etc/localtime",
           "24":"cp /usr/share/zoneinfo/Asia/Kathmandu /etc/localtime",
           "25":"cp /usr/share/zoneinfo/Asia/Dhaka /etc/localtime",
           "26":"cp /usr/share/zoneinfo/Asia/Rangoon /etc/localtime",
           "27":"cp /usr/share/zoneinfo/Asia/Bangkok /etc/localtime",
           "28":"cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime",
           "29":"cp /usr/share/zoneinfo/Asia/Tokyo /etc/localtime",
           "30":"cp /usr/share/zoneinfo/Pacific/Guam /etc/localtime",
           "31":"cp /usr/share/zoneinfo/Pacific/Noumea /etc/localtime",
           "32":"cp /usr/share/zoneinfo/Pacific/Wallis /etc/localtime",
           "33":"cp /usr/share/zoneinfo/Pacific/Auckland /etc/localtime",
           "34":"cp /usr/share/zoneinfo/Pacific/Kiritimati /etc/localtime"}
def xml_make(tag_str,dict_flag,att_name,tzoneval):
    if os.path.exists("/gms/conf/gmsconfig.xml") == False:
        return 6
    tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_time")
    if nodes == []:
        a=text_xml.create_node(tag_str,{att_name:dict_flag,"num":tzoneval}," ","\n\t")
        b=text_xml.create_node("gms_time",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_time")
        text_xml.add_child_node(child_nodes,a)
    else:
        net_node=text_xml.find_nodes(tree,"gms_time/"+ tag_str)
        if net_node == []:
            a=text_xml.create_node(tag_str,{att_name:dict_flag,"num":tzoneval}," ","\n\t")
            text_xml.add_child_node(nodes,a)
        else:    
            text_xml.change_node_properties(net_node,{att_name:dict_flag,"num":tzoneval},"")
    text_xml.write_xml(tree, "/gms/conf/gmsconfig.xml") 
    return 0	
    
def xml_automake(tag_str,dict_flag,att_name,port_num,switch):
    if os.path.exists("/gms/conf/gmsconfig.xml") == False:
        return 6
    tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_time")
    if nodes == []:
        a=text_xml.create_node(tag_str,{att_name:dict_flag,"port":port_num,"switch":switch}," ","\n\t")
        b=text_xml.create_node("gms_time",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_time")
        text_xml.add_child_node(child_nodes,a)
    else:
        net_node=text_xml.find_nodes(tree,"gms_time/"+ tag_str) 
        if net_node == []:
            a=text_xml.create_node(tag_str,{att_name:dict_flag,"port":port_num,"switch":switch},"")
            text_xml.add_child_node(nodes,a)
        else:    
            text_xml.change_node_properties(net_node,{att_name:dict_flag,"port":port_num,"switch":switch},"")
    text_xml.write_xml(tree, "/gms/conf/gmsconfig.xml") 
    return 0	
def read_timezone():
    tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
    nodes = text_xml.find_nodes(tree,"gms_time/manual_time")
    return nodes[0].attrib['num']
def kill_engine():
    os.system("killall -9 mtx tcad_gms dd apc dm java;rm -rf /var/run/gapi.session")
if __name__ == "__main__":
    #print len(sys.argv)
    tmp_dict={}
    text_xml.change_dict(sys.argv,tmp_dict,len(sys.argv))
    #print tmp_dict,len(sys.argv)
    if len(sys.argv) < 6 :
        str1=' '
        tmp=tmp_dict['time']	
        tmp_flag=tmp.find(str1)
        if tmp_flag == -1:
            cmd_time2="date -s @"+tmp
        else:
            cmd_time2="date -s '"+tmp+"'"
        if tmp_dict['timezone'] == "0":
            tmp_dict['timezone']="27"
            cmd_time1=time_dict["27"]
        else:
            cmd_time1=time_dict[tmp_dict['timezone']]
        cmd_time=cmd_time1+";"+cmd_time2
        if os.system(cmd_time) != 0:
            exit(5)
        else:
            ret=xml_make("manual_time",cmd_time1,"timezone",tmp_dict['timezone'])
            if sys_auth.read_auth_flag() == '0' and com_to_conf.read_xge_device() == '1':
                cmd_str="python "
                for i in range(0,len(sys.argv)):
                    cmd_str=cmd_str+"'"+sys.argv[i]+"' "
                print cmd_str
                atexit.register(comm_socket_main.main,cmd_str)
    else:
        if tmp_dict['switch'] != '0':
            cmd_time="ntpdate "+tmp_dict['realm']+" "+tmp_dict['port']
            if os.system(cmd_time) != 0:
                exit(8)
        ret=xml_automake("auto_time",tmp_dict['realm'],"server",tmp_dict['port'],tmp_dict['switch'])
        if sys_auth.read_auth_flag() == '0' and com_to_conf.read_xge_device() == '1':
            cmd_str="python "+sys.argv[0]+" time "+"'"+time.strftime("%Y-%m-%d %H:%M:%S")+"'"+" timezone "+read_timezone() 
            atexit.register(comm_socket_main.main,cmd_str)
    os.system("hwclock -w")
    kill_engine()
    exit(ret)

