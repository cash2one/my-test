#!/usr/python
#Filename:test.py
import ctypes
if __name__ == '__main__':
	getfile="1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
	ll=ctypes.cdll.LoadLibrary  
	lib=ll("./test.so")
	lib.Dev_id(getfile)
	print getfile
	print getfile
