#!/usr/bin/env python
# _*_ coding: utf8 _*_

import os, sys, getopt, time
import shlex
import fcntl
import shutil
import errno
import traceback
import commands
import subprocess
from xml.etree import ElementTree
from xml.etree.ElementTree import ElementTree,Element
from common import logger

VERBOSE = True

gms_conf='/gms/conf/gmsconfig.xml'
gms_conf_bak='/gms/conf/gmsconfigbak.xml'
bug_path = '/data/tmpdata/debug/debug.ok'
log_path = '/data/log/gapi/'
gapi_path = os.path.dirname(sys.argv[0])
session_file = '/var/run/%s.session' % os.path.splitext(os.path.basename(sys.argv[0]))[0]
session_timeout = 2
def check_xmlfile():
    tree = ElementTree()
    try:
        tree.parse(gms_conf)
        return 1
    except:
        return 0
def enable_ssh(argv):
    script = os.path.join(gapi_path, 'modules', 'debug', 'enable_ssh.pyc')
    return exec_script(script)

def enable_debug(argv):
    script = os.path.join(gapi_path, 'modules', 'bug', 'getgmsinfo.pyc')
    return exec_script(script)

def flow_def(argv):
    script = os.path.join(gapi_path, 'modules', 'flow_def', 'flow_def_main.pyc')
    return exec_script(script,argv)

def disable_ssh(argv):
    script = os.path.join(gapi_path, 'modules', 'debug', 'disable_ssh.pyc')
    return exec_script(script)

def run_syslog(argv):
    script = os.path.join(gapi_path, 'modules', 'warn', 'run_syslog.pyc')
    return exec_script(script, argv)

def set_ip(argv):
    script = os.path.join(gapi_path, 'modules', 'network', 'xml_att.pyc')
    return exec_script(script, argv)

def set_device(argv):
    script = os.path.join(gapi_path, 'modules', 'device', 'set_sw.pyc')
    return exec_script(script, argv)

def set_time(argv):
    script = os.path.join(gapi_path, 'modules', 'time', 'xml_time.pyc')
    return exec_script(script, argv)

def set_proxy(argv):
    script = os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_proxy.pyc')
    return exec_script(script, argv)

def set_wtl(argv):
    script = os.path.join(gapi_path, 'modules', 'wtl','wtl.pyc')
    return exec_script(script, argv)

def set_speed(argv):
    script = os.path.join(gapi_path, 'modules', 'proxy','cloud', 'cloud_comm_daemon.pyc')
    return exec_script(script, argv)

def set_engine(argv):
    script = os.path.join(gapi_path, 'modules', 'engine', 'engine.pyc')
    return exec_script(script, argv)

def set_ethinfo(argv):
    script = os.path.join(gapi_path, 'modules', 'ethinfo', 'ethinfo.pyc')
    return exec_script(script, argv)

def set_auth(argv):
    script = os.path.join(gapi_path, 'modules', 'auth', 'gms_auth.pyc')
    return exec_script(script, argv)

def set_network(argv):
    script = os.path.join(gapi_path, 'modules', 'set_network', 'set_network_main.pyc')
    return exec_script(script, argv)

def get_genkey(argv):
    script = os.path.join(gapi_path, 'modules', 'auth', 'genkey.pyc')
    return exec_script(script, argv)

def write_conf(argv):
    script = os.path.join(gapi_path, 'modules', 'auth', 'com_to_conf.pyc')
    return exec_script(script, argv)

def ischild(argv):
    script = os.path.join(gapi_path, 'modules', 'network', 'ischild.pyc')
    return exec_script(script, argv)

def flowconfig(argv):
    script = os.path.join(gapi_path, 'modules', 'pro_restore', 'pro_restore_main.pyc')
    return exec_script(script, argv)

def setEmail(argv):
    script = os.path.join(gapi_path, 'modules', 'est', 'email_mtx.pyc')
    return exec_script(script, argv)

def setSecurity(argv):
    script = os.path.join(gapi_path, 'modules', 'est', 'security.pyc')
    return exec_script(script, argv)

def setThreshold(argv):
    script = os.path.join(gapi_path, 'modules', 'est', 'threshold.pyc')
    return exec_script(script, argv)

def setDDos(argv):
    script = os.path.join(gapi_path, 'modules', 'ddos', 'ddos_main.pyc')
    return exec_script(script, argv)
gapi_dict = {
        'enable_ssh': enable_ssh,
        'disable_ssh': disable_ssh,
        'run_syslog': run_syslog,
        'set_ip': set_ip,
        'set_time': set_time,
        'set_wtl':set_wtl,
        'cloud_proxy': set_proxy,
        'engine': set_engine,
        'sys_logmon': set_auth,
        'cloud_comm_daemon': set_speed,
        'ethinfo':set_ethinfo,
        'writeconf': write_conf,
        'genkey': get_genkey,
        'ischild': ischild,
        'flow_define': flow_def,
        'flowconfig': flowconfig,
        'enable_debug': enable_debug,
        'device': set_device,
        'set_network': set_network,
        'setEmail': setEmail,
        'setSecurity': setSecurity,
        'setThreshold': setThreshold,
        'setDDos': setDDos,
        }

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
    if status == 0:
        shutil.copyfile(gms_conf,gms_conf_bak)
    if status != 0:
        if 0 == check_xmlfile():
            shutil.copyfile(gms_conf_bak,gms_conf)
        if VERBOSE: logger.logger.warn('exec script[%s] status[%s] != 0. %s.' % (script, status, stderr))
    return status

def exec_function(sessname, funcname, argv, force):
    try:
        function = gapi_dict[funcname]
    except KeyError:
        return 2
    else:
        #session_fd = open(session_file, 'rwa+')
        session_fd = open(session_file, 'a+')
        fcntl.flock(session_fd, fcntl.LOCK_EX)#|fcntl.LOCK_NB)
        #try:
        #    fcntl.flock(session_fd, fcntl.LOCK_EX|fcntl.LOCK_NB)
        #except IOError as ex:
        #    session_fd.close()
        #if VERBOSE: logger.logger.info('lock fail. exit.')
        #    return 118

        if VERBOSE: logger.logger.debug('lock success')
        session_fd.seek(0)
        data = session_fd.readline()
        if VERBOSE: logger.logger.debug('session data [%s]' % (data))
        if data == '':
            session_data = '%s %s' % (sessname, int(time.time()))
        else:
            try:
                last_session, last_second = data.split(' ')
                if sessname == last_session:
                    session_data = '%s %s' % (sessname, int(time.time()))
                else:
                    cur_second = int(time.time())
                    last_second = int(last_second)
                    if (cur_second - last_second) > session_timeout or force == True:
                        session_data = '%s %s' % (sessname, int(time.time()))
                    else:
                        fcntl.flock(session_fd, fcntl.LOCK_UN)
                        session_fd.close()
                        if VERBOSE: logger.logger.debug('unlock success')
                        sys.stdout.write("%s" % last_session)
                        return 3
            except ValueError, NameError:
                if VERBOSE: logger.logger.warn('Illegal session data [%s]' % data)
                session_data = '%s %s' % (sessname, int(time.time()))

        session_fd.truncate(0)
        session_fd.write('%s' % session_data)
        session_fd.flush()

        if VERBOSE: logger.logger.debug('exec %s'% (function))
        try:
            ret = function(argv)
            if VERBOSE: logger.logger.debug('exec %s success. ret[%s].'% (function, ret))
        except Exception as ex:
            if ex.errno == errno.EACCES: # 没有执行权限
                ret = 117
            else:
                ret = 116
            if VERBOSE: logger.logger.warn('exec %s fail. %s.'% (function, str(ex)))

        fcntl.flock(session_fd, fcntl.LOCK_UN)
        session_fd.close()
        if VERBOSE: logger.logger.debug('unlock success')
        return ret

def main():
    logger.init_log(log_path)

    opts, args = getopt.getopt(sys.argv[1:], '', ['session=', 'function=', 'args=', 'force'])
    if args != []:
        if VERBOSE: logger.logger.info('Usage: %s --session session --function function --args args' % sys.argv[0])
        return 1

    sessname = funcname = args = None
    force = False
    argv = []
    for opt, arg in opts:
        if opt == '--session':
            sessname = arg
        elif opt == '--function':
            funcname = arg
        elif opt == '--force':
            force = True
        elif opt == '--args':
            args = arg
            argv = shlex.split(args)

    if sessname == None or funcname == None:
        if VERBOSE: logger.logger.info('Usage: %s --session session --function function --args args' % sys.argv[0])
        return 1
    else:
        if VERBOSE:
            logger.logger.info('exec <%s --session %s --function %s [--args %s] [--force %s]>' % (sys.argv[0],
                                                                                                sessname, funcname,
                                                                                                args, force))
        return exec_function(sessname, funcname, argv, force)

if __name__ == '__main__':
    exit(main())

