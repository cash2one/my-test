# -*- coding:utf-8 -*-
import os
import sys
import time
import logging
import threading

class FileLogger():
    ''' 根据日期写日志文件。
        path 参数为日志的根目录，下面的目录结构为 yyyy/mm/dd/yyyymmdd.log 形式。
        如果需要修改 目录 和 文件名可以重载 logfiledirname 和 logfilebasename 函数。
        level 和 format 和 logging 的参数格式相同。 '''
    def __init__(self, level=logging.NOTSET, format=None, path='.'):
        self.locker = threading.Lock()
        self.format = format
        self.level = level
        self.path = path
        self.loggerinit()

    def loggerinit(self):
        # 创建日志文件目录
        # 初始化 logger formatter handler
        # 其实 formatter logger 可以重用
        self.logtime = time.time()
        logfile = self.logfileinit(self.logtime)
        handler = logging.FileHandler(logfile)

        formatter = logging.Formatter(self.format)
        handler.setFormatter(formatter)

        self.logger = logging.Logger('')
        self.logger.addHandler(handler)
        self.logger.setLevel(self.level)

    def loggerswitch(self):
        self.loggerinit()

    def loggerexpired(self):
        curdate = int(time.strftime("%Y%m%d", time.localtime()))
        logdate = int(time.strftime("%Y%m%d", time.localtime(self.logtime)))
        # 没有使用 > 号。这样即使在执行中系统日期被向前调也不影响日志
        if curdate == logdate:
            return False
        else:
            return True

    def logfileinit(self, timestamp):
        dirname = self.logfiledirname(timestamp)
        if not os.path.exists(dirname):
            os.makedirs(dirname)

        basename = self.logfilebasename(timestamp)
        filename = os.path.join(dirname, basename)
        return filename

    def logfiledirname(self, timestamp):
        return os.path.join(self.path, time.strftime("%Y/%m", time.localtime(timestamp)))

    def logfilebasename(self, timestamp):
        return 'engine-' + time.strftime("%d", time.localtime(timestamp)) + '.log'

    def logrecord(self, record):
        with self.locker:
            if self.loggerexpired():
                self.loggerswitch()
            self.logger.handle(record)

    def logmessage(self, msg):
        logleveldict = {'error': logging.ERROR,
                        'warn': logging.WARN,
                        'info': logging.INFO,
                        'debug': logging.DEBUG,}

        funcName = sys._getframe().f_back.f_back.f_code.co_name
        lineno = sys._getframe().f_back.f_back.f_lineno
        loglevel = logleveldict[sys._getframe().f_back.f_code.co_name]
        record = logging.LogRecord(name='',
                                    pathname='',
                                    level=loglevel,
                                    func=funcName,
                                    lineno=lineno,
                                    msg=msg,
                                    args=None,
                                    exc_info=None,)
        self.logrecord(record)

    def error(self, msg):
        self.logmessage(msg)

    def warn(self, msg):
        self.logmessage(msg)

    def info(self, msg):
        self.logmessage(msg)

    def debug(self, msg):
        self.logmessage(msg)

logger = None
def init_log(log_path):
    global logger
    logger = FileLogger(path = log_path,
            format = '%(asctime)s *%(levelname)s* [%(funcName)s:%(lineno)d]: %(message)s')

if __name__ == '__main__':
    import time
    import threading
    logger = FileLogger(path = '/data/log/engine/',
                        format = '==%(asctime)s %(threadName)s [%(funcName)s:%(lineno)d]== %(message)s')

    def testlogger1(tag=''):
        while True:
            logger.debug("[%s] wahahahaha" % tag)
            time.sleep(1)

    def testlogger2(tag=''):
        while True:
            logger.debug("[%s] wakakakakakakaka" % tag)
            time.sleep(1)

    import comm_common
    comm_common.create_thread(func=testlogger1, name='test-thread-1')#,args=('tag1',))
    comm_common.create_thread(func=testlogger2, name='test-thread-2')#,args=('tag2',))

    while True:
        time.sleep(1)

