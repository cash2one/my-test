# _*_ coding: utf8 _*_

import time
from vds_logger import logger

total_file_cnt = 0
total_line_cnt = 0
error_file_cnt = 0
error_line_cnt = 0
null_file_cnt = 0
seconds = 0

def inc_file_cnt():
    global total_file_cnt
    total_file_cnt += 1

def inc_line_cnt():
    global total_line_cnt
    total_line_cnt += 1

def inc_error_file_cnt():
    global error_file_cnt
    error_file_cnt += 1

def inc_error_line_cnt():
    global error_line_cnt
    error_line_cnt += 1

def inc_null_file_cnt():
    global null_file_cnt
    null_file_cnt += 1

def dump():
    logger.info("---------- vds transform info statistic ----------")
    logger.info("process vds file cnt = %d" % total_file_cnt)
    logger.info("process vds line cnt = %d" % total_line_cnt)
    if error_file_cnt > 0:
        logger.info("error vds file cnt = %d" % error_file_cnt)
    if error_line_cnt > 0:
        logger.info("error vds line cnt = %d" % error_line_cnt)
    if null_file_cnt > 0:
        logger.info("null vds file cnt = %d" % null_file_cnt)
    logger.info("process [%d] files spend [%d] seconds." %
                (total_file_cnt, seconds))

last_sec = 0
def cyclicaldump(wait_sec = 60):
    global last_sec
    cur_sec = time.time()
    if cur_sec - last_sec > wait_sec:
        dump()
        last_sec = cur_sec

