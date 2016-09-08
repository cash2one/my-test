#!/usr/bin/python
# _*_ coding: utf8 _*_ 
import os
import sys
sys.path.append("/gms/gapi/modules/network")
sys.path.append("/gms/gapi/init")
import text_xml
import cloud_start
import auth_log
import time
import sys_auth
import signal
import com_to_conf
auth_line=True
def get_fork_stat(forkname):
	'''''判断进程状态
		forkname: 进程名
		进程存在返回进程号，不存在返回False'''
	comm_main_cmd="ps x|grep -v 'grep'|grep "+forkname+"|awk '{print $1}'|awk 'NF>0'|cut -f 1 -d r"
	res=os.popen(comm_main_cmd)
	ret=res.read()
	#print ret
	# 进程没有开启
	if ret == "":
		return False
	# 进程已经开启
	else:
		return ret
def read_ifconfig():
	pf=os.popen("ethtool eth3 |grep 'baseT/Full'|grep -v 'Half\|Advertised'|cut -f 2 -d ':'|tail -n 1|cut -f 1 -d 'b'")
	buf=pf.readline()
	#flag=True
	#while flag:
	#	buf=pf.readline()
	#	if buf == "":
	#		break
	#	if buf.find("Speed") != -1:
	#		bas=buf.strip()
	#return bas[bas.index("Speed")+len("Speed")+1:].split("M")[0].strip()
	if com_to_conf.read_xge_device() == "1":
		num=4*5*int(buf.strip())
	else:
		num=buf.strip()
	return str(num)
def str_time_to(src_time):
	#print src_time
	if len(src_time) > 1:
		return time.strftime("%Y-%m-%d %H:%M:%S",time.strptime(src_time,"%Y%m%d%H%M%S"))
	else:
		return src_time
def surput_time(src_time):
	#print src_time
	if len(src_time) > 1 :
		file_time=time.mktime(time.strptime(src_time,"%Y%m%d%H%M%S"))
		now_time=sys_auth.strom_time()
		strval=str((file_time-now_time)/(3600*24))
		return strval.split(".")[0]
	elif src_time == '0':
		return src_time
	else:
		return "3650"

def xml_logmon(filename,auth_infor):
    speed=read_ifconfig()
    tree = text_xml.read_xml(auth_log.path_dict['gmsconfig'])
    root = tree.getroot()
    nodes = text_xml.find_nodes(tree,"gms_auth")
    #surtime=surput_time(auth_infor[auth_infor.index("mtx")+1])
    if nodes == []:
        #auth_line=False
        a=text_xml.create_node("auth_file",{"name":filename}," ","\n\t")
        c=text_xml.create_node("auth_time",{"mtx":str_time_to(auth_infor[auth_infor.index("mtx")+1]),"apt":str_time_to(auth_infor[auth_infor.index("apt")+1]),"vds":str_time_to(auth_infor[auth_infor.index("vds")+1])}," ","\n\t")
        d=text_xml.create_node("sur_time",{"speed":speed}," ","\n\t")
        b=text_xml.create_node("gms_auth",{},"\n\t","\n")
        root.append(b)
        child_nodes=text_xml.find_nodes(tree,"gms_auth")
        text_xml.add_child_node(child_nodes,a)
        text_xml.add_child_node(child_nodes,c)
        text_xml.add_child_node(child_nodes,d)
    else:
        net_node=text_xml.find_nodes(tree,"gms_auth/auth_file") 
        text_xml.change_node_properties(net_node,{"name":filename},"")
        dns_node=text_xml.find_nodes(tree,"gms_auth/auth_time") 
        text_xml.change_node_properties(dns_node,{"mtx":str_time_to(auth_infor[auth_infor.index("mtx")+1]),"apt":str_time_to(auth_infor[auth_infor.index("apt")+1]),"vds":str_time_to(auth_infor[auth_infor.index("vds")+1])},"")
        sur_node=text_xml.find_nodes(tree,"gms_auth/sur_time") 
        text_xml.change_node_properties(sur_node,{"speed":speed},"")
    text_xml.write_xml(tree, auth_log.path_dict['gmsconfig'])  
def read_gmsconfig():
	tree = text_xml.read_xml(auth_log.path_dict['gmsconfig'])
	mtx_infor=text_xml.find_nodes(tree,"gms_engine/mtx")
	for d in mtx_infor:
		mtx_st=d.attrib["status"]
	apt_infor=text_xml.find_nodes(tree,"gms_engine/apt")
	for d in apt_infor:
		apt_st=d.attrib["status"]
	vds_infor=text_xml.find_nodes(tree,"gms_engine/vds")
	for d in vds_infor:
		vds_st=d.attrib["dd"]
	return mtx_st,apt_st,vds_st

def read_xml_conf():
	#tree = text_xml.read_xml("/gms/conf/gmsconfig.xml")
	tree = text_xml.read_xml(auth_log.path_dict['gmsconfig'])
	device_infor=text_xml.find_nodes(tree,"gms_device/attr")
	for d in device_infor:
		root_node=d.attrib["root"]
		cloud_node=d.attrib["cloudcon"]
		manager_node=d.attrib["manager"]
		monitor_node=d.attrib["monitor"]
	print root_node,cloud_node
	return root_node,cloud_node,manager_node,monitor_node
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
def reg_conf():
	fp= open("/gms/conf/reg.dat","r")
	buf=fp.read()
	fp.close()
	return buf.split("|")[7]
def start_mtx():
	stream_num=pag_conf("stream_num")
	reg_id=reg_conf()
	if reg_id != "\N":
		if os.path.exists("/gms/mtx/conf/wtl.conf"):
			mtx_cmd="cd "+auth_log.path_dict['mtx_path']+";"+"./mtx  -d special --devid "+reg_id+" -t "+stream_num+" --flow_num "+stream_num+" -w "+"/data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-"+reg_id+".mtd"+" -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  4  --back_trace --emergency -I  -o 10240  1>/dev/null 2>&1 &"
		else:
			mtx_cmd="cd "+auth_log.path_dict['mtx_path']+";"+"./mtx  -d special --devid "+reg_id+" -t "+stream_num+" --flow_num "+stream_num+" -w "+"/data/tmpdata/engine/mtd/%Y%m%d-%H%M%S-"+reg_id+".mtd"+" -G 30 --dynpool_size 200M --ypool_size 400M --mtdver  4  --back_trace --emergency -I  -o 10240  1>/dev/null 2>&1 &"
	return os.system(mtx_cmd)


def py_logmon(filename,auth_infor,auth_flag):
	auth_log.logger.debug("[sys_logmon 78] into sys_logmon")
	crond_num=get_fork_stat("crond")
	auth_log.logger.debug("[sys_logmon 78] crond stat %s" % crond_num)
	comm_num=get_fork_stat("comm_main.py")
	auth_log.logger.debug("[sys_logmon 78] comm_main stat %s" % comm_num)
	cloud_num=get_fork_stat("cloud_comm_main.py")
	auth_log.logger.debug("[sys_logmon 78] cloud_comm_main.py stat %s" % cloud_num)
	store_num=get_fork_stat("store_proc")
	auth_log.logger.debug("[sys_logmon 78] store_proc stat %s" % store_num)
	flow_num=get_fork_stat("flow_proc")
	auth_log.logger.debug("[sys_logmon 78] flow_proc stat %s" % flow_num)
	merge_num=get_fork_stat("merge_proc")
	auth_log.logger.debug("[sys_logmon 78] merge_proc stat %s" % merge_num)
	guard_num=get_fork_stat("sys_guard")
	auth_log.logger.debug("[sys_logmon 78] sys_guard stat %s" % guard_num)
	mtx_num=get_fork_stat("mtx")
	auth_log.logger.debug("[sys_logmon 78] mtx stat %s" % mtx_num)
	apt_num=get_fork_stat("apt")
	auth_log.logger.debug("[sys_logmon 78] apt stat %s" % apt_num)
	online_num=get_fork_stat("offline")
	auth_log.logger.debug("[sys_logmon 78] online stat %s" % online_num)
	vds_num=get_fork_stat("vds/dd")
	auth_log.logger.debug("[sys_logmon 78]  vds stat %s" % vds_num)
	event_num=get_fork_stat("event_export.py")
	auth_log.logger.debug("[sys_logmon 78] event_export.py start %s" % event_num)
	url_num=get_fork_stat("url_detect.py")
	auth_log.logger.debug("[sys_logmon 78] url_detect.py stat %s" % url_num)
	trans_num=get_fork_stat("remote_trans.py")
	auth_log.logger.debug("[sys_logmon 78] remote_trans.py stat %s" % trans_num)
	#读取gmsconfig.xml文件获取root，cloud，manager，monitor的状态
	root_node,cloud_node,manager_node,monitor_node=read_xml_conf()
	#读取gmsconfig.xml文件获取三大引擎是否被关闭
	mtx_st,apt_st,vds_st=read_gmsconfig()
	#print root_node	
	#读取license解密后的内容，获得各个模块的授权状态
	store_switch=auth_infor[auth_infor.index("store_proc")+1]
	comm_switch=auth_infor[auth_infor.index("comm")+1]
	store_switch=auth_infor[auth_infor.index("store_proc")+1]
	flow_switch=auth_infor[auth_infor.index("flow_proc")+1]
	merge_switch=auth_infor[auth_infor.index("merge_proc")+1]
	guard_switch=auth_infor[auth_infor.index("sys_guard")+1]
	if auth_flag == True:
		#此为授权成功后做的操作,将最新的license文件、引擎授权有效日期和剩余时间写到xml中
		xml_logmon(filename,auth_infor)
		#通过对比各个模块的授权状态，将需要开启的启动，已经开启的保持原有状态
		#if crond_num == False:
		#	os.popen("crond -L /dev/null") 
		if (store_switch == "1") and (store_num == False):
			store_path,store_file=os.path.split(auth_log.path_dict["store_path"]) 
			start_store=os.popen("cd "+store_path+";nohup ./"+store_file+" 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 113]start store_proc %s" % start_store.read()) 
		if (flow_switch == "1") and (flow_num == False):
			flow_path,flow_file=os.path.split(auth_log.path_dict["flow_path"]) 
			start_flow=os.popen("cd "+flow_path+";nohup ./"+flow_file+" 1>/dev/null 2>&1 &")
			#start_flow=os.popen("cd /gms/storage;nohup ./flow_proc 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 115]start flow_proc %s" % start_flow.read()) 
		if (merge_switch == "1") and (merge_num == False):
			merge_path,merge_file=os.path.split(auth_log.path_dict["merge_path"]) 
			st_merge=os.popen("cd "+merge_path+";nohup ./"+merge_file+" 1>/dev/null 2>&1 &")
			#st_merge=os.popen("cd /gms/storage;nohup ./merge_proc 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 118]start merge_proc %s" % st_merge.read()) 
		if (guard_switch == "1") and (guard_num == False):
			guard_path,guard_file=os.path.split(auth_log.path_dict["sys_guard"]) 
			st_guard=os.popen("cd "+guard_path+";nohup ./"+guard_file+" 1>/dev/null 2>&1 &")
			#st_guard=os.popen("cd /gms/guard;nohup ./sys_guard 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 118]start sys_guard %s" % st_guard.read()) 
		if (comm_num == False) and (comm_switch == "1"):
			com_path,com_file=os.path.split(auth_log.path_dict["comm_main"]) 
			start_comm=os.popen("cd "+com_path+";nohup python "+com_file+" 1>/dev/null 2>&1 &")
			#start_comm=os.popen("cd /gms/comm;nohup python comm_main.py 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 118]start comm_main.pyc %s" % start_comm.read())
		if (trans_num == False) and (comm_switch == "1"):
			trans_path,trans_file=os.path.split(auth_log.path_dict["remote_trans"]) 
			start_trans=os.popen("cd "+trans_path+";nohup python "+trans_file+" 1>/dev/null 2>&1 &")
			#start_trans=os.popen("cd /gms/comm/remote/;nohup python remote_trans.py 1>/dev/null 2>&1 &")
			auth_log.logger.info(" [sys_logmon 118]start remote_trans.py %s" % start_trans.read())
		if root_node == "1":
			e_cmd="/nmsmw/python-2.6.8/bin/python /nmsmw/daemon/mtx_export/event_export.py 1>/dev/null 2>&1 &"
			if (cloud_num == False) and (comm_switch == "1"):
				auth_log.logger.info(" [*****Is root ,but not run cloud_comm_main.py start it now  ******************")
				ret=cloud_start.main()
				auth_log.logger.info(" [***********start cloud_comm_main.py  ***************ret[%s]***" % ret)
			if cloud_node != "1":
				auth_log.logger.info(" [*****Is root 1** cloudcn 0**need start [%s] ***" % e_cmd)
				if event_num == False:
					ret=os.system(e_cmd)
					auth_log.logger.info(" [***** start [%s] ***" % e_cmd)
				else:
					auth_log.logger.info(" [*****  [%s] is runing ,Don't restart***" % e_cmd)
			else:
				if event_num != False:
					auth_log.logger.info(" [***** Is root 1**cloud 1** [%s]  killed**" % e_cmd)
					os.system("kill -9 %s" % event_num)
			if monitor_node == "1":
				if (os.path.exists("/gms/conf/reg.dat")) and (reg_conf() != "\n"):
					auth_log.logger.debug(" [sys_logmon 131] this root and monitor had reged already..")
					if mtx_num == False:
						if mtx_st == "1":
							ret=start_mtx()
							if ret == 0:
								auth_log.logger.info(" [sys_logmon 135]start mtx sucess")
							else:
								auth_log.logger.warn(" [sys_logmon 136]start mtx fail")
						else:
							auth_log.logger.info(" *****************mtx status is %s in gmsconfig.xml *********" % mtx_st)
					if online_num == False:
						if apt_st == "1":
							offline_path,offline_file=os.path.split(auth_log.path_dict["offline_path"]) 
							ret=os.system("cd "+offline_path+";nohup ./"+offline_file+" 1>/dev/null 2>&1 &")
							if ret == 0:
								auth_log.logger.info(" [sys_logmon 142]start offline sucess")
							else:
								auth_log.logger.warn(" [sys_logmon 144]start offline fail")
						else:
							auth_log.logger.info(" *****************onffline status is %s in gmsconfig.xml *********" % apt_st)
					if apt_num == False:
						if apt_st == "1":

							apt_path,apt_file=os.path.split(auth_log.path_dict["apt_path"]) 
							ret=os.system("cd "+apt_path+";nohup ./"+apt_file+" 1>/dev/null 2>&1 &")
							if ret == 0:
								auth_log.logger.info(" [sys_logmon 142]start apt sucess")
							else:
								auth_log.logger.warn(" [sys_logmon 144]start apt fail")
						else:
							auth_log.logger.info(" *****************apt status is %s in gmsconfig.xml *********" % apt_st)
					if vds_num == False:
						if vds_st == "1":
							vds_path,vds_file=os.path.split(auth_log.path_dict["vds_path"]) 
							ret=os.system("cd "+vds_path+";nohup "+vds_file+" 1>/dev/null 2>&1 &")
							if ret == 0:
								auth_log.logger.info(" [sys_logmon 148]start vds sucess")
							else:
								auth_log.logger.warn(" [sys_logmon 150]start vds fail")
						else:
							auth_log.logger.info(" *****************vds status is %s in gmsconfig.xml *********" % vds_st)
					if url_num == False:
						url_path,url_file=os.path.split(auth_log.path_dict["url_detect"]) 
						ret=os.system("cd "+url_path+";nohup python "+url_file+"  1>/dev/null 2>&1 &")
						if ret == 0:
							auth_log.logger.info(" [sys_logmon 148]start url_detect.py sucess")
						else:
							auth_log.logger.warn(" [sys_logmon 150]start url_detect.py fail")
				else:
					if mtx_num != False:
						os.system("kill -9 %s" % mtx_num)
						auth_log.logger.info(" [**************not reg try kill mtx **************************")
					if online_num != False:
						os.system("kill -9 %s" % online_num)
						auth_log.logger.info(" [*************not reg try kill  offline **********************")
					if apt_num != False:
						os.system("kill -9 %s" % apt_num)
						auth_log.logger.info(" [*************not reg try kill apt ***************************")
					if vds_num != False:
						os.system("kill -9 %s" % vds_num)
						os.system("killall apc")
						os.system("killall dm")
						auth_log.logger.info(" [*************not reg try kill  vds **************************")
					if url_num != False:
						os.system("kill -9 %s" % url_num)
						auth_log.logger.info(" [*************not reg try kill  url_detect.py ****************")

					auth_log.logger.warn(" [sys_logmon 150] This is root and monitor , but don't reg....")
			else:
				if url_num != False:
					os.system("kill -9 %s" % url_num)
				if online_num != False:
					os.system("kill -9 %s" % online_num)
				mtx_flag=os.popen("killall -9 mtx")
				auth_log.logger.warn("[sys_logmon 138] mtx have authed due date killing %s" % mtx_flag.read())
				apt_flag=os.popen("killall -9 apt")
				auth_log.logger.warn("[sys_logmon 140] apt have authed due date killing %s" % apt_flag.read())
				dd_flag=os.popen("killall -9 dd")
				auth_log.logger.warn("[sys_logmon 142] vds ->dd have authed due date killing %s" % dd_flag.read())
				apc_flag=os.popen("killall -9 apc")
				auth_log.logger.warn("[sys_logmon 144] vds ->apc have authed due date killing %s" % apc_flag.read())
				dm_flag=os.popen("killall -9 dm")
				auth_log.logger.warn("[sys_logmon 146] vds ->dm  have authed due date killing %s" % dm_flag.read())
		elif monitor_node == "1":
			if cloud_num != False:
				os.system("kill -9 %s" % cloud_num)
			if event_num != False:
				os.system("kill -9 %s" % event_num)
			if os.path.exists("/gms/conf/reg.dat") and (reg_conf != "\N"):
				auth_log.logger.debug(" [sys_logmon 155] this isn't root ,but is monitor had reged already..")
				if mtx_num == False:
					if mtx_st == "1":
						ret=start_mtx()
						if ret == 0:
							auth_log.logger.info(" [sys_logmon 135]start mtx sucess")
						else:
							auth_log.logger.warn(" [sys_logmon 136]start mtx fail")
					else:
						auth_log.logger.info(" *****************mtx status is %s in gmsconfig.xml *********" % mtx_st)
				if online_num == False:
					if apt_st == "1":
						offline_path,offline_file=os.path.split(auth_log.path_dict["offline_path"]) 
						ret=os.system("cd "+offline_path+";nohup ./"+offline_file+" 1>/dev/null 2>&1 &")
						if ret == 0:
							auth_log.logger.info(" [sys_logmon 142]start offline sucess")
						else:
							auth_log.logger.warn(" [sys_logmon 144]start offline fail")
					else:
						auth_log.logger.info(" *****************onffline status is %s in gmsconfig.xml *********" % apt_st)
				if apt_num == False:
					if apt_st == "1":

						apt_path,apt_file=os.path.split(auth_log.path_dict["apt_path"]) 
						ret=os.system("cd "+apt_path+";nohup ./"+apt_file+" 1>/dev/null 2>&1 &")
						if ret == 0:
							auth_log.logger.info(" [sys_logmon 142]start apt sucess")
						else:
							auth_log.logger.warn(" [sys_logmon 144]start apt fail")
					else:
						auth_log.logger.info(" *****************apt status is %s in gmsconfig.xml *********" % apt_st)
				if vds_num == False:
					if vds_st == "1":
						vds_path,vds_file=os.path.split(auth_log.path_dict["vds_path"]) 
						ret=os.system("cd "+vds_path+";nohup "+vds_file+" 1>/dev/null 2>&1 &")
						if ret == 0:
							auth_log.logger.info(" [sys_logmon 148]start vds sucess")
						else:
							auth_log.logger.warn(" [sys_logmon 150]start vds fail")
					else:
						auth_log.logger.info(" *****************vds status is %s in gmsconfig.xml *********" % vds_st)
				if url_num == False:
					url_path,url_file=os.path.split(auth_log.path_dict["url_detect"]) 
					ret=os.system("cd "+url_path+";nohup python "+url_file+"  1>/dev/null 2>&1 &")
					if ret == 0:
						auth_log.logger.info(" [sys_logmon 148]start url_detect.py sucess")
					else:
						auth_log.logger.warn(" [sys_logmon 150]start url_detect.py fail")
				#*********************************************************************************************************************************************
				#if mtx_num == False:
				#	ret=start_mtx()
				#	if ret == 0:
				#		auth_log.logger.info(" [sys_logmon 159]start mtx sucess")
				#	else:
				#		auth_log.logger.warn(" [sys_logmon 161]start mtx fail")
				#if online_num == False:
				#	ret=os.system("cd /gms/apt/offline;nohup ./offline 1>/dev/null 2>&1 &")
				#	if ret == 0:
				#		auth_log.logger.info(" [sys_logmon 142]start offline sucess")
				#	else:
				#		auth_log.logger.warn(" [sys_logmon 144]start offline fail")
				#if apt_num == False:
				#	ret=os.system("cd /gms/apt/online;nohup ./apt 1>/dev/null 2>&1 &")
				#	if ret == 0:
				#		auth_log.logger.info(" [sys_logmon 165]start apt sucess")
				#	else:
				#		auth_log.logger.warn(" [sys_logmon 167]start apt fail")
				#if vds_num == False:
				#	ret=os.system("nohup /vds/script/monitor.sh 1>/dev/null 2>&1 &")
				#	if ret == 0:
				#		auth_log.logger.info(" [sys_logmon 171]start apt sucess")
				#	else:
				#		auth_log.logger.warn(" [sys_logmon 173]start apt fail")
				#if url_num == False:
				#	ret=os.system("cd /gms/url_detect/;nohup python url_detect.py  1>/dev/null 2>&1 &")
				#	if ret == 0:
				#		auth_log.logger.info(" [sys_logmon 242]start url_detect.py sucess")
				#	else:
				#		auth_log.logger.warn(" [sys_logmon 244]start url_detect.py fail")
			else:
				if mtx_num != False:
					os.system("kill -9 %s" % mtx_num)
					auth_log.logger.info(" [**************not reg try kill mtx **************************")
				if online_num != False:
					os.system("kill -9 %s" % online_num)
					auth_log.logger.info(" [*************not reg try kill  offline **********************")
				if apt_num != False:
					os.system("kill -9 %s" % apt_num)
					auth_log.logger.info(" [*************not reg try kill apt ***************************")
				if vds_num != False:
					os.system("kill -9 %s" % vds_num)
					os.system("killall apc")
					os.system("killall dm")
					auth_log.logger.info(" [*************not reg try kill  vds **************************")
				if url_num != False:
					os.system("kill -9 %s" % url_num)
					auth_log.logger.info(" [*************not reg try kill  url_detect.py ****************")
				auth_log.logger.warn(" [sys_logmon 157] This is monitor ,but don't reg....")
		elif manager_node == "1":
			if trans_num != False:
				os.system("kill -9 %s" % trans_num)
			if cloud_num != False:
				os.system("kill -9 %s" % cloud_num)
			if event_num != False:
				os.system("kill -9 %s" % event_num)
			if url_num != False:
				os.system("kill -9 %s" % url_num)
			if online_num != False:
				os.system("kill -9 %s" % online_num)
			mtx_flag=os.popen("killall -9 mtx")
			auth_log.logger.warn("[sys_logmon 138] not is manager mtx will killed %s" % mtx_flag.read())
			apt_flag=os.popen("killall -9 apt")
			auth_log.logger.warn("[sys_logmon 140] not is manager apt will  killed %s" % apt_flag.read())
			dd_flag=os.popen("killall -9 dd")
			auth_log.logger.warn("[sys_logmon 142] not is manager vds ->dd will killed %s" % dd_flag.read())
			apc_flag=os.popen("killall -9 apc")
			auth_log.logger.warn("[sys_logmon 144] not is manager vds ->apc will killed %s" % apc_flag.read())
			dm_flag=os.popen("killall -9 dm")
			auth_log.logger.warn("[sys_logmon 146] not is manager vds ->dm  will killed %s" % dm_flag.read())
		return 0
	else:
		#print "into False"
		xml_logmon(filename,auth_infor)
		if trans_num != False:
			os.system("kill -9 %s" % trans_num)
		if cloud_num != False:
			os.system("kill -9 %s" % cloud_num)
		if event_num != False:
			os.system("kill -9 %s" % event_num)
		if url_num != False:
			os.system("kill -9 %s" % url_num)
		if online_num != False:
			os.system("kill -9 %s" % online_num)
		#cron_flag=os.popen("killall -9 crond")
		#auth_log.logger.warn("[sys_logmon 128] crond have authed due date killing %s" % cron_flag.read())
		guard_flag=os.popen("killall -9 sys_guard")
		auth_log.logger.warn("[sys_logmon 130] sys_guard have authed due date killing %s" % guard_flag.read())
		store_flag=os.popen("killall -9 store_proc")
		auth_log.logger.warn("[sys_logmon 132] store_flag have authed due date killing %s" % store_flag.read())
		flow_flag=os.popen("killall -9 flow_proc")
		auth_log.logger.warn("[sys_logmon 134] flow_proc have authed due date killing %s" % flow_flag.read())
		merge_flag=os.popen("killall -9 merge_proc")
		auth_log.logger.warn("[sys_logmon 136] merge_proc have authed due date killing %s" % merge_flag.read())
		mtx_flag=os.popen("killall -9 mtx")
		auth_log.logger.warn("[sys_logmon 138] mtx have authed due date killing %s" % mtx_flag.read())
		apt_flag=os.popen("killall -9 apt")
		auth_log.logger.warn("[sys_logmon 140] apt have authed due date killing %s" % apt_flag.read())
		dd_flag=os.popen("killall -9 dd")
		auth_log.logger.warn("[sys_logmon 142] vds ->dd have authed due date killing %s" % dd_flag.read())
		apc_flag=os.popen("killall -9 apc")
		auth_log.logger.warn("[sys_logmon 144] vds ->apc have authed due date killing %s" % apc_flag.read())
		dm_flag=os.popen("killall -9 dm")
		auth_log.logger.warn("[sys_logmon 146] vds ->dm  have authed due date killing %s" % dm_flag.read())
		if cloud_num != False:
			os.system("kill -9 %s" % cloud_num)
		if comm_num != False:
			os.system("kill -9 %s" % comm_num)
		if event_num != False:
			os.system("kill -9 %s" % event_num)
		if url_num != False:
			os.system("kill -9 %s" % url_num)
		auth_log.logger.warn("[sys_logmon 136] python have authed due date killing %s" % merge_flag.read())
		return 15

