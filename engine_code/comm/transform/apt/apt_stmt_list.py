# _*_ coding: utf8 _*_

import os
import sys
import time
import shutil
import socket
import traceback
import apt_exec_info as exec_info
from apt_logger import logger
from apt_path_conf import backup_dir

class AptStmtList():
    def __init__(self, fname):
        self.linum = 0
        self.fname = fname
        self.ts = time.time()
        exec_info.inc_file_cnt()

    def __enter__(self):
        return self

    def __exit__(self, etype, value, tb):
        # 捕捉在文件转换过程中发生的异常
        exec_info.seconds += time.time() - self.ts
        exec_info.cyclicaldump()
        if etype == None:
            return True
        else:
            srcfile = self.fname
            dstfile = os.path.join(backup_dir, os.path.basename(srcfile))
            shutil.copy(srcfile, dstfile)
            logger.info("**** process file[%s] linum[%d] ERROR. ****" % (self.fname, self.linum))
            logger.info(traceback.format_exception(etype, value, tb))
            return True

    def stmts(self):
        if os.path.getsize(self.fname) == 0:
            exec_info.inc_null_file_cnt()
        else:
            #print("+ %s" % self.fname)
            with open(self.fname) as fp:
                field_info = fp.readline().strip()
                field_name_list = map(str.strip, field_info.split('\t'))
                for line in fp:
                    self.linum += 1
                    exec_info.inc_line_cnt()

                    if line.strip() == '':
                        continue

                    field_list = map(str.strip, line.strip().split('\t'))
                    field_dict = dict(zip(field_name_list, field_list))
                    #print("===================> field_dict = %s" % field_dict)

                    yield field_dict
