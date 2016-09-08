#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os
import sys
import md5
import time
import types
import shutil
import zipfile
import random
import threading


class CommException(Exception):
    def __init__(self, msg):
        self.msg = msg
        BaseException.__init__(self)

    def __str__(self):
        return self.msg

stop = False
def is_continue():
    if stop:
        return False
    else:
        return True

def stop_process():
    global stop
    stop = True

def sumfile(fobj):
    '''Returns an md5 hash for an object with read() method.'''
    m = md5.new()
    while is_continue():
        d = fobj.read(8096)
        if not d:
            break
        m.update(d)
    return m.hexdigest()

def md5sum(fname):
    '''Returns an md5 hash for file fname, or stdin if fname is "-".'''
    if fname == '-':
        ret = sumfile(sys.stdin)
    else:
        try:
            f = file(fname, 'rb')
        except:
            return 'Failed to open file'
        ret = sumfile(f)
        f.close()
    return ret

def md5cmp(md5, fname):
    if md5 == md5sum(fname):
        return True
    else:
        return False

#serialno = random.randint(1, 65535)
def get_serialno():
    #global serialno
    #serialno += 1
    #return serialno
    # 上面的做法在范围超过 2^32 4294967296 时会有问题。
    # 在 python 中 socket.htonl 4295952528 和 985232 有相同值。
    # 4295952528 - 985232 == 2^32
    return random.randint(1, 65535)

def file_filter_bysuffix(input, suffix=None):
    def ffilter(list, dirname, fnames):
        if suffix != None:
            fnames = filter(lambda f: os.path.splitext(f)[1] in suffix, fnames)

        list.extend(filter(os.path.isfile, map(lambda f: os.path.join(dirname, f), fnames)))

    def fprocess(path):
        path = os.path.abspath(path)
        if os.path.isfile(path):
            flist.append(path)
        elif os.path.isdir(path):
            os.path.walk(path, ffilter, flist)

    flist = []
    dlist = []
    if type(input) in types.StringTypes:
        dlist.append(input)
    elif type(input) == types.ListType:
        dlist = input
    map(fprocess, dlist)
    flist.sort()
    #print flist
    return flist

def comm_file_copy(src_file, dst_dir):
    dst_file = os.path.join(dst_dir, os.path.split(src_file)[-1])
    tmp_dst_file = dst_file + ".tmp"
    shutil.copy(src_file, tmp_dst_file)
    os.rename(tmp_dst_file, dst_file)

def file_filter(input, filter_func):
    def filter_func_wrapper(list, dirname, fnames):
        fnames = [os.path.join(dirname, fname) for fname in fnames]
        fnames = filter(filter_func, [fname for fname in fnames if os.path.isfile(fname)])
        return list.extend(fnames)

    flist = []
    dlist = []
    if type(input) in types.StringTypes:
        dlist.append(input)
    elif type(input) == types.ListType:
        dlist = input
    for path in dlist:
        path = os.path.abspath(path)
        if os.path.isfile(path) and filter_func(path):
            flist.append(path)
        elif os.path.isdir(path):
            os.path.walk(path, filter_func_wrapper, flist)
    flist.sort()
    #print flist
    return flist

def file_list(input):
    return file_filter(input, lambda f: True)

def do_something_if_dir_notexist(dir, func):
    if not os.path.exists(dir) or not os.path.isdir(dir):
        func(dir)

def raise_if_dir_notexist(dir):
    def func(dir):
        raise(IOError("dir[%s] no exist." % dir))
    do_something_if_dir_notexist(dir, func)

def creat_if_dir_notexist(dir):
    do_something_if_dir_notexist(dir, os.makedirs)

def createzip(zfname, srcfname, passwd='9527'):
    # 调用命令行。python 自带的 zipfile 不支持待密码的压缩
    import subprocess
    tmpfname = zfname + '.tmp'
    p = subprocess.Popen(['zip', '-j', '-P', passwd, tmpfname, srcfname],
                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()
    os.rename(tmpfname, zfname)

def cmdextractzip(zfname, dstpath, passwd):
    ''' 解压出错返回 False。成功返回文件名列表。'''
    import zipfile
    try:
        flist = []
        fp = zipfile.ZipFile(zfname)
        fp.setpassword(passwd)
        for srcname in fp.namelist():
            dstname = os.path.join(dstpath, srcname)
            tmpname = '%s.%s' % (dstname, time.time())
            result = os.system('unzip -p -P %s %s %s > %s' % (passwd, zfname, srcname, tmpname))
            if result == 0:
                os.rename(tmpname, dstname)
                flist.append(dstname)
            else:
                # 如果设备上的 unzip 不支持 -P 也会生成 tmpname 文件。在这里删除。
                if os.path.exists(tmpname):
                    os.remove(tmpname)
                return False
        fp.close()
        return flist
    except:
        return False

# 先调用系统的 unzip 解压。找不到再调用 python 的库。
# python 自带的解压缩函数比系统 unzip 慢了很多。
# 而且没有分段读的接口，只能一次读出整个文件，有可能占用内存过大被杀掉。
# 解压缩 100 M 的文件，分别用时 511.67  3.16
# 解压缩 1G 的文件，分别用时 6097.17 39.76
def extractzip(zfname, dstpath, passwd='9527'):
    import zipfile
    result = cmdextractzip(zfname, dstpath, passwd)
    if result == False:
        try:
            flist = []
            fp = None
            fp = zipfile.ZipFile(zfname)
            fp.setpassword(passwd)
            for srcname in fp.namelist():
                dstname = os.path.join(dstpath, srcname)
                tmpname = '%s.%s' % (dstname, time.time())
                with open(tmpname, 'w') as tmpfp:
                    tmpfp.write(fp.read(srcname))
                os.rename(tmpname, dstname)
                flist.append(dstname)
            fp.close()
            return flist
        except:
            if fp: fp.close()
            raise
    else:
        return result

# 读 zip 文件列表不需要密码
def listzip(zfname):
    import zipfile
    fp = zipfile.ZipFile(zfname)
    # 只要是 zip 文件，就一定能读取文件列表，不需要担心异常
    flist = fp.namelist()
    fp.close()
    return flist

# 从文件列表创建 zip 文件。
def createzip2(zfname, arcfname, srcfname):
    tmpzfname = zfname + ".tmp"
    fp = zipfile.ZipFile(tmpzfname, 'w', zipfile.ZIP_DEFLATED)
    fp.write(srcfname, arcfname)
    fp.close()
    os.rename(tmpzfname, zfname)

def create_thread(func, args=(), name=None):
    thread = threading.Thread(target=func, args=args)
    if name:
        thread.setName(name)
    thread.setDaemon(True)
    thread.start()
    return thread

# 引用pid描述符，确保在程序执行时文件描述符不会被自动释放
pidfp = None
def check_pidfile():
    import fcntl
    global pidfp
    pidfile = '/var/run/%s.pid' % os.path.splitext(os.path.basename(sys.argv[0]))[0]
    pidfp = open(pidfile, 'w')
    try:
        fcntl.flock(pidfp, fcntl.LOCK_EX | fcntl.LOCK_NB)
        pidfp.write("%s\n" % os.getpid())
    except:
        print("another %s is running... exit." % sys.argv[0])
        stop_process()

def safe_copy(srcfile, dstpath):
    if os.path.isdir(dstpath):
        dstfile = os.path.join(dstpath, os.path.basename(srcfile))
    else:
        dstfile = dstpath
    # 只用 .tmp 作为后缀有时候不安全
    tmpfile = dstfile + '.%d' % time.time()
    shutil.copy(srcfile, tmpfile)
    os.rename(tmpfile, dstfile)

if __name__ == '__main__':
    #print "list dir['./', './transform/']: "
    #for file in file_filter_bysuffix(['./transform/', './'], [".pyc", ".py"]):
    #    print file,
    #print "\nlist dir['./transform/']: "
    #for file in file_filter_bysuffix('./transform/', [".pyc", ".py"]):
    #    print file,

    createzip(sys.argv[1], sys.argv[2])
    #createzip(sys.argv[1], [sys.argv[2], sys.argv[3]])
    #print extractzip(sys.argv[1], sys.argv[2])

