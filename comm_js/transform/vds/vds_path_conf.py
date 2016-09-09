# _*_ coding: utf8 _*_

from transform import common
from vds_logger import logger

# vds_path_config
input_dir = common.trans_data_ve_path   #/data/tmpdata/engine/virus/
output_dir = common.comm_ve_up_path     #/data/tmpdata/comm/virus/up/
backup_dir = common.trans_backup_ve_path#/data/permdata/wrong/trans/ve/
restore_dir = common.vds_restore_path   #/data/permdata/vds/

def vds_path_check():
    logger.info("input dir = %s" % input_dir)
    logger.info("output dir = %s" % output_dir)
    logger.info("backup dir = %s" % backup_dir)
    logger.info("restore dir = %s" % restore_dir)
    common.creat_if_dir_notexist(input_dir)
    common.creat_if_dir_notexist(output_dir)
    common.creat_if_dir_notexist(backup_dir)
    common.creat_if_dir_notexist(restore_dir)

