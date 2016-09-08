# _*_ coding: utf8 _*_

import os
from transform.common import fwritelines
from vds_logger import logger


def VdsFileSave(fname, data_list):
    try:
        #logger.info("%s %s" %  (fname, data_list))
        if os.path.exists(fname):
            # 这个文件名不应该存在，如果现在的处理有问题，应该调整流程
            cnt = 1
            oldname = fname
            basename, extname = os.path.splitext(fname)
            while True:
                newname = "%s_(%d)%s" % (basename, cnt, extname)
                if os.path.exists(newname):
                    cnt += 1
                    continue
                else:
                    fname = newname
                    break
            logger.info("**** fname[%s] already exist. rename[%s] ****" % (oldname, fname))

        fwritelines(fname, data_list)
    except IOError as reason:
        logger.info("**** save result file[%s] fail. %s. ****" % (fname, str(reason)))
        # 最大可能是没有权限。传出异常，应该会在 StmtsList.__exit__ 中处理异常，备份文件。
        # 不过这时很可能备份也没有权限
        raise

