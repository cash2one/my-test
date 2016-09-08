#!/usr/bin/python
# _*_ coding: utf8 _*_
import os
import time
import sys
import ctypes
import fcntl
import auth_log
import sys_logmon
auth_conf_path="/gms/gapi/modules/auth/conf/auth_path.json"
rsa_file="/gms/auth/conf/rsa.1024.pub"
rsa_tmppath="/gms/auth/conf/tmp_rsa"
des_tmppath="/gms/auth/conf/tmp_des"
def endWith(*endstring):
	ends = endstring
	def run(s):
		f = map(s.endswith,ends)
		if True in f: return s
	return run
def get_filefrom_path(filepath,filetail):
	list_file = os.listdir(filepath)
	a = endWith(filetail)
	f_file = filter(a,list_file)
	#for i in f_file: print i
	return f_file

def str_split(src_str,flag,num):
	dst_str=src_str.split(flag)
	#print dst_str
	#for i in dst_str:
	#	print i
	if num != 2:
		return dst_str[num]
	time_gau=dst_str.pop()
	time_str=time_gau.split('.')
	return time_str[0]

def file_split(filename,auth_len):
	try:
		fp=open(filename,"rb")
	except:
		exit(14)
	fcntl.flock(fp, fcntl.LOCK_EX)
	file_size=os.path.getsize(filename)
	des_buf=fp.read(file_size-auth_len)
	rsa_buf=fp.read()
	rsa_w=open(rsa_tmppath,"w+")
	fcntl.flock(rsa_w, fcntl.LOCK_EX)
	des_w=open(des_tmppath,"w+")
	fcntl.flock(des_w, fcntl.LOCK_EX)
	rsa_w.write(rsa_buf)
	des_w.write(des_buf)
	fcntl.flock(rsa_w, fcntl.LOCK_UN)
	fcntl.flock(des_w, fcntl.LOCK_UN)
	fcntl.flock(fp, fcntl.LOCK_UN)
	fp.close()
	rsa_w.close()
	des_w.close()
	
def rsa_decode():
	tmp_cmd= "openssl dgst -verify "+rsa_file+" -sha1 -signature "+rsa_tmppath+" "+des_tmppath
	if os.system(tmp_cmd) != 0:
		auth_log.logger.error("[readdir 54] rsa decode fail...")
		sys.exit(14)
	else:
		auth_log.logger.debug("[readdir 53] rsa decode sucess...")
		return 0

def des_decode(des_file,keystr):
	getfile="????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????"
	print "des_decode",des_file
	ll=ctypes.cdll.LoadLibrary
	lib=ll("/gms/gapi/modules/auth/os/mydes.so")
	lib.des_descrypt("chanct-gms",des_file,getfile)
	#auth_log.logger.debug("[readdir 64] des decode...")
	return getfile
def get_devid_id():
	getfile="??????????????"
	ll=ctypes.cdll.LoadLibrary
	lib=ll("/gms/gapi/modules/auth/os/genkey.so")
	lib.Dev_id(getfile)
	auth_log.logger.debug("[readdir 71] get devid %s" % getfile)
	return getfile
def strom_time():
	inttime=time.time()
	strutime=time.localtime(inttime)
	if strutime[1] <10:
		moutmp="0"+str(strutime[1])
	else:
		moutmp=str(strutime[1])
	if strutime[2] <10:
		daytmp="0"+str(strutime[2])
	else:
		daytmp=str(strutime[2])
	str_time=str(strutime[0])+moutmp+daytmp+'00'+'00'+'00'
	return time.mktime(time.strptime(str_time,'%Y%m%d%H%M%S'))
def main():
	auth_log.read_json_conf(auth_conf_path)
	auth_log.record_log_init()
	#rsa_file=auth_log.path_dict['rsa_filepath']
	#rsa_tmppath=auth_log.path_dict["rsa_tmppath"]
	#des_tmppath=auth_log.path_dict["des_tmppath"].strip()
	if len(sys.argv) > 1:
		authfile=sys.argv[1]
		auth_log.logger.debug("[readdir 72] %s Do authorization operation" % authfile)
	else:
		f_ftmp=get_filefrom_path("/gms/conf",".gau")
		if f_ftmp == []:
			auth_log.logger.error("[readir] Not found auth_file")
			exit(9)
		time_end = float(0)
		authfile=f_ftmp[0]
		for f in f_ftmp:
			time_str=str_split(f,"-",2)
			#print time_str
			if time.mktime(time.strptime(time_str,'%Y%m%d%H%M%S')) > time_end:
				authfile=f
				time_end=time.mktime(time.strptime(time_str,'%Y%m%d%H%M%S'))
		auth_log.logger.debug("[readdir 85] %s Check the state of authorization" % authfile)
	auth_file_path="/gms/conf/"+authfile
	# 获得license文件签名的长度len
	auth_len=int(str_split(authfile,"-",1))
	# 将license文件签名和des加密的内容分别存到tmp_rsa和tmp_des文件中
	file_split(auth_file_path,auth_len)
	# rsa解密
	rsa_decode()
	# des解密
	auth_log.logger.debug("[readdir 96] check DES")
	getfile=des_decode(des_tmppath,"1")
	auth_infor=getfile.split(";")
	dev_id_num=get_devid_id()
	auth_log.logger.debug("[readdir 95] %s" % getfile)
	auth_log.logger.debug("[readdir 96] check devid ID")
	print dev_id_num
	if dev_id_num[:14] == auth_infor[1][:14]:
		auth_log.logger.debug("[readir 106] devid True")
	else:
		auth_log.logger.error("[readir 107] devid False")
		exit(14)
	mtx_str_time=auth_infor[auth_infor.index("mtx")+1]
	#print auth_infor.index("mtx"),mtx_str_time
	if mtx_str_time == '1':
		auth_log.logger.debug("[readir 116] mtx forever auth")
		ret=True
	elif mtx_str_time == '0':
		auth_log.logger.debug("[readir 119] mtx don't auth")
		ret=True
	else:
		mtx_time_t=time.mktime(time.strptime(mtx_str_time,'%Y%m%d%H%M%S'))
		now_time_t=strom_time()
		if mtx_time_t > now_time_t :
			auth_log.logger.debug("[readir 125] engine auth to %s" % mtx_str_time)
			ret=True
		else:
			auth_log.logger.warn("[readir 128] engine have authed due date")
			ret=False
	ret=sys_logmon.py_logmon(authfile,auth_infor,ret)
	#exit(ret)

	return ret
	#print getfile,ret

if __name__ == '__main__':
	exit(main())

