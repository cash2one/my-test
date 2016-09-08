#!/usr/bin/python 
# _*_ coding: utf8 _*_
import os,sys 
def check_port(port):
	try:
		int_port=int(port)
	except:
		return 21
	if int_port < 1 or int_port >65535:
		return 21
	return 0
	
def check_ip(ipaddr): 
	import sys 
	addr=ipaddr.strip().split('.')  #切割IP地址为一个列表 
	#print addr 
	if len(addr) != 4:  #切割后列表必须有4个参数 
		print "check ip address failed!"
		return 1 
	for i in range(4): 
		try: 
			addr[i]=int(addr[i])  #每个参数必须为数字，否则校验失败 
		except: 
			print "check ip address failed!"
	 		return 1 
		if addr[i]<=255 and addr[i]>=0:    #每个参数值必须在0-255之间 
			pass
		else: 
			print "check ip address failed!"
	 		return 1 
	print "check ip address success!"
	return 0
if __name__ == "__main__":
	if  len(sys.argv)!=2:  #传参加本身长度必须为2 
		print "Example: %s 10.0.0.1 "%sys.argv[0] 
		exit(1) 
	else: 
		check_ip(sys.argv[1])  #满足条件调用校验IP函数

