# -*- coding:utf-8 -*-
import os
import sys
import time
import logging
import threading

import comm_log_record


class CloudLogger(comm_log_record.FileLogger):
    def logfiledirname(self, timestamp):
        return os.path.join(self.path, time.strftime("%Y/%m", time.localtime(timestamp)))

    def logfilebasename(self, timestamp):
        return 'cloud-' + time.strftime("%d", time.localtime(timestamp)) + '.log'


if __name__ == '__main__':
    import time
    import threading
    logger = CloudLogger(path = './log',
                        level = '',
                        format = '==%(asctime)s %(module)s %(funcName)s[%(lineno)d]== %(message)s')
    # 这里 module 不会被显示
    def testlogger1(tag):
        while True:
            logger.debug("[%s] wahahahaha" % tag)
            time.sleep(1)

    def testlogger2(tag):
        while True:
            logger.debug("[%s] wakakakakakakaka" % tag)
            time.sleep(1)

    thread = threading.Thread(target=testlogger1, args=("test1",))
    thread.setDaemon(True)
    thread.start()
    thread = threading.Thread(target=testlogger2, args=("test2",))
    thread.setDaemon(True)
    thread.start()

    while True:
        time.sleep(1)
