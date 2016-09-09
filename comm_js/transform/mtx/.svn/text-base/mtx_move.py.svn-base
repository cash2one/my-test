#!/usr/bin/env python
# _*_ coding: utf8 _*_

# 运行在检测结点。负责将 mtx 引擎生成的文件拷贝到通讯目录下

import os
import sys
import time
import syslog
import shutil
import json
import socket
import struct
import traceback
import comm_data
from transform import common

# 配置选项
src_dir = comm_data.trans_data_me_path
dst_dir = comm_data.comm_mtd_up_path

# 执行统计信息
mtd_file_count = 0
null_mtd_file_count = 0

logger = common.logger

def check_config():
    logger.info("mtd src dir = %s" % src_dir)
    logger.info("mtd dst dir = %s" % dst_dir)
    common.creat_if_dir_notexist(src_dir)
    common.creat_if_dir_notexist(dst_dir)

last_sec = 0
def exec_info_dump(wait_sec = 60):
    global last_sec
    cur_sec = time.time()
    if cur_sec - last_sec > wait_sec:
        last_sec = cur_sec
        logger.info("---------- mtx move info statistic ----------")
        logger.info("mtd file count = %d" % mtd_file_count)
        logger.info("null mtd file count = %d" % null_mtd_file_count)

def mtx_move():
    global mtd_file_count
    global null_mtd_file_count
    try:
        logger.info("========== mtx move thread start. ==========")
        check_config()
        cmd = 'lsof +d "%s" |grep -v "COMMAND"|awk \'{if ("REG"==$5) print $9}\'' % src_dir
        while common.is_continue():
            # 取得当前目录下已经打开的文件
            try:
                all_file_list = common.file_list(src_dir)
                open_file_list = map(str.strip, os.popen(cmd).readlines())
                #print("opening file[%s]" % open_file_list)
                for file in all_file_list:
                    if not file in open_file_list:
                        extname = os.path.splitext(file)[-1]
                        if extname == '.mtd':
                            #print("process file[%s]" % file)
                            mtd_file_count += 1
                            if os.path.getsize(file) == 0:
                                # 有这么一个情况。生成的 3000+ 个文件，有 3000 个空文件，只有十几个有数据。
                                # 当时怀疑是 mtd 先 open 一个文件，然后 close。再然后往里边追加内容。
                                # 于是作了这个判断，只有文件生成超过 20 分钟，而且还是空的，才认为是一个空文件。
                                # 后来发现似乎和打的包相关。添加这个判断证明 mtx 确实有空文件生成。
                                # 文件格式: /data/tmpdata/engine/mtd/20141015-094640-20000209.mtd
                                try:
                                    basename = os.path.basename(file)
                                    date = os.path.splitext(basename)[0]
                                    ctime = time.mktime(time.strptime(date[:date.rfind('-')], '%Y%m%d-%H%M%S'))
                                    # 假如已经过了一个小时，文件还是空。就认为是个空文件。
                                    if time.time() - ctime > 1200:
                                        # 空文件删掉不上传。否则国家主机上的博雅程序会挂掉
                                        null_mtd_file_count += 1
                                        os.remove(file)
                                    else:
                                        pass
                                except:
                                    logger.info('%s name fmt error. backup.' % file)
                                    shutil.move(file, file + '.bak')
                            else:
                                # 把 .mtd 后缀的文件拷贝到目标目录，然后再加上 .ok 后缀
                                tmp = file.split("-")
                                tmp_name = tmp[0] + "-" + tmp[1] + "_" + comm_data.uuid + '.mtd'
                                dstname = os.path.join(dst_dir, os.path.basename(tmp_name))
                                shutil.copy(file, dstname)
                                os.rename(dstname, dstname + '.ok')
                                os.remove(file)
                                # 如果在删除之前挂掉，下次还会再打包这个文件

                time.sleep(0.5)
                exec_info_dump()
            except Exception as reason:
                logger.info(traceback.format_exc())
    except Exception as reason:
        logger.info(traceback.format_exc())
    finally:
        exec_info_dump()
        logger.info("========== mtx move thread stop. ==========")

if __name__ == '__main__':
    mtx_move()

