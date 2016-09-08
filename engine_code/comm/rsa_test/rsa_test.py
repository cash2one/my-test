import base64
import ctypes
import sys
import os
 
rsa_file = "/gms/comm/rsa_test/rsa.1024.pub"
dir_auth = "/gms/conf/"
def str_split(src_str,flag,num):
	dst_str=src_str.split(flag)
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
	file_size=os.path.getsize(filename)
	des_buf=fp.read(file_size-auth_len)
	rsa_buf=fp.read()
	rsa_w=open('rsa_123',"w+")
	des_w=open('des_123',"w+")
	rsa_w.write(rsa_buf)
	des_w.write(des_buf)
	fp.close()
	rsa_w.close()
	des_w.close()

def rsa_decode(rsa_tmppath, des_tmppath):
	tmp_cmd= "openssl dgst -verify "+rsa_file+" -sha1 -signature "+rsa_tmppath+" "+des_tmppath
        print tmp_cmd
	if os.system(tmp_cmd) != 0:
		sys.exit(14)
	else:
		return 0

def des_decode(des_file,keystr):
	getfile="???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????"
	print "des_decode",des_file
	ll=ctypes.cdll.LoadLibrary
	lib=ll("/gms/comm/rsa_test/mydes.so")
	lib.des_descrypt("chanct-gms",des_file,getfile)
	#auth_log.logger.debug("[readdir 64] des decode...")
	return getfile

def main(auth_file_path, authfile):
    #auth_file_path = '/home/wangkun/Desktop/test/auth-128-20150401093737.gau'
    #authfile = 'auth-128-20150401093737.gau'
    file_split(auth_file_path, int(str_split(authfile,"-",1)))
    rsa_tmppath = "/gms/comm/rsa_test/rsa_123"
    des_tmppath = "/gms/comm/rsa_test/des_123"
    rsa_decode(rsa_tmppath, des_tmppath)
    getfile=des_decode(des_tmppath,"1")
    auth_infor=getfile.split(";")
    return auth_infor

def rsa_test():
    file_all = os.listdir(dir_auth)
    file_gau_dict = dict([(i, os.path.getmtime(dir_auth + i)) for i in os.listdir(dir_auth) if os.path.splitext(i)[1] ==  '.gau']) 
    file_gau = sorted(file_gau_dict.iteritems(), key=lambda d:d[1], reverse = True)[0][0]
    auth_infor = main(dir_auth + file_gau, file_gau)
    return  (auth_infor[24], auth_infor[25])
if __name__ == "__main__":
    #rsa_file = "./rsa.1024.pub"
    #auth_infor = main("/gms/comm/rsa_test/auth-128-20150401100817.gau", "auth-128-20150401100817.gau")
    #print  auth_infor[24]
    print rsa_test()
