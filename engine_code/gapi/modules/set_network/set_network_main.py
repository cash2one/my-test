#!/usr/bin/python
# _*_ coding: utf8 _*_
import os
import sys
import subprocess
import getopt
import errno
import shlex
gapi_path='/gms/gapi/'
def exec_script(script, argv=[]):
    argv.insert(0, script)

    ext_map = {'.py': 'python', '.pyc': 'python'}
    ext_name = os.path.splitext(script)[-1]
    if ext_map.has_key(ext_name):
        argv.insert(0, ext_map[ext_name])

    p = subprocess.Popen(argv, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    sys.stdout.write('%s' % stdout)
    status = p.returncode
    if status != 0:
        #if VERBOSE: logger.logger.warn('exec script[%s] status[%s] != 0. %s.' % (script, status, stderr))
        print ('status[%s],error[%s]' % (status,stderr))
    return status

def set_speed(argv):
    script = os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_comm_daemon.pyc')
    return exec_script(script, argv)
def set_proxy(argv):
    script = os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_proxy.pyc')
    return exec_script(script, argv)
def set_ip(argv):
    script = os.path.join(gapi_path, 'modules', 'network', 'xml_att.pyc')
    return exec_script(script, argv)
gapi_dict = {
        'set_ip': set_ip,
        'cloud_proxy': set_proxy,
        'cloud_comm_daemon': set_speed,
        }
def exec_function(funcname,argv):
    try:
        function = gapi_dict[funcname]
    except KeyError:
        return 2
    else:
        try:
            ret = function(argv)
        except Exception as ex:
            if ex.errno == errno.EACCES: # 没有执行权限
                ret = 117
            else:
                ret = 116
        return ret




def main():
    opts, args = getopt.getopt(sys.argv[1:], '', ['set_ip=', 'cloud_comm_daemon=', 'cloud_proxy=', 'force'])
    if args != []:
        return 1
    set_ip = cloud_deamon = cloud_proxy = None
    force = False
    argv = []
    for opt, arg in opts:
        if opt == '--set_ip':
            if arg != 'None':
                set_ip = shlex.split(arg)
                #print set_ip
                ret=exec_function("set_ip", set_ip)
                if ret != 0:
                    return ret;
        elif opt == '--cloud_comm_daemon':
            if arg != 'None':
                cloud_daemon = shlex.split(arg)
                #print cloud_deamon
                ret=exec_function("cloud_comm_daemon", cloud_daemon)
                if ret != 0:
                    return ret;
        elif opt == '--cloud_proxy':
            if arg != 'None':
                cloud_proxy = shlex.split(arg)
                #print cloud_proxy
                ret=exec_function("cloud_proxy", cloud_proxy)
                if ret != 0:
                    return ret;
    return 0
if __name__ == "__main__":
    exit(main())
