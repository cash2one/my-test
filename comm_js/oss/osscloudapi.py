#!/usr/bin/python
#coding:utf-8
import sys
import time
sys.path.append("../")
from oss.oss_api import *
from oss.oss_util import *

class OssAPI2(OssAPI):
    RATE_FACTOR = 5
    HEADER_SIZE = 0

    def get_object_range(self, bucket, object, first_pos='', last_pos=''):
        return self.get_object(bucket, object, headers={'Range': 'bytes=%s-%s' % (first_pos, last_pos)})

    def get_object_to_file2(self, bucket, object, object_size, fname, rate):
        first_pos = 0
        block_size = int(rate) * int(self.RATE_FACTOR) - int(self.HEADER_SIZE)
        fp = open(fname, 'wb')
        while object_size > 0:
            if object_size > block_size:
                last_pos = first_pos + block_size - 1
            else:
                last_pos = first_pos + object_size - 1
            begin_time = time.time()
            res = self.get_object_range(bucket, object, first_pos, last_pos)
            if (res.status / 100) == 2:
                data = res.read()
                fp.write(data)

                first_pos = last_pos + 1
                recv_size = len(data)
                object_size -= recv_size

                sleep_time = int(recv_size) / int(rate) - (time.time() - begin_time)
                if sleep_time > 0.001:
                    time.sleep(sleep_time)
                print '[%s - %s] GET: %s bytes/s' % (begin_time, time.time(), recv_size/(time.time()-begin_time))
                return res
            else:
                fp.close()
                os.remove(fname)
                return res
        fp.close()
        #return res 

    def put_object_from_fp2(self, bucket, object, fp, content_type, rate):
        fp.seek(os.SEEK_SET, os.SEEK_END)
        filesize = fp.tell()
        fp.seek(os.SEEK_SET)

        method = 'PUT'
        tmp_object = object
        conn = self._open_conn_to_put_object(method, bucket, object, filesize, content_type)

        send_buf_size = int(rate) * int(self.RATE_FACTOR) - int(self.HEADER_SIZE)
        l = fp.read(send_buf_size)
        retry_times = 0
        while len(l) > 0:
            send_size = len(l)
            try:
                begin_time = time.time()
                conn.send(l)
                retry_times = 0
            except:
                retry_times += 1
                if retry_times > 10:
                    print "retry too many times"
                    raise
            else:
                l = fp.read(send_buf_size)
            finally:
                sleep_time = (int(send_size) / int(rate)) - (time.time() - begin_time)
                if sleep_time > 0.001:
                    time.sleep(sleep_time)
                print '[%s - %s] PUT: (%s bytes/s)' % (begin_time, time.time(), send_size/(time.time()-begin_time))
        res = conn.getresponse()
        if check_redirect(res):
            self.host = helper_get_host_from_resp(res, bucket)
            return self.put_object_from_fp2(bucket, tmp_object, fp, content_type, rate)
        else:
            return res

    def put_object_from_file2(self, bucket, object, filename, rate):
        content_type = get_content_type_by_filename(filename)

        fp = open(filename, 'rb')
        res = self.put_object_from_fp2(bucket, object, fp, content_type, rate)
        fp.close()
        return res
#if __name__ == '__main__':
#    oss = OssAPI2("oss.aliyuncs.com","ZnY7oVJVSvQIdztK","CBYbwqpWpMqOk8jIecVANAHec5m5bA")
#
#    #res = oss.put_object_from_file2('oss-uuid-record', 'xxx.zzz', 'xxx.zzz.up', 1024)
#    #print res
#
#    res = oss.get_object_to_file2('oss-uuid-record', 'xxx.zzz', os.path.getsize('xxx.zzz.up'), 'xxx.zzz.down', 1024)
#    print res
#
#    #res = oss.get_bucket('oss-uuid-record', '')
#    #if (res.status / 100) == 2:
#    #    body = res.read()
#    #    print body
#
