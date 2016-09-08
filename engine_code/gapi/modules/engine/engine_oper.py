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
import time

devid = ''
         
#-------------offline start--------------
#def offline_start():
#    ret = subprocess.Popen(['killall -9 offline'],stdout=subprocess.PIPE,shell=True)
#    if ret.wait() == 0:
#        if os.path.exists("/gms/apt/offline/"):
#            ret_status = subprocess.Popen(['cd /gms/apt/offline;./offline >/dev/null 2>&1 &'],stdout=subprocess.PIPE,shell = True)
#            if ret_status.wait() == 0:
#                print '/gms/apt/offline/offline start succeed .'
#                enginelog.logger.debug(" /gms/apt/offline/offline start succeed . ")
#                return True
#            else:
#                return False
#        else:
#            print 'dir [/gms/apt/offline/] not exists .'
#            enginelog.logger.debug(" dir [/gms/apt/offline/] not exists . " )
#            return False
#    else:
#        print ret.communicate()[1]
#        enginelog.logger.debug(" %s " % (ret.communicate()[1]))
#        return False
#     
#---------------------------------
# @bref read regfile to get devid|
#---------------------------------
def read_reg(filename):
    with open(filename,'r') as fp:
        data = fp.readlines()
        fdata = data[0].split('|')
        devid = fdata[7]
    return devid
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
    #print 'write apt status [%s] into [%s] ..... '%(args,engine_data.gmsconfigxml)
    enginelog.logger.debug("write apt status [%s] into %s ....."%(args,engine_data.gmsconfigxml))
#--------------------------------------
# @bref operate mtx_xml status        |
#-------------------------------------
def mtx_xml(args):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    tnode = parse.find_nodes(tree,"gms_engine")
    nodes = parse.find_nodes(tree,"gms_engine/mtx")
    if not nodes:
        newnode = parse.create_node("mtx",{"status":args},"")  
        parse.add_child_node(tnode,newnode)
    else:
        parse.change_node_properties(nodes,{"status":args})
    parse.write_xml(tree,engine_data.gmsconfigxml)   
    #print 'write mtx status [%s] into [%s] .....'%(args,engine_data.gmsconfigxml)
    enginelog.logger.debug("write mtx status [%s] into [%s] ....."%(args,engine_data.gmsconfigxml))
#--------------------------------------
# @bref operate vds_xml status        |
#-------------------------------------
def vds_xml(args):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    tnode = parse.find_nodes(tree,"gms_engine")
    nodes = parse.find_nodes(tree,"gms_engine/vds")
    if not nodes:
        newnode = parse.create_node("vds",{"dd":args,"dm":args,"apc":args,"status":args},"")  
        parse.add_child_node(tnode,newnode)
    else:
        parse.change_node_properties(nodes,{"dd": args,"dm":args,"apc":args,"status":args})
    parse.write_xml(tree,engine_data.gmsconfigxml)    
    print 'vds  status [%s] into [%s] .'%(args,engine_data.gmsconfigxml)
    enginelog.logger.debug("write vds status [%s] into [%s] ."%(args,engine_data.gmsconfigxml))
#-----------------------------------------------------
# @bref detect engine status to distribute apt vds priv |
#-----------------------------------------------------
def detect_engine_status(engineName):
    tree = parse.read_xml(engine_data.gmsconfigxml)
    nodes = parse.find_nodes(tree,"gms_engine/%s"%(engineName))
    for node in nodes:
        data = node.attrib.get("status")
    return data

def pag_conf(colu):
    fp=open("/gms/mtx/pag.conf","r")
    while True:
        buf=fp.readline()
        if buf == "": 
            break
        if buf.find(colu) == 0:
            valu=buf
    ret=valu[valu.index("=")+1:]
    fp.close()
    return ret.strip().rstrip("\n")

#-------------------------------
# @bref mtx_start public section| 
#-------------------------------
def mtx_public():
    global devid
    timestamp = '%Y%m%d-%H%M%S-'

    stream_num = pag_conf('stream_num')

    p = subprocess.Popen(["cd /gms/mtx ;./mtx -d special --devid %s -t %s --flow_num %s -w /data/tmpdata/engine/mtd/%s%s.mtd -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  3  --back_trace --emergency -I  -o 10240 --no_wtl >/dev/null 2>&1 &"%(devid,stream_num,stream_num,timestamp,devid)],stdout=subprocess.PIPE,shell=True)
    if p.wait() == 0:
        mtx_xml("1")
        print 'start  mtx succeed .'
        enginelog.logger.debug("start  mtx succeed .")
    else:
        mtx_xml("0")
        print 'start mtx FAIL .'
        enginelog.logger.warn("start mtx FAIL .")
        return 23
#--------------------------------
# @bref no white list  public   |
#-------------------------------
def mtx_white():
    global devid
    timestamp = '%Y%m%d-%H%M%S-'
    
    stream_num = pag_conf('stream_num')

    p = subprocess.Popen(["cd /gms/mtx ;./mtx -d special --devid %s -t %s --flow_num %s -w /data/tmpdata/engine/mtd/%s%s.mtd -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  3  --back_trace --emergency -I  -o 10240 >/dev/null 2>&1 &"%(devid,stream_num,stream_num,timestamp,devid)],stdout=subprocess.PIPE,shell=True)
    if p.wait() == 0:
        mtx_xml("1")
        print 'start  mtx succeed .'
        enginelog.logger.debug(" start  mtx succeed .")
    else:
        mtx_xml("0")
        print 'start mtx fail .'
        enginelog.logger.warn(" start mtx fail . ")
        return 23

def mtx_exist_operate():
    proc = subprocess.Popen(['killall -9 mtx'],stdout=subprocess.PIPE,shell=True)
    if proc.wait() == 0:
        if os.path.exists("/gms/mtx/") and os.path.isfile(engine_data.mtx_white_path):
            print ' in mtx start ................'
            mtx_white()
        else:
            print ' --------------- common start !!!!!!!!!'
            mtx_public()

def mtx_no_exist_operate():
    if os.path.exists("/gms/mtx/") and os.path.isfile(engine_data.mtx_white_path):
        print ' in mtx start ................'
        mtx_white()
    else:
        print ' --------------- common start !!!!!!!!!'
        mtx_public()

# ----mtx start --------------
def mtx_start():
    def reg_file():
        global devid
        if os.path.isfile(engine_data.regfile_path):
            devid = read_reg(engine_data.regfile_path)
        else:
            print '/gms/conf/reg.dat not exists .'
            enginelog.logger.debug('/gms/conf/reg.dat not exists .')

    reg_file()
    pid = subprocess.Popen(['pidof mtx'],stdout=subprocess.PIPE,shell=True)    
    if pid.communicate()[0].split():
        mtx_exist_operate()         
    else:
        mtx_no_exist_operate()

def apt_exist_oprate():
    proc = subprocess.Popen(['killall -9 apt'],stdout=subprocess.PIPE,shell=True)
    if proc.wait() == 0:
        if os.path.exists("/gms/apt/online/"):
            subproc = subprocess.Popen(['cd /gms/apt/online;./apt >/dev/null 2>&1 &'],stdout=subprocess.PIPE,shell=True)
            if subproc.wait() == 0:
            #    status  = offline_start()
            #    if status:
                apt_xml("1")
                #print 'start apt  succeed .'
                enginelog.logger.debug("start apt succeed .")
            else:
                apt_xml("0")
                #print 'start apt fail .'
                enginelog.logger.debug("start apt fail .")
                return 24
        else:    # tcad start API
            apt_xml("1")
    else:
        # apt  no exist .it will operate it  .
        apt_xml("1")
        enginelog.logger.debug("write 1 in xml apt statu .")
    
def apt_no_exist_oprate():
    if os.path.exists("/gms/apt/online/"):
        subproc = subprocess.Popen(['cd /gms/apt/online;./apt >/dev/null 2>&1 &'],stdout=subprocess.PIPE,shell=True)
        if subproc.wait() == 0:
            #status  = offline_start()   #offline集成在online里了
            #if status:
            apt_xml("1")
            print 'start apt  succeed .'
            enginelog.logger.debug("start apt  succeed .")
        else:
            apt_xml("0")
            print 'start apt  fail .'
            enginelog.logger.debug("start apt fail .")
            return 24
    else:
        # apt or tcad no exist .it will operate it  .
        apt_xml("1")

#-------apt start-----------------------
def apt_start():
    pids = subprocess.Popen(['pidof apt'],stdout=subprocess.PIPE,shell=True)    
    if pids.communicate()[0].split:
        apt_exist_oprate()
    else:
        apt_no_exist_oprate()

#---------vds start---------------------
def vds_exist_oprate():
    proc = subprocess.Popen(['killall -9 dd dm apc'],stdout=subprocess.PIPE,shell=True)
    if proc.wait() == 0:
        if os.path.exists("/vds/"):
            subproc = subprocess.Popen(['/bin/sh /vds/script/monitor.sh >/dev/null 2>&1 &'],stdout=subprocess.PIPE,shell=True)
            if subproc.wait() == 0:
                vds_xml("1")
                print ' start vds succeed . '
                enginelog.logger.debug(" start vds succeed . ")
            else:
                vds_xml("0")
                print ' start vds fail . '
                enginelog.logger.debug(" start vds fail . ")
                return 25
        else:
            print 'dir [/vds/ dd dm apc] not exists .'
            enginelog.logger.debug("dir [/vds/ : dd dm apc] not exists . " )
    else:
        enginelog.logger.debug(" killall -9 dd dm apc  fail . ")

def vds_no_exist_oprate():
    if os.path.exists("/vds/"):
        subproc = subprocess.Popen(['/bin/sh /vds/script/monitor.sh >/dev/null 2>&1 &'],stdout=subprocess.PIPE,shell=True)
        if subproc.wait() == 0:
            vds_xml("1")
            print ' start vds succeed . '
            enginelog.logger.debug(" start vds succeed . ")
        else:
            vds_xml("0")
            print ' start vds fail . '
            enginelog.logger.debug(" start vds fail . ")
            return 25
    else:
        print 'dir [/vds/ dd dm apc] not exists .'
        enginelog.logger.debug("dir [/vds/ : dd dm apc] not exists . " )

def vds_start():
    pids = subprocess.Popen(['pidof dd dm apc'],stdout=subprocess.PIPE,shell=True)
    if pids.communicate()[0].split():
        vds_exist_oprate()    
    else:
        vds_no_exist_oprate()
#--------------mtx stop---------
def mtx_stop():
    proc = subprocess.Popen(['killall -9 mtx'],stdout = subprocess.PIPE,stderr = subprocess.PIPE,shell = True)
    if  proc.wait() == 0 :
        print 'close mtx succeed .'
        enginelog.logger.debug(" close mtx succeed . ")
    else:
        print 'close mtx fail .'

    try:
        mtx_xml("0")
    except Exception,ex:
        print ex
        enginelog.logger.debug(" %s " % ( ex ))
    
#--------apt stop -----------------------         
def apt_stop():
    proc = subprocess.Popen(['killall -9 apt offline'],stdout = subprocess.PIPE,stderr = subprocess.PIPE,shell = True)
    if  proc.wait() == 0 :
        print ' close apt  succeed .'
        enginelog.logger.debug("close apt  succeed . ")
    else:
        enginelog.logger.debug("close apt  .")

    try:
        apt_xml("0")
    except Exception,ex:
        print ex
        enginelog.logger.debug("[%s]"%(ex))
        
#--------vds stop -----------------------         
def vds_stop():
    proc = subprocess.Popen(['killall -9 dd apc dm'],stdout = subprocess.PIPE,stderr = subprocess.PIPE,shell = True)
    if  proc.wait() == 0 :
        print 'close vds succeed .'
        enginelog.logger.debug(" close vds succeed . ")
    else:
        print '%s' %(proc.communicate()[1].strip())

    try:
        vds_xml("0")
    except Exception,ex:
        print ex
        enginelog.logger.info(" %s " % ( ex ) )

