#!/usr/bin/python
#encoding=utf-8
import sys
import re
import os
import glob
import time
import traceback

start = time.clock()

def tsplit(string, delimiters):
    delimiters = tuple(delimiters)
    stack = [string, ]

    for delimiter in delimiters:
        for i, substring in enumerate(stack):
            substack = substring.split(delimiter)
            stack.pop(i)
            for j, _substring in enumerate(substack):
                stack.insert(i + j, _substring)
    return stack

file_name_all = []
def fun_dir(path):
    global file_name_all
    for file_name in glob.glob(path + os.sep + '*'):
        if os.path.isdir(file_name):
            fun_dir(file_name)
        else:
            if re.findall(r'^d20*', os.path.basename(file_name)):
                file_name_all.append(file_name)

file_sqlname = sys.argv[1]
file_sql = open(file_sqlname)
try:
    query_sql = file_sql.read()
    file_sql.close()
finally:
    file_sql.close()

query_sqltemp = re.split('where', query_sql)
query_field = tsplit(query_sqltemp[0], ('select', ','))
query_condition = tsplit(query_sqltemp[1], ('and', ','))

def make_result_fname(result_dname, sql_fname):
    tm, uuid = os.path.splitext(os.path.basename(file_sqlname))[0].split('_')
    return os.path.join(result_dname, "%s_%s.ok" % (tm, uuid))

file_resultname = make_result_fname('/data/tmpdata/comm/query/dns/up/', file_sqlname)
tmp_file_resultname = file_resultname + '.tmp'
file_result = open(tmp_file_resultname, 'w+')

try:
    type_value = ""
    timenow_lower = 0
    timenow_upper = 10**15
    for condition in query_condition:
        if "type" in condition:
            type_value = condition.split('\"')[1]
        if "timenow>" in condition:
            timenow_lower = condition.split('>')[1]
            if timenow_lower[-1] == ' ':
                timenow_lower = timenow_lower[:-1]
            timenow_lower = int(timenow_lower)
        if "timenow<" in condition:
            timenow_upper = condition.split('<')[1]
            if timenow_upper[-1] == ' ':
                timenow_upper = timenow_upper[:-1]
            timenow_upper = int(timenow_upper)
    
    fun_dir("/data/permdata/dns/")
    
    for file_name in file_name_all:
        file_name_type = os.path.basename(os.path.splitext(file_name)[1])
        file_name_type = file_name_type[1:]
        file_name_time = os.path.basename(os.path.splitext(file_name)[0])
        file_name_time = int(file_name_time[1:15])
        open_condition_flag = True
    
        type_value = ""
        for condition in query_condition:
            if "type" in condition:
                type_value = condition.split('\"')[1]
        if type_value == "":
            type_value = file_name_type
        elif type_value == file_name_type:
            pass
        else:
           open_condition_flag = False
        if timenow_lower - 10000 < file_name_time and file_name_time < timenow_upper + 10000:
            pass
        else:
            open_condition_flag = False
        if open_condition_flag == False:
            continue
        else:
            file_dnsname = file_name
            file_dns = open(file_dnsname)
    
            for line in file_dns:
                list = line.split('\t')
                record = []
                for word in list:
                    if word[-1] == '\n':
                        record.append(word[:-1])
                    else:
                        record.append(word)
    
                query_field_number = []
                for word in query_field:
                    if "req_dn" in word:
                        query_field_number.append(4)
                    elif "resp_data" in word:
                        query_field_number.append(5)
                    elif "type" in word:
                        query_field_number.append(-1)
                    elif "dip" in word:
                        query_field_number.append(9)
                    elif "timenow" in word:
                        query_field_number.append(10)
                    elif "*" in word:
                        query_field_number.append(-2)
    
                query_field_len = len(query_field_number)
                query_field_count = 1
                result = ""
                for number in query_field_number:
                    if number == -1:
                        result = type_value
                    elif number == -2:
                        result = record[4] + '\t' + record[5] + '\t' + type_value + '\t' + record[9] + '\t'
                        for date in tsplit(record[10], ('-', ' ', ':')):
                            result += date
                    elif number == 10:
                        for date in tsplit(record[number], ('-', ' ', ':')):
                            result += date
                    else:
                        result += record[number]
                    if query_field_count < query_field_len:
                        result += '\t'
                        query_field_count += 1
                    else:
                        result += '\n'
    
                write_file_flag = True
                for condition in query_condition:
                    if "req_dn" in condition:
                        value = condition.split('\"')[1]
                        if record[4] == value:
                            pass
                        else:
                            write_file_flag = False
                            break
                    if "resp_data" in condition:
                        value = condition.split('\"')[1]
                        if record[5] == value:
                            pass
                        else:
                            write_file_flag = False
                            break
                    if "dip" in condition:
                        value = condition.split('\"')[1]
                        if record[9] == value:
                            pass
                        else:
                            write_file_flag = False
                            break
                    if "timenow" in condition:
                        timenow = ""
                        for date in tsplit(record[10], ('-', ' ', ':')):
                            timenow += date
                        timenow = int(timenow)
                        if timenow_lower < timenow and timenow < timenow_upper:
                            pass
                        else:
                            write_file_flag = False
                            break
                if write_file_flag:
                    file_result.write(result)
            file_dns.close()
except:
    print "SQL stmt error. %s" % traceback.format_exc()
    file_result.write(traceback.format_exc())

file_result.close()

os.rename(tmp_file_resultname, file_resultname)
count_time = (time.clock() - start)
print "Time used:",count_time
