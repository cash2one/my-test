# _*_ coding: utf8 _*_

import os
import sys
import time
import shutil
import traceback
from vds_path_conf import input_dir, restore_dir
from vds_logger import logger

class VdsFileList():
    def __init__(self):
        pass

    def __enter__(self):
        return self

    def __exit__(self, etype, value, tb):
        if etype == None:
            return True
        else:
            # 在这里处理目录遍历过程中的异常
            logger.error("**** catch file list ERROR. ****")
            logger.error(traceback.format_exception(etype, value, tb))
            return True

    def files(self):
        flist = os.listdir(input_dir)
        flist.sort()
        #print flist
        for basename in flist:
            extname = os.path.splitext(basename)[-1]
            fullname = os.path.join(input_dir, basename)
            if os.path.isfile(fullname) and extname == '.list':
                # 返回所有以 .list 结尾的文件。.list 结尾证明文件已经写完
                if basename[:3] == 'v0_':
                    # v0_ 开头是需要处理的文件
                    yield fullname
                    os.remove(fullname)
                else:
                    #logger.info("move [%s] to [%s]" % (fullname, restore_dir))
                    shutil.move(fullname, restore_dir)
                    #newname = os.path.join(restore_dir, basename)
                    #os.rename(fullname, newname)

