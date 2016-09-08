#!/usr/bin/env python
# -*- coding:utf-8 -*-

import os
import logging
import logging.config

# 创建日志目录 
def creat_if_dir_notexist(dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
    return True

# 初始化日志系统 
def init_logger(args="initLoger"):
    #initLogger, debugGMS
    dir = "/data/log/gapi/"
    creat_if_dir_notexist(dir)
    logging.config.fileConfig(os.path.abspath("/gms/gapi/modules/est/log/logger.conf"))
    logger = logging.getLogger(args)
    return logger

if __name__ == "__main__":
    logger = init_logger()
    logger.debug('This is debug message')
    logger.info('This is info message')
    logger.warning('This is warning message')
    logger.error('This is error message')
