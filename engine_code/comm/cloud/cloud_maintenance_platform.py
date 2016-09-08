#!/usr/bin/env python
# -*- coding:utf-8 -*-

'''
@auther: wangkun
@date: Aug, 27, 2015
'''

import os, time, datetime
#from log.init_logger import init_logger

#logger = init_logger()
# #logger.error('This is debug message')

def main( path = "/data/gms_host/mtx/up" ):
    # 生成需要的各种参数条件 
    todayTime = int(time.strftime('%Y%m%d') + "24")
    yesterday = datetime.date.today() - datetime.timedelta(days=1)
    yesterdayTime = int(yesterday.strftime('%Y%m%d') + "08")
    #logger.info( '==========yesterdayTime: %s===todayTime: %s===============' % (yesterdayTime, todayTime) )

    # 目录及文件列表获取 
    file_name_list = [i for i in os.listdir(path) \
                        if ".ok" in i and "_" in i  \
                        and (yesterdayTime <= int(i[:10]) <= todayTime)  ]
    path_name =list(set([ i.split("_")[1].split(".")[0] for i in file_name_list]))
    #logger.info('path_name: %s===create file\'s time: %s' % (path_name, int(i[:9])))
    #logger.debug(file_name_list)
    
    # 汇总生成文件
    try:
        for i in path_name:
            mk_dir = path + "/" + i + "/"
            if not os.path.exists(mk_dir) :
                os.makedirs(mk_dir ) 
            path_list = [path + "/" + j for j in file_name_list if i in j]
            with open(mk_dir + time.strftime('%Y%m%d%H%M%S') + "_" + i + ".ok", "a") as fp :
                #logger.info(path_list)
                for k in path_list:
                    with open(k) as kfp:
                        data = kfp.read()
                    #logger.debug('k: %s' % k )
                    fp.write(data)
                    os.remove(k)
    except Exception, ex:
        #logger.error(ex)
        result = False
    else:
        result = True
    return result

if __name__ == "__main__": 
    main()
    main("/data/gms_host/virus")
