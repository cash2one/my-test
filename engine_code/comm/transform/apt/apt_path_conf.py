# _*_ coding: utf8 _*_

import sys
import transform.common as common
from apt_logger import logger

# apt_path_config
input_dirs = [common.trans_data_ae_online_path, common.trans_data_ae_offline_path]
output_dir = common.comm_ae_up_path
backup_dir = common.trans_backup_ae_path

def apt_path_check():
    logger.info("input dirs = %s" % input_dirs)
    logger.info("output dir = %s" % output_dir)
    logger.info("backup dir = %s" % backup_dir)
    for input_dir in input_dirs:
        common.creat_if_dir_notexist(input_dir)
    common.creat_if_dir_notexist(output_dir)
    common.creat_if_dir_notexist(backup_dir)

