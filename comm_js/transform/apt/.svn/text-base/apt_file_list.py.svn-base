# _*_ coding: utf8 _*_

import os
import sys
import time
import traceback
from transform.common import isemptydir
from apt_path_conf import input_dirs
from apt_logger import logger


class AptFileList():
    def __init__(self):
        pass

    def __enter__(self):
        return self

    def __exit__(self, etype, value, tb):
        if etype == None:
            return True
        else:
            # 在这里处理目录遍历过程中的异常
            logger.info("**** catch file list ERROR. ****")
            logger.info(traceback.format_exception(etype, value, tb))
            return True

    def dirs(self):
        ''' 返回所有名字合法(yyyymmdd)的 apt 目录。
            并删除处理过的过期(今天以前)目录。 '''
        dlist = []
        for input_dir in input_dirs:
            for dname in os.listdir(input_dir):
                dname = os.path.join(input_dir, dname)
                if self.dname_valid(dname) and os.path.isdir(dname):
                    if self.dname_expired(dname) and isemptydir(dname):
                        #logger.info("remove expired dir[%s]" % dname)
                        os.rmdir(dname)
                    else:
                        dlist.append(dname)
                #else:
                    #logger.info("invailed apt dir name[%s]" % dname)

        return dlist

    def flagfiles(self):
        ''' 返回所有排序后的 .ok 后缀文件的完整路径名 '''
        def fnamecmp(d1, d2):
            k1 = '/'.join(d1.split('/')[-2:])
            k2 = '/'.join(d2.split('/')[-2:])
            return cmp(k1, k2)

        flist = []
        for dname in self.dirs():
            for fname in os.listdir(dname):
                if self.is_flag_file(fname):
                    flist.append(os.path.join(dname, fname))

        flist.sort(fnamecmp)
        #print("sorted flag file list = %s" % flist)
        return flist

    def files(self):
        ''' 返回所有 apt 文件。并删除处理过的文件。
            判断 apt 文件合法的办法是，找到 .ok 后缀的文件，去掉 .ok 后缀就是合法的 apt 文件。 '''
        for flag_fname in self.flagfiles():
            data_fname = flag_fname[:-3]
            if os.path.exists(data_fname):
                #print data_fname
                yield data_fname
                #logger.info("remove file[%s]" % data_fname)
                os.remove(flag_fname)
                os.remove(data_fname)
            else:
                logger.warn("data file[%s] not found." % data_fname)
                os.remove(flag_fname)

    def is_flag_file(self, fname):
        ''' 判断文件名是否有效。dddd.xx.ok。 '''
        # 现在的文件名类似 20140403145800.00.ok 的形式。只需要判断后缀
        fname = os.path.split(fname)[-1]
        basename, extname = os.path.splitext(fname)
        #if extname == '.ok' and basename.isdigit():
        if extname == '.ok':
            return True
        else:
            return False

    def dname_valid(self, dname):
        ''' 判断目录名格式是否有效。(yyyymmdd) '''
        dname = os.path.split(dname)[-1]
        if len(dname) == 8 and dname.isdigit():
            return True
        else:
            return False

    def dname_expired(self, dname):
        ''' 判断目录是否过期。今天以前的目录被认为是过期目录。'''
        dir_date = int(os.path.split(dname)[-1])
        cur_date = int(time.strftime("%Y%m%d", time.localtime(time.time())))
        if cur_date > dir_date:
            return True
        else:
            return False

