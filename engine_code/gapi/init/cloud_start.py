#!/usr/bin/python
import os
import sys
import subprocess
sys.path.append("/gms/gapi/modules/network")
import text_xml
gms_conf_xml="/gms/conf/gmsconfig.xml"
tmp_dict={}
gapi_path="/gms/gapi"
limit_speed=os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_comm_daemon.pyc')
limit_proxy=os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_proxy.pyc')
def read_gms_xml():
    tree = text_xml.read_xml(gms_conf_xml)
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"comm_stat")
    tmp_dict['comm_link']=nodes[0].attrib['link']
    nodes = text_xml.find_nodes(tree,"gms_device/attr")
    tmp_dict['root']=nodes[0].attrib['root']
    tmp_dict['cloud']=nodes[0].attrib['cloudcon']
    proxy_nodes = text_xml.find_nodes(tree,"gms_proxy/pro")
    tmp_dict['protol']=proxy_nodes[0].attrib['flag']
    protol_info=text_xml.find_nodes(tree,"gms_proxy/"+tmp_dict['protol'].lower())
    tmp_dict['ip']=protol_info[0].attrib['ip']
    tmp_dict['passwd']=protol_info[0].attrib['passwd']
    tmp_dict['port']=protol_info[0].attrib['port']
    tmp_dict['username']=protol_info[0].attrib['username']
    #<gms_limit downlimit="3455" uplimit="1233" />
    limit_nodes=text_xml.find_nodes(tree,"gms_limit")
    if limit_nodes[0].attrib['downlimit'] == '0':
        tmp_dict['downlimit']='-1'
    else:
        tmp_dict['downlimit']=limit_nodes[0].attrib['downlimit']
    if limit_nodes[0].attrib['uplimit'] == '0':
        tmp_dict['uplimit']='-1'
    else:
        tmp_dict['uplimit']=limit_nodes[0].attrib['uplimit']
    network_nodes=text_xml.find_nodes(tree,"gms_network/dns")
    tmp_dict["proxy"]=network_nodes[0].attrib['proxy']
    return 0

def main():
    comm_main_cmd="ps x|grep -v 'grep'|grep cloud_comm_main.py|awk '{printf $1}'"
    res=os.popen(comm_main_cmd)
    ret=res.read()
    cloud_cmd=""
    if ret == "":
        read_gms_xml()
        print limit_speed,"test"
        if (tmp_dict['root'] == "1" or tmp_dict['comm_link'] == "0") :
            if (tmp_dict['cloud'] == "1" or tmp_dict['comm_link'] == "0"):
                if tmp_dict['proxy'] == '1':
                    cloud_cmd="python "+limit_proxy+" -m "+tmp_dict['protol']+" -i "+tmp_dict['ip']+" -p "+tmp_dict['port']+" -u "+tmp_dict['username']+" -w "+tmp_dict['passwd']+" -h "+tmp_dict['uplimit']+" -l "+tmp_dict['downlimit']
                else:
                    cloud_cmd="python "+limit_speed+" -h "+tmp_dict['uplimit']+" -l "+tmp_dict['downlimit']
            else:
                cloud_cmd="python "+limit_speed+" -h "+tmp_dict['uplimit']+" -l "+tmp_dict['downlimit']
            print cloud_cmd
            #cloud_cmd="df -h"
    return subprocess.call(cloud_cmd,shell=True)
  
if __name__ == "__main__":
    exit(main())
