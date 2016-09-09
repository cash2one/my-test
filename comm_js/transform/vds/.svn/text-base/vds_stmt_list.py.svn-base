# _*_ coding: utf8 _*_

import os
import sys
import time
import shutil
import socket
import traceback
import vds_exec_info as exec_info
from vds_logger import logger
from vds_path_conf import backup_dir

class VdsStmtList():
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
            with open(self.fname) as fp:
                #print("===================> file name [%s]" % self.fname)
                #field_info = fp.readline().strip()
                for line in fp:
                    #print("===================> line [%s]" % line)
                    if line.strip() == '':
                        continue
                    exec_info.inc_line_cnt()
                    field_list = line.strip().split('|')
                    field_dict = {}
                    field_dict['TS'] = field_list[0].split()[1][3:].strip()
                    field_dict['PROC'] = field_list[1].strip()
                    field_dict['VXID'] = field_list[2].strip()
                    field_dict['VXName'] = field_list[3].strip()
                    field_dict['ENName'] = field_list[4].strip()
                    field_dict['SIP'] = field_list[5].strip()
                    field_dict['DIP'] = field_list[6].strip()
                    field_dict['SPORT'] = field_list[7].strip()
                    field_dict['DPORT'] = field_list[8].strip()
                    field_dict['PID'] = field_list[9].strip()
                    field_dict['TDIR'] = field_list[10].strip()
                    field_dict['ADIR'] = field_list[11].strip()
                    field_dict['CID'] = field_list[12].strip()
                    field_dict['RISK'] = field_list[13].strip()
                    field_dict['IDIR'] = field_list[14].strip()
                    field_dict['FID'] = field_list[15].strip()
                    field_dict['EType'] = field_list[16].strip()
                    field_dict['BType'] = field_list[17].strip()
                    field_dict['SType'] = field_list[18].strip()
                    field_dict['SMAC'] = field_list[19].strip()
                    field_dict['DMAC'] = field_list[20].strip()
                    field_dict['APN'] = field_list[21].strip()
                    field_dict['IMSI'] = field_list[22].strip()
                    field_dict['MSISDN'] = field_list[23].strip()
                    field_dict['IME'] = field_list[24].strip()
                    field_dict['URL'] = field_list[25].strip()
                    #print("===================> field_dict = %s" % field_dict)

                    self.linum += 1
                    yield field_dict
