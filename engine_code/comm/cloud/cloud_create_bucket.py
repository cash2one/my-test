#!/usr/bin/python
# _*_ coding:utf8 _*_


import time
import signal
import sys
import os
sys.path.append("../")
from oss.oss_api import *
import comm_data
import comm_init

if __name__ == "__main__":

    comm_init.read_pre_json()
# oss.create_bucket("test-gms-mtx-up", "private")
    print comm_data.OSS_ALIYUNCS_COM
    print comm_data.OSS_ID
    print comm_data.OSS_KEY

    oss = OssAPI(str(comm_data.OSS_ALIYUNCS_COM), str(comm_data.OSS_ID), str(comm_data.OSS_KEY))

    res = oss.create_bucket(str(comm_data.MTX_UP_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.MTX_UP_BUCKET)
    else:
        print res.status, res.read()

    res = oss.create_bucket(str(comm_data.MTX_DOWN_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.MTX_DOWN_BUCKET)
    else:
        print res.status, res.read()

    res = oss.create_bucket(str(comm_data.REG_DOWN_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.REG_DOWN_BUCKET)
    else:
        print res.status, res.read()

    res = oss.create_bucket(str(comm_data.REG_UP_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.REG_UP_BUCKET)
    else:
        print res.status, res.read()

    res = oss.create_bucket(str(comm_data.CONF_UP_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.CONF_UP_BUCKET)
    else:
        print res.status, res.read()

    res = oss.create_bucket(str(comm_data.CONF_DOWN_BUCKET), "private")
    if (res.status == 200):
        print "create %s SUCC." %(comm_data.CONF_DOWN_BUCKET)
    else:
        print res.status, res.read()


    res = oss.list_all_my_buckets()
    print res.status, res.read()


