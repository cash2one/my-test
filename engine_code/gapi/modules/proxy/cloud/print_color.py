#!/usr/bin/python
# _*_coding:utf8 _*_

import os

def inBlack(s):  
    return highlight('') + "%s[30;2m%s%s[0m"%(chr(27), s, chr(27))   

def inRed(s):  
    return highlight('') + "%s[31;2m%s%s[0m"%(chr(27), s, chr(27))  

def inGreen(s):  
    return highlight('') + "%s[32;2m%s%s[0m"%(chr(27), s, chr(27))   

def inYellow(s):  
    return highlight('') + "%s[33;2m%s%s[0m"%(chr(27), s, chr(27))   

def inBlue(s):  
    return highlight('') + "%s[34;2m%s%s[0m"%(chr(27), s, chr(27))   

def inPurple(s):  
    return highlight('') + "%s[35;2m%s%s[0m"%(chr(27), s, chr(27))   

def inWhite(s):  
    return highlight('') + "%s[37;2m%s%s[0m"%(chr(27), s, chr(27))  

def highlight(s):  
    return "%s[30;2m%s%s[1m"%(chr(27), s, chr(27))



def cloud_file_to_color(fname):
    return inBlue(fname)

def local_file_to_color(fname):
    return inYellow(fname)

def print_delete(cloud_file, result):
    
    title = inPurple("DELETE")

    c_file = cloud_file_to_color(cloud_file)

    if (result == "FAIL"):
        res = inRed(result) + "!!!!"
    else:
        res = result + "."

    print title + " " + "CLOUD[" + c_file + "] " + res
def print_delete_local(local_file):
    
    title = inPurple("DELETE")

    c_file = cloud_file_to_color(local_file)

    print title + " " + "REPORT LOCAL[" + c_file + "] "

def print_copy(src_file, dst_file):
    
    title = inGreen("COPY")

    c_file = cloud_file_to_color(src_file)

    f_file = local_file_to_color(dst_file)

    print title + " " + "CLOUD[" + c_file + "] ----> FILE[" + f_file + "] ..."
def print_creat(src_file):
    
    title = "CREAT"

    c_file = cloud_file_to_color(src_file)

    #f_file = local_file_to_color(dst_file)

    print title + " " + "CLOUD DIR[" + c_file + "] SUCCEED!!! ..."

def print_downloading(cloud_file, local_file):
    
    title = "Downloading"

    c_file = cloud_file_to_color(cloud_file)

    f_file = local_file_to_color(local_file)

    print title + " " + "CLOUD[" + c_file + "] -> FILE[" + f_file + "] ..."
def print_upload(local_file, cloud_file, result):
    
    title = inGreen("UP")

    c_file = cloud_file_to_color(cloud_file)

    f_file = local_file_to_color(local_file)

    if (result == "FAIL"):
        res = inRed(result) + "!!!!"
    else:
        res = result + "."

    print title + " " + "FILE[" + f_file + "] -> CLOUD[" + c_file + "] " + res

def print_download(cloud_file, local_file, result):
    
    title = inPurple("DOWN")

    c_file = cloud_file_to_color(cloud_file)

    f_file = local_file_to_color(local_file)

    if (result == "FAIL"):
        res = inRed(result) + "!!!!"
    else:
        res = result + "."

    print title + " " + "CLOUD[" + c_file + "] -> FILE[" + f_file + "] " + res

