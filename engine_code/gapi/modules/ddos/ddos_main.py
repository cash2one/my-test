#!/usr/bin/python

'''
@auther: wangkun
@date: September, 01, 2015
'''

import os, sys, getopt, time
import init_conf
sys.path.append("/gms/gapi/modules/est/")
from log.init_logger import init_logger
logger = init_logger()
#logger.debug('This is debug message')
PATH = os.path.abspath("/gms/tcad/ddos/ddos.conf")

def call_shell(args):
    count = 0
    while 1 :
        count += 1
        result = os.system(args)
        if not result or count == 3 :
            print args
            logger.info(args)
            break
        logger.error("shell command line command error:%d" % result)
        time.sleep(3)

def preprocess(path, tags = 0 ):
    try:
        result = True
        with open(path, "r") as fp :
            input = fp.readlines()
        if not tags :
            note1, note2 = "; aaa = ", "#"
            logger.info("pre:%s" % "src file-->tmp file")
        else:
            note1, note2 = "#", "; aaa = "
            logger.info("pre:%s" % "tmp file = --> src file")
        output = []
        for i in input:
            if i.startswith(note2):
                i.replace(note2, note1)
            else:
                pass
            output.append(i)
        #input = input.replace(note3, note4)
        with open(path, "w") as fp:
            for i in output:
                fp.write(i)
    except Exception, ex:
        logger.error("preprocess error:%s" % str(ex))
        result = Fales
    finally:
        return result

def main():
    try:
        result = 0
        opts,args = getopt.getopt(sys.argv[1:],"o:a:d:l:",["in_flow=", "in_new_conn=", "in_live_conn=", "out_flow=", "out_live_conn=", "out_new_conn="])
        opts_dict = dict(opts)
        with open(PATH, "r") as fp :
            input = fp.readlines()
        #if not preprocess(PATH ):
        #    raise Exception(sys.exit(116))
        xml_opts = init_conf.XmlAction()
    	xml_opts.update("ddos_switch", "open", str(opts_dict["-o"]))
    	init_file = init_conf.OperationFile(PATH)
        init_file.modify("ddos", "switch", str(opts_dict["-o"]))
    	if str(opts_dict["-o"]) == "1":
    		xml_opts.update("ddos_switch", "self_learn", str(opts_dict["-l"]))
    		init_file.modify("base_conf", "self_learn_switch", str(opts_dict["-l"]))
    		if str(opts_dict["-a"]) != "null":
    			for i in opts_dict["-a"].split(";"):
    				xml_opts.create(i)
    				init_conf.ip_list_conf("c", i)
                        call_shell("/gms/tcad/monitor -r1")
    		if str(opts_dict["-d"]) != "null":
    			xml_opts.delete(str(opts_dict["-d"]))
    			init_conf.ip_list_conf("d", opts_dict["-d"])
                        call_shell("/gms/tcad/monitor -r1")
    		if str(opts_dict["-l"]) == "0":
                        ini_args_list = [
    			'flow_in", '     + str(int(float(opts_dict["--in_flow"])*1024*1024)),
    			'new_tcp_in", '  + opts_dict["--in_new_conn"],
    			'live_tcp_in", ' + opts_dict["--in_live_conn"],
    			'flow_out", '    + str(int(float(opts_dict["--out_flow"])*1024*1024)),
    			'new_tcp_out", ' + opts_dict["--out_new_conn"],
    			'live_tcp_out", '+ opts_dict["--out_live_conn"]
                        ]
                        for i in ini_args_list:
    				eval('init_file.modify("base_conf", "threshold_' + i + ')')
    
    			xml_args_list = [
                        '"in_threshold", "flow", "' + opts_dict["--in_flow"],
    			'"in_threshold", "tcp_new_conn", "' + opts_dict["--in_new_conn"],
    			'"in_threshold", "tcp_live_conn", "' + opts_dict["--in_live_conn"],
    			'"out_threshold", "flow", "' + opts_dict["--out_flow"],
    			'"out_threshold", "tcp_new_conn", "' + opts_dict["--out_new_conn"],
    			'"out_threshold", "tcp_live_conn", "' + opts_dict["--out_live_conn"]
                        ] 
    			for i in xml_args_list:
    				eval("xml_opts.update(" + i + '")')
        call_shell("/gms/tcad/monitor -r")
    	xml_opts.save()
        logger.debug(input)
        with open(PATH, "r") as fp :
            output = fp.readlines()
        logger.debug(output)
        for i in output:
            tmp_data = i.split("=")
            if len(tmp_data)==2:
                for j in input:
                    tmp_dataj = j.split("=")
                    if len(tmp_dataj) == 2 and tmp_data[0].strip() == tmp_dataj[0].strip():
                        input[input.index(j)] = i
                        break
        with open(PATH, "w") as fp :
            for i in input:
                fp.write(i)
        sys.exit(0)
    except Exception, ex:
        print ex
        logger.error(ex)
        sys.exit(116)
        result = 116
    finally:
        #preprocess(PATH , 1)
        return result

if __name__ == "__main__":
    exit(main())
