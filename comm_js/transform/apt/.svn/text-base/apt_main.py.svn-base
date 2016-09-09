# _*_ coding: utf8 _*_

import time
import traceback
import apt_exec_info as exec_info
import apt_path_conf as path_conf
from transform import common
from apt_logger import logger
from apt_fmttrans import AptFormatTransform

def main():
    try:
        logger.info("========== apt transform thread start. ==========")
        path_conf.apt_path_check()
        fmt_trans = AptFormatTransform()
        while common.is_continue():
            # files_transform 中发生的异常会被 AptFileList 中的 __exit__ 拦截处理
            fmt_trans.files_transform()
            exec_info.cyclicaldump()
            time.sleep(0.1)
    except Exception:
        exec_info.dump()
        logger.info("*** catch unexpected ERROR. exit. ***")
        logger.info(traceback.format_exc())
        raise

if __name__ == "__main__":
    main()

