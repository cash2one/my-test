# _*_ coding: utf8 _*_

import time
import traceback
import vds_exec_info as exec_info
import vds_path_conf as path_conf
from transform import common
from vds_logger import logger
from vds_fmttrans import VdsFormatTransform

def main():
    try:
        logger.info("========== vds transform thread start. ==========")
        path_conf.vds_path_check()
        fmt_trans = VdsFormatTransform()
        while common.is_continue():
            # files_transform 中发生的异常会被 VdsFileList 中的 __exit__ 拦截处理
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

