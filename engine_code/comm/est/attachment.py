#!/usr/bin/env python
#-*- coding:utf-8 -*-

'''
@auther: wangkun
@date: Aug, 17, 2015
'''

import os,time,socket,struct, shutil

EVENTTYPEDICT = {
    '1': u'木马',
    '1': u'僵尸网络',
    '2': u'网站事件',
    '3': u'攻击尝试事件',
    } 
MTXEVENT = {
             "event1": ' 事件类型,事件名称,开始时间,结束时间,源IP,源端口,目的IP,目的端口,URL\n  ',
             "event3": ' 事件类型,事件名称,开始时间,结束时间,源IP,源端口,目的IP,目的端口,URL\n  ',
           }
AlarmEventPath = os.path.abspath("/data/tmpdata/comm/alarm_event/email")

def transform(data):
    try:
        if isinstance(data, unicode):
            return data.encode('gb2312')
        else:  
            return data.decode('utf-8').encode('gb2312')
    except Exception, ex :
        return data

class EmailConTemplate(object):
    def __init__(self):
        pass
    # 获取mtx安全事件目录下的文件列表
    def file_list(self, dir, postfix):
        file_final_dict = {}
        for file_tuple in os.walk(dir):
            file_final_dict.update({file_tuple[0]:[ filename for filename in file_tuple[2] \
                                    if os.path.splitext(filename)[1] == postfix]})
        return file_final_dict

    # 处理数据以便写入模板 
    def data_temp(self, data):
        data_list = data.split("|")
        if len(data_list) >= 15:
          try:
            data_str = u"恶意代码传播事件" + ','+ ",".join([data_list[i] for i in [ 4, 1]]) + ","
            data_other = "," + socket.inet_ntoa(struct.pack("i", socket.htonl(long(data_list[7])))) \
                      + "," + data_list[9] \
                      + "," + socket.inet_ntoa(struct.pack("i", socket.htonl(long(data_list[6])))) \
                      + "," + data_list[8] \
                      + "," + "" + "\n"
            data_str += data_other
            data_str = transform(data_str)
            if "time_start" < locals().keys():
                time_start = min(time_start, data_list[1])
            else:
                time_start = data_list[1]
          except :
            # data_test = time.strptime(data_list[5], "%Y-%m-%d %H:%M:%S")
            data_str = EVENTTYPEDICT.get(data_list[4], u"其他事件") + ',' + ",".join([data_list[i] for i in [ 10, 6, 7]]) 
            data_other = "," + socket.inet_ntoa(struct.pack("i", socket.htonl(long(data_list[11])))) \
                      + "," + data_list[13] \
                      + "," + socket.inet_ntoa(struct.pack("i", socket.htonl(long(data_list[12])))) \
                      + "," + data_list[14] \
                      + "," + "|".join(data_list[15:-5]) + "\n"
            data_str += data_other
            data_str = transform(data_str)
            if "time_start" < locals().keys():
                time_start = min(time_start, data_list[6])
            else:
                time_start = data_list[6]
                
        else:
            data_str = transform(data)
        return (data_str,time_start)

    def data_temp3(self, data_line):
        return data_line

    # 合并数据并写入同一个文件中，以便作为附件发送给客户 
    def merge_file(self, src_file_dict, final_file, eve_arg = "event1"):
        with open(final_file, "a+") as finalF :
            if src_file_dict :
                finalF.write(transform(MTXEVENT[eve_arg]))
            for src_dir, src_file_list in src_file_dict.items() :
                for src_file in src_file_list : 
                    tmp_file = src_dir + "/" +src_file
                    with open(tmp_file) as srcF :
                        data_src = srcF.readlines()
                    for data_line in data_src :
                        if eve_arg == "event1" :
                            data_line = self.data_temp(data_line)
                        elif eve_arg == "event3" :
                            data_line = self.data_temp(data_line)
                        finalF.write(data_line[0])
                        if "time_start_data" in locals().keys():
                            time_start_data = min(time_start_data, data_line[1])
                        else:
                            time_start_data = data_line[1]
                                        
                    if eve_arg == "event1" :
                        shutil.move( tmp_file ,AlarmEventPath + "/" + src_file )
                    else:
                        os.remove(tmp_file)
        if os.path.exists(final_file) and os.path.getsize(final_file) :
            final_path, finale_file_name = os.path.split(final_file)
            result_file = final_path + "/event_" \
                        + time.strftime("%Y%m%d%H%M%S", time.strptime(time_start_data, "%Y-%m-%d %H:%M:%S")) \
                        + "-" + time.strftime( "%Y%m%d%H%M%S", time.localtime())+".csv"
            os.renames(final_file, result_file )
            return (result_file, time_start_data)
        else:
           return False

if __name__ == "__main__":
    opera = EmailConTemplate()
    file_dict = opera.file_list("/root/test", ".ok")
    print opera.merge_file(file_dict, "/root/aaa.csv")
    print os.path.getsize("/root/aaa.csv")
