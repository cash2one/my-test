#! /usr/local/bin/python
# -*- coding:utf-8 -*-

import os
import commands
import string
import sys
sys.path.append("/gms/gapi/modules/auth/")
sys.path.append("/gms/comm/")
import com_to_conf
import sys_auth
import base64
import pyDes
import atexit
src_file_path='/data/tmpdata/debug/'
def get_plain_sys_info():
	"""get_plain_sys_info() --> string
	get sys_info unprocessed by magicencode()
	"""
	# GMS path
	cmds = [

			#system info
			'echo "#system info"',
			'/bin/date',
			'/bin/cat /proc/version',
			'/gms/bin/genkey',
			'/bin/cat /proc/meminfo',
			'/bin/cat /proc/cpuinfo',
			'/bin/cat /proc/interrupts',
			'/bin/cat /gms/conf/version.xml',
			'/bin/dmesg',
			'/usr/bin/uptime',
			'/bin/df',
			'/bin/ps -ef',
			'/sbin/lsmod',
			'/bin/cat /etc/hosts',
			'/bin/cat /proc/cmdline',
			'/bin/cat /proc/slabinfo',

			
			#network info
			'/sbin/ifconfig',
			'/sbin/ifconfig -a',
			'/bin/netstat -an',
			'/bin/netstat -r',
			'/sbin/route',
			'/bin/cat /etc/resolv.conf',
			'/sbin/arp -an',


		]

	
	sysinfo='GmsSysInfo Version 2014/03/02 by Auther:lichuanhai \r\n'
	for i in cmds:
		sysinfo=sysinfo+'\r\n\r\n-------------------------\r\n%s\r\n------------------------\r\n'%i
		sysinfo=sysinfo+commands.getoutput(i)

	return sysinfo


def des_encrypt_auth_body(data):
	key = pyDes.des("chGMS123", pyDes.CBC, "\0\0\0\0\0\0\0\0")
	s = key.encrypt(data, "*")
	return base64.encodestring(s)

def des_decrypt_auth_body(data):
	key = pyDes.des("chGMS123", pyDes.CBC, "\0\0\0\0\0\0\0\0")
	data = base64.decodestring(data)
	return key.decrypt(data, "*")
def make_file():
	fp=os.popen('python /gms/gapi/modules/auth/genkey.pyc')
	tmp="mkdir -p "+src_file_path 
	os.system(tmp)
	return src_file_path+'debug_'+fp.read()+'.ok'
def writefile(buf):
	file=make_file()
	try:
		f=open(file,'w')
		f.write(buf)
		f.close()
	except:
		print 'Error when write gmsinfo. Create filename.'
		return -1
	filepath,filename=os.path.split(file)
	print filename
	return 0

def getgmsinfo():
        
	sysinfo = get_plain_sys_info()
	return writefile(des_encrypt_auth_body(sysinfo)) 


def viewgmsinfo(fn):
	try:
		f=open(fn)
		s=f.read()
		f.close()
		s=des_decrypt_auth_body(s)
		print s
	except:
		print 'Error when display gmsinfo. Check filename.'

def usage():
	print """
USAGE:
	getgmsinfo.pyc                  -- get gms sysinfo package
	getgmsinfo.pyc -v <file>        -- view gms sysinfo package
	getgmsinfo.pyc -h               -- display this help
"""

# ---------------------------------
# main start here

if __name__=='__main__':
	if len(sys.argv)==1:
		getgmsinfo()
		if com_to_conf.is_manger_device() == '1' and com_to_conf.read_xge_device() == '1':
			import comm_socket_main
			cmd_str="python "+sys.argv[0]
			atexit.register(comm_socket_main.main,cmd_str)
		#get_plain_sys_info()
	elif len(sys.argv)==3:
		[opt,fn]=sys.argv[1:3]
		if opt=='-v':
			viewgmsinfo(fn)
		else:
			usage()
	elif len(sys.argv)==2:
		[opt]=sys.argv[1:2]
		if opt=='-h' or opt=='-v':
			usage()
	else:
		usage()
	exit(0)




