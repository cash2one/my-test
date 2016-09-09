#!/usr/bin/python
# _*_coding:utf8 _*_
import signal
import os
import sys
import time
import xml.dom.minidom
import threading
import thread
import types
import zipfile

out_mtd_file_src_path = "/filedata/gms/comm/me/up/"
out_mtd_file_dst_path = "/gms/me/up/"

in_mtd_file_src_path = "/gms/me/down/"
in_mtd_file_dst_path = "/filedata/gms/ali/me/down/"


#------------------------------.
# @brief 主程序捕获函数		   |
#------------------------------*
def sig_exit(sig, stack):
    print("recv signal[%d]. exit." %sig)
    exit()

def write_buf_to_file(buf, file_path):
    try:
        tmp_file = open(file_path, 'w')
    except IOError, e:
        print e
        return 1
    else:
        tmp_file.write(buf)
        tmp_file.close()
        return 0

def get_buf_from_file(file):
    buf = ""
    fd = open(file, "r") 
    buf = fd.read() 
    fd.close()
    return buf 

def createzip(zfile, files):
    flist = []
    if types.StringType == type(files):
        flist.append(files)
    elif types.ListType == type(files):
        flist = files
    zfile = os.path.abspath(zfile)
    tmpzfile = zfile + ".tmp"
    f = zipfile.ZipFile(tmpzfile, 'w', zipfile.ZIP_DEFLATED)
    for sfile in flist:
        sfile = os.path.abspath(sfile)
        f.write(sfile, os.path.split(sfile)[-1])
    f.close()
    os.rename(tmpzfile, zfile)

def extractzip(zfile, dpath):
    dpath = os.path.abspath(dpath)
    f = zipfile.ZipFile(zfile, 'r')
    namelist = f.namelist()
    dstnamelist = []
    try:
        for fname in namelist:
            data = f.read(fname)
            dstfile = os.path.join(dpath, fname)
            tmpdstfile = dstfile + ".tmp"
            tmpf = open(tmpdstfile, "w")
            tmpf.write(data)
            tmpf.close()
            os.rename(tmpdstfile, dstfile)
            dstnamelist.append(dstfile)
    except Exception as reason:
        # 在发生异常时，删除已经解压的文件
        for file in dstnamelist:
            os.remove(file)
        f.close()
        raise
    else:
        f.close()
        return dstnamelist

 #-----------------------------------------.
# @brief 遍历制定目录，得到指定后缀的文件 |
# @param path		文件目录			  |
# @param suffix		后缀				  |
#-----------------------------------------*
def file_filter_bysuffix(input, suffix=None):
    def ffilter(list, dirname, fnames):
        if suffix != None:
            fnames = filter(lambda f: os.path.splitext(f)[1] in suffix, fnames)

        list.extend(filter(os.path.isfile, map(lambda f: os.path.join(dirname, f), fnames)))

    def fprocess(path):
        path = os.path.expanduser(path)
        if os.path.isfile(path):
            flist.append(path)
        elif os.path.isdir(path):
            os.path.walk(path, ffilter, flist)

    flist = []
    map(fprocess, input)
    return flist   

def move_file_out():

    while True:

        for mtd_file in file_filter_bysuffix([out_mtd_file_src_path], ['.ok']):
            dst_zip_file = out_mtd_file_dst_path +"zip/" +os.path.basename(mtd_file) + ".tmp"
            final_zip_file = out_mtd_file_dst_path + os.path.basename(mtd_file)

            buf = ""
            buf = get_buf_from_file(mtd_file)
            retn = write_buf_to_file(buf, dst_zip_file)
            os.rename(dst_zip_file, final_zip_file)    
            
            print "MOVE FILE{%s} -> FILE[%s]" %(mtd_file, final_zip_file)
            if (retn == 0):
                os.remove(mtd_file)
                print "DELETE FILE{%s}!" %(mtd_file)

        time.sleep(0.5)

def move_file_in():
    while True:

        for mtd_file in file_filter_bysuffix([in_mtd_file_src_path], ['.ok']):

            final_zip_file = in_mtd_file_src_path + "zip/" + os.path.basename(mtd_file)
            dst_file = in_mtd_file_dst_path + os.path.basename(mtd_file) + ".tmp"
            final_file = in_mtd_file_dst_path + os.path.basename(mtd_file)
            createzip(final_zip_file, mtd_file)
            os.remove(mtd_file)

            buf = ""
            buf = get_buf_from_file(final_zip_file)
            retn = write_buf_to_file(buf, dst_file)
            os.rename(dst_file, final_file)    

            print "MOVE FILE[%s] -> FILE{%s}" %(final_zip_file, final_file)
            if (retn == 0):
                os.remove(final_zip_file)
                print "DELETE FILE[%s]" %(final_zip_file)
            

        time.sleep(0.5)


def extractzip_busi():
    dst_path = out_mtd_file_dst_path
    zip_path = out_mtd_file_dst_path+"zip/"
    while True:
        for zip_file in file_filter_bysuffix([zip_path],[".ok"]):
            try:
                comm_common.extractzip(zip_file,dst_path)
            except Exception, ex:
                print "==extractzip Excepiton==>file[%s]" %(zip_file)
                print ex
                continue
            os.remove(zip_file)
        time.sleep(0.5)



def main():

    #开启文件传出线程
    try:
        thread = threading.Thread(target=move_file_out,args=())
        thread.setDaemon(True)
        thread.start()
    except Exception, ex:
        print ex
       

    #开启文件传入线程
    try:
        thread = threading.Thread(target=move_file_in,args=())
        thread.setDaemon(True)
        thread.start()
    except Exception, ex:
        print ex
    
    #zip解压线程
    try:
        thread = threading.Thread(target=extractzip_busi,args=())
        thread.setDaemon(True)
        thread.start()
    except Exception, ex:
        print ex

if __name__=="__main__":
    #注册捕捉退出信号
    signal.signal(signal.SIGINT, sig_exit)

    main()

    while (True):
        time.sleep(3)
