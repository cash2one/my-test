#!/usr/bin/env python
# -*- coding: UTF-8 -*-

'''
@auther: wangkun
@date: Sept, 15, 2015
'''

import email_object,getopt,sys, os
sys.path.append(os.path.abspath("/gms/gapi/modules/est/log/"))
from init_logger import init_logger              
logger = init_logger()

# -s == "1" 是升级；其他是重启 
def main():
    try:
        opts,args = getopt.getopt(sys.argv[1:], "s:o:n:", ["help",]);
        print opts
        opts =  dict(opts)
        if opts["-s"] == "1":
            args = u" 系统升级，从版本%s升级到版本%s，请知悉 " % (opts["-o"],opts["-n"])
        else:
            args = u" 系统重启，请知悉 "
        email_object.email_threshold(args)
        logger.info(opts)
    except Exception, ex:
        print ex
        logger.error(ex)
    finally:
        return True

if __name__ == "__main__" :
    exit(main())
