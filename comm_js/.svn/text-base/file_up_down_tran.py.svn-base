# _*_ coding: utf8 _*_

import os
import sys
import time
import socket
import struct
import shutil
import zipfile
import traceback
import subprocess
import syslog
import comm_data
import comm_file_tran
import comm_cmd_proc
import comm_reg
import comm_common
import device_manage
import comm_log_record
from xml.etree import ElementTree as ET
from comm_common import file_filter_bysuffix, createzip, extractzip, listzip, comm_file_copy, safe_copy
from est.email_object import email_threshold
from est.init_config import InitConfig as init_config

sys.path.append('/gms/gapi/modules/ddos/')

PATH = os.path.abspath("/gms/conf/gmsconfig.xml")
STATTXT = os.path.abspath("/gms/guard/conf/stat.txt")
INITCONF = os.path.abspath("/gms/guard/conf/guard.conf")
FLAGTHRE = 0

def stat_path():
    from init_conf import OperationFile
    init_file = OperationFile("/gms/guard/conf/guard.conf")
    path = init_file.read("sysstatus_file","file_path")
    if "tmpdata/db" in path:
        return True
    else:
        return False
        
def update_data():
    root = ET.parse(PATH)
    comm_data.sys_war = root.find("threshold").get("war")
    comm_data.sys_pro = root.find("threshold").get("pro")
    return True

def stat_init(stat_file = "stat_file"):
    #global FLAGTHRE
    if stat_path():
        with open(STATTXT) as fp:
            data_stat = fp.readlines()
        data_stat_dict = dict([i.split(":") for i in data_stat])
        dist_data = float(data_stat_dict.get("db", 0))
        #comm_log_record.logger.info('disk usage:%s' % dist_data)
        #comm_log_record.logger.info('[parameter flag: %s]===[war: %s]===[pro: %s]' % (FLAGTHRE, comm_data.sys_war, comm_data.sys_pro))
    else:
        with open(stat_file) as fp :
            stat_con = fp.read()
        #comm_log_record.logger.info('[parameter flag: %s]===[war: %s]===[pro: %s]' % (FLAGTHRE, comm_data.sys_war, comm_data.sys_pro))
        dist_data = float(stat_con.split("|")[-1])
        #comm_log_record.logger.info('%s' % stat_con)
    return dist_data


def stat_email(xml_time, stat_file="stat_file"):
    
        global FLAGTHRE
        xml_time_now = os.stat(PATH).st_mtime
        if xml_time != xml_time_now:
            update_data()
            xml_time = xml_time_now
        #if dist_data >= float(100-int(comm_data.sys_war) ):
        try:
            dist_data = stat_init()
            if float(dist_data) == float(0):
                pass
            elif FLAGTHRE == 2 and dist_data >= float(100-int(comm_data.sys_war)) and dist_data <= float(100-int(comm_data.sys_pro)):                      
                    FLAGTHRE = 1
            elif  dist_data <= float(100-int(comm_data.sys_war)):
                    FLAGTHRE = 0
            elif FLAGTHRE == 0 and  dist_data >= float(100-int(comm_data.sys_war)) \
                                         and dist_data <= float(100-int(comm_data.sys_pro)):
                    #dist_data = u" 硬盘阈值达到%s%s，请知悉 " % (dist_data, "%")
                    #setattr(comm_data, "psize", 1)
                    FLAGTHRE = 1
                    comm_log_record.logger.info('comm data psize number:%s' % FLAGTHRE )
                    comm_log_record.logger.info('threshold email parameter :%s' % dist_data)
                    tmp_data = "%,磁盘剩余空间不足，已达到磁盘预警阈值" + str(comm_data.sys_war)+"%"
                    dist_data = comm_data.sys_thresold.decode('utf-8') % (dist_data, tmp_data)
                    email_threshold(dist_data)
                    comm_log_record.logger.info('threshold email send OK !!!')
            elif FLAGTHRE == 1 and dist_data > float(100-int(comm_data.sys_pro) ):
                    #setattr(comm_data, "psize", 0)
                    FLAGTHRE = 2
                    comm_log_record.logger.info('comm data psize number:%s' % FLAGTHRE )
                    comm_log_record.logger.info('threshold email parameter :%s' % dist_data)
                    tmp_data = "%,磁盘剩余空间不足，已达到磁盘保护阈值" + str(comm_data.sys_pro)+"%"
                    dist_data = comm_data.sys_thresold.decode('utf-8') % (dist_data, tmp_data)
                    email_threshold(dist_data)
                    comm_log_record.logger.info('threshold email send OK !!!')
            else:
                #comm_log_record.logger.info('[parameter flag: %s]===[war: %s]===[pro: %s]' % (FLAGTHRE, comm_data.sys_war, comm_data.sys_pro))
                pass
        except Exception, ex:
            print ex
            comm_log_record.logger.info('send stat email fail:%s' % ex )
        finally:
            return xml_time
#10G
def xge_update_file():
    #if comm_data.ismanager == "1" :
    update_file_suffix = ".up"
    for down_file in file_filter_bysuffix([comm_data.comm_ue_down_path], [update_file_suffix]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            try:
                if comm_data.ismanager == "1" :
                        sub_dev_list = []
                        for i in comm_data.sub_dev_list:
                            sub_dev_list.append(i.ip)
                            shutil.copy(down_file, down_file + i.ip)
                        os.remove(down_file)
                        #if comm_data.ismonitor != "1" :
                        #    os.remove(down_file)
                        #else:
                        #    shutil.move(down_file, comm_data.comm_ue_ui_path + os.path.basename(down_file))
                        for sub_dev in sub_dev_list :
                            shutil.move(down_file + sub_dev , down_file)
                            down_file_transfer_cmd(sub_dev, comm_cmd_proc.ue_file_down, comm_cmd_proc.notify, down_file)
                            #os.remove(down_file + sub_dev)
                comm_log_record.logger.info("update system file  from file[%s] successed." % down_file)
                if comm_data.ismonitor == "1" and comm_data.ismanager != "1":#add is monitor fun "/gms/updata/upgrade_system.sh 1"
                    shutil.move(down_file, comm_data.comm_ue_ui_path + os.path.basename(down_file))
                    os.system("nohup /gms/updata/upgrade_system.sh 1  1>/dev/null 2>&1 &")
                    #p = subprocess.Popen(['/gms/updata/upgrade_system.sh 1', down_file],
                    #                    stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    #p.wait()
                    comm_log_record.logger.info("rename file[%s] successod. " % down_file)
            except:
                comm_log_record.logger.error("update system from file[%s] fail. %s." % (down_file, traceback.format_exc()))

def up_file_transfer_cmd(filetype, eventtype, filename):
    if comm_data.up_cmd_info == None:
        comm_data.up_cmd_info = comm_cmd_proc.comm_make_cmdinfo(filetype, eventtype, filename)
        #comm_log_record.logger.debug("make up file cmd [%s]" % comm_data.up_cmd_info)
    else:
        # 如果走到这里说明上面的某些流程出问题了。
        comm_log_record.logger.warn("make up file cmd. up_cmd_info %s!= None" % comm_data.up_cmd_info)

#下载文件命令接口, 需要将文件传输到对应的子设备下(在扫描文件的时候可以看到dev_id,有dev_id转换)
def down_file_transfer_cmd(ip, filetype, eventtype, filename):
    for i in comm_data.sub_dev_list:
        if i.ip == ip:
            if i.down_cmd_info == None:
                i.down_cmd_info = comm_cmd_proc.comm_make_cmdinfo(filetype, eventtype, filename)
                #comm_log_record.logger.debug("make down file cmd [%s]" % i.down_cmd_info)
            break;

def process_local_rule_file(rule_file):
    def is_mtx_rule(rule_file):
        if "/mtx/" in rule_file:
            return True
        else:
            return False

    def is_vds_rule(rule_file):
        if "/vds/" in rule_file:
            return True
        else:
            return False

    def is_apt_rule(rule_file):
        if "/apt/" in rule_file:
            return True
        else:
            return False

    def is_surl_rule(rule_file):
        if "/surl/" in rule_file:
            return True
        else:
            return False

    if is_mtx_rule(rule_file):
        return update_mtx_rule(rule_file)
    elif is_vds_rule(rule_file):
        return update_vds_rule(rule_file)
    elif is_apt_rule(rule_file):
        return update_apt_rule(rule_file)
    elif is_surl_rule(rule_file):
        return update_surl_rule(rule_file)
    else:
        return False

def update_vds_rule(rule_file):
    # /vds/script/update.sh -manual /vds/script/avl_vxbase_20140827_08.tar.gz.ok
    result = False
    update_script = '/vds/script/update.sh'
    try:
        # 解压这里
        zflist = listzip(rule_file)
        if len(zflist) == 1 and zflist[0] == os.path.basename(rule_file):
            extractzip(rule_file, os.path.dirname(rule_file))
            #p = subprocess.Popen([update_script, '-manual', rule_file],
            #                    stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            #p.wait()
            # 使用上面的方式执行脚本会报 OSError: [Errno 8] Exec format error 异常。
            # 改为使用 os.system 这里没有，也无法判断是否升级成功。只能由测试去查看vds目录来判断。
            os.system('%s -manual %s' % (update_script, rule_file))
            result = True
            comm_log_record.logger.info('vds rule file[%s] update Success?' % rule_file)
        else:
            comm_log_record.logger.info('vds rule file[%s] fmt error. flist[%s].' % (rule_file, zflist))
    except:
        comm_log_record.logger.error('vds rule file[%s] update Fail.' % rule_file)
        comm_log_record.logger.info(traceback.format_exc())
    finally:
        try:
            backname = os.path.join(comm_data.comm_vds_rule_backup_path, '%s.%d' % (os.path.basename(rule_file), time.time()))
            shutil.move(rule_file, backname)
        except:
            comm_log_record.logger.info(traceback.format_exc())
        return result

def update_apt_rule(rule_file):
    back_file = rule_file + '.%s' % time.time()
    os.rename(rule_file, back_file)
    return True

def update_surl_rule(rule_file):
    def reload_url_detect():
        reload_scripts = ['/gms/url_detect/reload.sh',
                            '../url_detect/reload.sh']

        for reload_script in reload_scripts:
            if os.path.exists(reload_script):
                p = subprocess.Popen([reload_script],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
                p.wait()
                if p.returncode == 0:
                    comm_log_record.logger.error('url_detect reload Success.')
                    return True
                else:
                    comm_log_record.logger.error('url_detect reload Fail. %s.' % p.stderr.read())
                    return False

        comm_log_record.logger.error('url_detect reload Fail. script not found.')
        return False

    try:
        # 解压缩到 url_detect/conf 目录下。解压后的文件名和压缩包同名。
        conf_path = os.path.dirname(comm_data.surl_conf_file)
        extractzip(rule_file, conf_path)

        # 重命名解压缩的文件名为规则名
        rule_basename = os.path.basename(rule_file)
        rule_tempname = os.path.join(conf_path, rule_basename)
        rule_backname = comm_data.surl_conf_file + '.%s' % time.time()
        if os.path.exists(comm_data.surl_conf_file):
            # 备份原有的规则文件
            os.rename(comm_data.surl_conf_file, rule_backname)

        os.rename(rule_tempname, comm_data.surl_conf_file)

        # 重载 url_detect 进程
        reload_url_detect()

        # 最后删除规则文件
        os.remove(rule_file)

        return True
    except:
        comm_log_record.logger.info(traceback.format_exc())
        comm_log_record.logger.info('update surl file[%s] fail.' % rule_file)
        # 如果上面失败，备份出错文件。回退修改。
        if os.path.exists(rule_file):
            os.rename(rule_file, rule_file + '.bak')
        if os.path.exists(rule_backname) and not os.path.exists(comm_data.surl_conf_file):
            os.rename(rule_backname, comm_data.surl_conf_file)
        if os.path.exists(rule_tempname):
            os.remove(rule_tempname)
        return False

def update_mtx_rule(conf_file):
    def check_mtx_conf_file(mtx_conf_file):
        # 只要是压缩包，并且包内只有一个文件，就认为是正确
        if comm_common.is_zipfile(mtx_conf_file) and len(listzip(mtx_conf_file)) == 1:
            return True
        else:
            return False

    def multi_copy_mtx_conf_file(mtx_conf_file):
        dirname, basename = os.path.split(mtx_conf_file)
        prefix = basename[0:basename.rfind('_')]
        for sub_dev_uuid in device_manage.get_all_sub_uuid():
            dstname = '%s/%s_%s.ok' % (dirname, prefix, sub_dev_uuid)
            comm_log_record.logger.info("mtx conf file [%s] -> [%s]\n" % (mtx_conf_file, dstname))
            safe_copy(mtx_conf_file, dstname)

    def update_mtx_conf_file(mtx_conf_file):
        # 简单说就是把压缩包内的文件解压缩，然后改名为 /gms/mtx/conf/rule.conf
        try:
            flist = extractzip(mtx_conf_file, comm_data.mtx_conf_path)
            src_rule_file_name = flist[0]
            dst_rule_file_name = os.path.join(comm_data.mtx_conf_path, 'rule.conf')
            dst_rule_back_name = '%s.%s' % (dst_rule_file_name, time.time())
            if os.path.exists(dst_rule_file_name):
                os.rename(dst_rule_file_name, dst_rule_back_name)
            os.rename(src_rule_file_name, dst_rule_file_name)
            if os.path.exists(dst_rule_back_name):
                os.remove(dst_rule_back_name)
        except:
            comm_log_record.logger.info(traceback.format_exc())
            if os.path.exists(src_rule_file_name):
                os.remove(src_rule_file_name)
            if os.path.exists(dst_rule_back_name):
                os.rename(dst_rule_back_name, dst_rule_file_name)
            raise

    def restart_mtx():
        try:
            subprocess.check_call(['killall', 'mtx'])
        except subprocess.CalledProcessError:
            # 最后杀死 mtx 出错。则什么都不做。只提示(有可能是 mtx 进程不存在)。
            comm_log_record.logger.warn(traceback.format_exc())
            comm_log_record.logger.warn('restart mtx fail.')

    if check_mtx_conf_file(conf_file) == True:
        try:
            result = False
            if comm_data.isroot == '1':
                # 如果是 root 结点，要把配置文件拆分成所有子结点的对应包
                # 只对压缩包进行拷贝，所以下级收到的压缩包的UUID是自己，
                # 但包内文件的名字是顶级结点
                multi_copy_mtx_conf_file(conf_file)

            # 所有结点都会升级 mtx 规则。但只有检测点需要重启 mtx。
            update_mtx_conf_file(conf_file)
            if comm_data.ismonitor == '1':
                restart_mtx()
            result = True
            comm_log_record.logger.info('update mtx conf[%s] success.' % conf_file)
        except:
            comm_log_record.logger.info(traceback.format_exc())
            comm_log_record.logger.info('update mtx conf[%s] fail. backup to dir[%s]' % (conf_file, comm_data.comm_re_backup_path))
            shutil.copy(conf_file, comm_data.comm_re_backup_path)
        finally:
            os.remove(conf_file)
            return result
    else:
        comm_log_record.logger.error('mtx rule file[%s] fmt error. backup to dir[%s]' % (conf_file, comm_data.comm_re_backup_path))
        shutil.move(conf_file, comm_data.comm_re_backup_path)
        return False

def send_down_file_success_action(fname):
    def copytodbftp(fname, db_dir, ftp_dir, bak_dir):
        try:
            if comm_data.if_is_xge_device == '1' and comm_data.ismonitor == '1' and comm_data.ismanager != '1':
                pass
            else:
                if comm_data.iscopytodb == True:
                    extractzip(fname, db_dir)
                    #comm_log_record.logger.info("extract zipfile[%s] to dir[%s]" % (fname, db_dir))
                if comm_data.iscopytoftp == True:
                    extractzip(fname, ftp_dir)
        except Exception:
            comm_log_record.logger.info(traceback.format_exc())
            if os.path.exists(fname):
                shutil.copy(fname, bak_dir)
                comm_log_record.logger.warn("extract zipfile[%s] fail. backup to dir[%s]" % (fname, bak_dir))

    # mtd 类型下面有两个目录。需要解压和拷贝。
    # 其它类型文件只需要删除
    if comm_data.comm_mtd3rd_down_path in fname:
        db_dir = comm_data.db_mtd3rd_path
        ftp_dir = comm_data.ftp_mtd3rd_path
        bak_dir = comm_data.comm_me_backup_path
        copytodbftp(fname, db_dir, ftp_dir, bak_dir)
        send_mtd_via_syslog(fname)

    elif comm_data.comm_mtdatt_down_path in fname:
        db_dir = comm_data.db_mtdatt_path
        ftp_dir = comm_data.ftp_mtdatt_path
        bak_dir = comm_data.comm_me_backup_path
        copytodbftp(fname, db_dir, ftp_dir, bak_dir)
        send_mtd_via_syslog(fname)

    os.remove(fname)

def exec_query_script(query_script, sql_fname):
    #os.chmod(down_file, os.R_OK|os.W_OK|os.X_OK)
    p = subprocess.Popen(['python', query_script, sql_fname],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    p.wait()

#文件下发接口
def send_down_file():
    while comm_common.is_continue():
        time.sleep(0.1)

        # 尽量确保注册文件先被传输
        reg_down_file()

        if comm_reg.is_reg_done() == 0:
            continue

        #遍历文件目录
        for down_file in file_filter_bysuffix([comm_data.comm_mtd3rd_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            #根据文件名中带有的id，通过设备管理模块提供的接口查找到对应的ip
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                # 如果文件属于本设备解压后删除，不下传。
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.mtx_evt_down)
                    if comm_data.iscopytodb == True:
                        extractzip(down_file, comm_data.db_mtd3rd_path)
                        #extractzip(down_file, comm_data.comm_mtdall_down_path)
                    if comm_data.iscopytoftp == True:
                        extractzip(down_file, comm_data.ftp_mtd3rd_path)
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
                    comm_log_record.logger.info("extract zipfile[%s] fail. backup to dir[%s]" % (down_file, comm_data.comm_me_backup_path))
                    shutil.copy(down_file, comm_data.comm_me_backup_path)
                finally:
                    send_mtd_via_syslog(down_file)
                    os.remove(down_file)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    # 找不到拓扑的文件删掉
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.mtx_evt_down, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_mtdatt_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                # 如果文件属于本设备解压后删除，不下传。
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.mtx_evt_down)
                    #shutil.copy(down_file, comm_data.comm_mtdatt_ansic__path)
                    if comm_data.iscopytodb == True:
                        extractzip(down_file, comm_data.db_mtdatt_path)
                        #extractzip(down_file, comm_data.comm_mtdall_down_path)
                    if comm_data.iscopytoftp == True:
                        extractzip(down_file, comm_data.ftp_mtdatt_path)
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
                    comm_log_record.logger.info("extract zipfile[%s] fail. backup to dir[%s]" % (down_file, comm_data.comm_me_backup_path))
                    shutil.copy(down_file, comm_data.comm_me_backup_path)
                finally:
                    send_mtd_via_syslog(down_file)
                    os.remove(down_file)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.mtx_evt_down, comm_cmd_proc.notify, down_file)

        '''for down_file in file_filter_bysuffix([comm_data.comm_mtdweb_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                # 如果文件属于本设备解压后删除，不下传。
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.mtx_evt_down)
                    shutil.copy(down_file, comm_data.comm_mtd3rd_down_path)
                    extractzip(down_file, comm_mtdweb_down_unzip_path )
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
                    comm_log_record.logger.info("extract zipfile[%s] fail. backup to dir[%s]" % (down_file, comm_data.comm_me_backup_path))
                    shutil.copy(down_file, comm_data.comm_me_backup_path)
                finally:
                    send_mtd_via_syslog(down_file)
                    os.remove(down_file)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.mtx_evt_down, comm_cmd_proc.notify, down_file)
'''
        #for down_file in file_filter_bysuffix([comm_data.comm_ue_down_path], [".ok"]):
        #    if not comm_common.is_continue():
        #        break
        #    reg_down_file()
        #    sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
        #    if sub_dev_uuid == comm_data.uuid:
        #        try:
        #            comm_data.inc_recv_local_cnt(comm_cmd_proc.ue_file_down)
        #            # 对 GMS-1.0.0.2-7b5d04c86c822bf5e73548540187c985-emergency_f9d86946-21fd-11e4-b78e-d5007112775c.ok 形式的文件名
        #            # 解压缩，重命名为 GMS-1.0.0.2-7b5d04c86c822bf5e73548540187c985-emergency 放到 /gms/updata/pack 目录下。
        #            # 这里依赖压缩包和原文件同名。否则会出现压缩包 a.ok 解出 b.ok ，然后把 a.ok 移到 pack 目录下，下一轮 b.ok uuid 无效。
        #            extractzip(down_file, comm_data.comm_ue_down_path)
        #            dst_file = os.path.join(comm_data.comm_ue_ui_path, os.path.splitext(os.path.basename(down_file))[0].split('_')[0])
        #            shutil.copy(down_file, dst_file)
        #            os.remove(down_file)
        #        except zipfile.BadZipfile:
        #            comm_log_record.logger.info("File[%s] is not a zip file" % down_file)
        #            if os.path.exists(down_file):
        #                os.rename(down_file, down_file + '.bak')
        #        except Exception:
        #            comm_log_record.logger.info(traceback.format_exc())
        #    else:
        #        sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
        #        if sub_dev_ip == "no_value":
        #            comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
        #            os.remove(down_file)
        #        else:
        #            down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.ue_file_down, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_re_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[-1]
            if sub_dev_uuid == comm_data.uuid:
                comm_data.inc_recv_local_cnt(comm_cmd_proc.re_file_down)
                # 在管理点时会走到这个流程。也拷贝到 mtx/conf 下。
                process_local_rule_file(down_file)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.re_file_down, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_conf_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                comm_data.inc_recv_local_cnt(comm_cmd_proc.conf_file_down)
                os.rename(down_file, down_file + '.bak')
                # ? 属于本设备的如何做 ?
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.conf_file_down, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_query_flow_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.query_flow_down_evt)
                    # 压缩包内的文件和压缩包同名
                    extractzip(down_file, os.path.dirname(down_file))
                    # 执行完会自动删除 down_file
                    p = subprocess.Popen(['/gms/query_flow/query_flow', down_file],
                                        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    p.wait()
                except:
                    comm_log_record.logger.info(traceback.format_exc())
                    os.rename(down_file, down_file + '.bak')
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.rename(down_file, down_file + '.bak')
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.query_flow_down_evt, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_query_dns_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.query_dns_down_evt)
                    sql_fname = down_file
                    extractzip(down_file, os.path.dirname(down_file))
                    exec_query_script('/gms/comm/query/dns_query.py', sql_fname)
                except:
                    comm_log_record.logger.info(traceback.format_exc())
                    os.rename(sql_fname, sql_fname + '.bak')
                else:
                    os.remove(sql_fname)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.rename(sql_fname, sql_fname + '.bak')
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.query_dns_down_evt, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_query_url_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.query_url_down_evt)
                    sql_fname = down_file
                    extractzip(down_file, os.path.dirname(down_file))
                    exec_query_script('/gms/comm/query/url_query.py', sql_fname)
                except:
                    comm_log_record.logger.info(traceback.format_exc())
                    os.rename(sql_fname, sql_fname + '.bak')
                else:
                    os.remove(sql_fname)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.rename(sql_fname, sql_fname + '.bak')
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.query_url_down_evt, comm_cmd_proc.notify, down_file)
        #10G
        if comm_data.if_is_xge_device == "1" :
            xge_update_file()
        #10G

def travel_path(path, filetype, suffix=".ok", eventtype=comm_cmd_proc.notify):
    xml_time = os.stat(PATH).st_mtime
    for up_file in file_filter_bysuffix([path], [suffix]):
        if comm_common.is_continue():
            # 确保有注册文件的时候，先传注册文件
            reg_up_file()

            if not os.path.exists(up_file):
                # 最可能的情况是取到几百个文件，然后测试时手动删除了文件，
                # 在这里进行判断，不然会连续报出几百个异常
                continue

            try:
                if  filetype != comm_cmd_proc.pcap_evt_up :#or filetype != comm_cmd_proc.sqlchop_evt_up :
                    if filetype == comm_cmd_proc.devstat_evt_up and islocalstat(up_file):
                        if comm_common.is_zipfile(up_file):
                            comm_log_record.logger.info("!!! up_file[%s] is zip file." % up_file)
                        else:
                            try:
                                #comm_log_record.logger.info("[%s]send device stat email." % xml_time)
                                xml_time = stat_email(xml_time, up_file)
                            except Exception,ex:
                                comm_log_record.logger.info('stat email:%s' % traceback.format_exc())
                              
                            createzip(up_file, up_file)
                    elif (comm_data.ismonitor == '1' and filetype != comm_cmd_proc.topo_evt_up):
                        # topo 文件不压缩。界面无法解压缩。
                        if comm_common.is_zipfile(up_file):
                            comm_log_record.logger.info("!!! up_file[%s] is zip file." % up_file)
                        else:
                            createzip(up_file, up_file)
                #if filetype == comm_cmd_proc.sqlchop_evt_up:
                #    try:
                #        if comm_common.is_zipfile(up_file)==False:
                #            createzip(up_file,up_file)
                #        extractzip(up_file, comm_data.db_mtdatt_path)
                #        extractzip(up_file, comm_data.comm_mtdall_down_path)
                #        if comm_data.isroot == "1" or comm_data.link == "0":
                #            os.remove(up_file)
                #    except Exception, ex:
                #        comm_log_record.logger.info("Error:%s" % ex )
                #        pass

                up_file_transfer_cmd(filetype, eventtype, up_file)

                # 向上发送线程发送完毕后，会置这个标识位。
                # 这个while不可以拿到 comm_make_cmdinfo 上面
                # 在最开始 up_cmd_info == None，这里创建命令，另一个线程上传，这里等待
                # 传输完毕，清 up_cmd_info，这里继续走
                # 如果拿到上面，只有一个文件的时候，会创建这个文件的命令，
                # 然后下一次又遍历到同一个文件, 并在这个 while 循环里等待。
                while comm_common.is_continue() and comm_data.up_cmd_info != None:
                    time.sleep(0.1)
            except Exception,ex:
                comm_log_record.logger.info('%s' % traceback.format_exc())
        else:
            break

def islocalstat(fname):
    dev_uuid = os.path.splitext(os.path.basename(fname))[0].split('_')[1].split('.')[0]
    if dev_uuid == comm_data.uuid:
        return True
    else:
        return False

def send_up_file_seccuss_action(fname):
    def copytodbftp(fname, dbpath, ftppath):
        try:
            if comm_data.if_is_xge_device == '1' and comm_data.ismonitor == '1' and comm_data.ismanager != '1':
                pass
            else:
                if comm_data.iscopytodb == True and dbpath != None:
                    extractzip(fname, dbpath)
                if comm_data.iscopytoftp == True and ftppath != None:
                    extractzip(fname, ftppath)
        except Exception:
            comm_log_record.logger.info(traceback.format_exc())
            if os.path.exists(fname):
                comm_log_record.logger.info("extract zipfile[%s] fail. backup" % fname)
                os.rename(fname, fname + '.bak')

    # mtd topo 查询(flow dns url) 不用入库直接删掉

    if comm_data.comm_ve_up_path in fname:
        copytodbftp(fname, comm_data.db_ve_path, comm_data.ftp_virus_path)
    elif comm_data.comm_sqlchop_up_path in fname:
        copytodbftp(fname, comm_data.db_mtdatt_path, None)
        copytodbftp(fname, comm_data.comm_mtdatt_down_path, None)
    elif comm_data.comm_ae_up_path in fname:
        copytodbftp(fname, comm_data.db_ae_path, comm_data.ftp_abb_path)
    elif comm_data.comm_surl_up_path in fname:
        copytodbftp(fname, comm_data.db_surl_path, comm_data.ftp_surl_path)
    elif comm_data.comm_devstat_up_path in fname and not islocalstat(fname):
        # 设备状态在检测点已经入库
        copytodbftp(fname, comm_data.db_devstat_path, None)
    #elif comm_data.comm_flow_up_path in fname and comm_data.ismonitor != '1':
    elif comm_data.comm_flow_up_path in fname and not islocalstat(fname):
        # 流量在检测点已经入库
        copytodbftp(fname, comm_data.db_flow_path, None)
    #elif comm_data.comm_old_event_flow_up_path in fname and comm_data.ismonitor != '1':
    elif comm_data.comm_old_event_flow_up_path in fname and not islocalstat(fname):
        # 流量在检测点已经入库
        copytodbftp(fname, comm_data.db_old_event_flow_path, None)

    os.remove(fname)

#文件上传接口
def send_up_file():
    while comm_common.is_continue():
        reg_up_file()

        if comm_reg.is_reg_done() == 0:
            # 如果没有注册成功，只上传注册文件
            time.sleep(0.1)
            continue

        travel_path(comm_data.comm_mtd_up_path, comm_cmd_proc.mtx_evt_up)

        travel_path(comm_data.comm_ve_up_path, comm_cmd_proc.ve_evt_up)

        travel_path(comm_data.comm_ae_up_path, comm_cmd_proc.ae_evt_up)

        travel_path(comm_data.comm_devstat_up_path, comm_cmd_proc.devstat_evt_up)

        travel_path(comm_data.comm_topo_up_src_path, comm_cmd_proc.topo_evt_up, suffix='.xml')

        travel_path(comm_data.comm_flow_up_path, comm_cmd_proc.flow_evt_up)
        # stat for anything
        if comm_data.old_flow == "1":
            travel_path(comm_data.comm_old_event_flow_up_path, comm_cmd_proc.old_flow_evt_up)

        travel_path(comm_data.comm_surl_up_path, comm_cmd_proc.surl_evt_up)

        travel_path(comm_data.comm_query_flow_up_path, comm_cmd_proc.query_flow_up_evt)

        travel_path(comm_data.comm_query_dns_up_path, comm_cmd_proc.query_dns_up_evt)

        travel_path(comm_data.comm_query_url_up_path, comm_cmd_proc.query_url_up_evt)

        try:
            if stat_path():
                xml_time = os.stat(PATH).st_mtime
                #comm_log_record.logger.info("[%s]send device stat email." % xml_time)
                xml_time = stat_email(xml_time)
        except Exception,ex:
            comm_log_record.logger.info('stat email:%s' % traceback.format_exc())
        travel_path(comm_data.comm_sqlchop_up_path, comm_cmd_proc.sqlchop_evt_up)

def reg_up_file():
   # if comm_data.if_is_xge_device == '1' :
   #     file_name = comm_data.comm_reg_up_tmp_path
   # else:
   #     file_name = comm_data.comm_reg_up_path
    for up_file in file_filter_bysuffix([comm_data.comm_reg_up_path], [".ok"]):
        if comm_common.is_continue() :
            # 注册文件不需要压缩。以前直接走命令。
            up_file_transfer_cmd(comm_cmd_proc.device_reg_up, comm_cmd_proc.notify, up_file)
            while comm_common.is_continue() and comm_data.up_cmd_info != None:
                time.sleep(0.1)
        else:
            break

#10G
def reg_dat():
    while comm_common.is_continue():
        if  comm_data.ismanager == '1':
            # ############wan#################
            reg_self = "reg_" + comm_data.uuid + ".ok"
            for reg_up in file_filter_bysuffix([comm_data.comm_reg_up_path], [".ok"]):
                reg_anyone = os.path.basename(reg_up)
                if reg_self == reg_anyone:
                    reg_self =  comm_data.comm_reg_up_tmp_path + reg_anyone
                    shutil.move(reg_up, reg_self)
            # ############wan#################
            #xxx =  comm_data.comm_reg_up_path + "reg_" + comm_data.uuid + ".ok"
            #yyy =  "/data/tmpdata/comm/reg/up/" + os.path.basename(up_file)
            #if xxx == up_file and comm_data.ismonitor == '1':
            #    shutil.move(yyy, up_file)  
            if (comm_reg.is_reg_done() == 1) : 
                reg_dat_file = comm_data.reg_dat_path + "reg.dat"
                # 获得本机注册文件dat的内容到内存里,
                # 因为is_reg_done*()已经判断，文件一定存在，而且正确, 
                reg_buf = comm_reg.get_regbuf_from_file(reg_dat_file)
                try:
                    #zhu yi
                    items = reg_buf.split("|")[7]
                    for reg_down in file_filter_bysuffix([comm_data.comm_reg_up_path], [".ok"]):
                            reg_buf = comm_reg.get_regbuf_from_file(reg_dat_file)
                            try:
                                # zu zhi nei rong
                                cloud_before = comm_reg.get_regbuf_from_file(reg_down).split("|")
                                cloud_before[7] = items
                                cloud_before[1] = '\N'
                                reg_file_ne = '|'.join(cloud_before)
                                # zai down xia chuangjian wen jian
                                if '|' in reg_file_ne : 
                                    reg_file = comm_data.comm_reg_down_path + os.path.basename(reg_down)
                                    fd = open(reg_file, "w")
                                    fd.write(reg_file_ne)
                                    fd.close()
                                    os.remove(reg_down)
                                    comm_log_record.logger.info("==ret==>CREATE REG OK![%s]" %reg_file)
                                else:
                                    comm_log_record.logger.info("==ret==>CREATE REG failed![%s]" %reg_file_ne)
                            except Exception as ef:
                                comm_log_record.logger.warn("add cloudid failed. exec auth scrip[reg_dat] fail. %s." %  str(ef))
                except Exception as ex:
                    comm_log_record.logger.warn("get cloudid failed. exec auth scrip[reg_dat] fail. %s." %  str(ex))
            else :
                    comm_log_record.logger.warn("reg dat is not exist. exec auth scrip[reg_dat] fail.")
        else:
             comm_log_record.logger.warn("this is not manager. exec auth scrip[reg_dat] .")
             break
                    
def send_up_pcap():
    while comm_common.is_continue():
        reg_up_file()

        if comm_reg.is_reg_done() == 0:
            # 如果没有注册成功，只上传注册文件
            time.sleep(0.1)
            continue
        travel_path(comm_data.comm_pcap_up_path, comm_cmd_proc.pcap_evt_up, suffix='.pcap')
                  

#注册文件传输
def reg_down_file():
    # 即使没注册成功也会遍历注册目录。因为需要在这里找到自己的注册文件
    # 如果没有注册成功不会走下面的流程。
    # 下级字节点的注册文件也不会下发，因为不注册成功不连接下级结点
    for down_file in file_filter_bysuffix([comm_data.comm_reg_down_path], [".ok"]):
        if not comm_common.is_continue():
            break

        # 1. 首先查看该注册文件是否是自身的注册文件
        try:
            reg_buf = comm_reg.get_regbuf_from_file(down_file)

            reg_uuid = reg_buf.split("|")[2]
            cloud_id = reg_buf.split("|")[7]

            if (reg_uuid == comm_data.uuid):
                comm_data.inc_recv_local_cnt(comm_cmd_proc.device_reg_down)
                # 2. 如果是自身的注册文件，则注册成功 生成reg.dat文件
                #. 查看cloud_id 是否为空,
                comm_log_record.logger.info('==reg==> process local reg file.')
                if (cloud_id == '\N'):
                    comm_log_record.logger.info("==reg==> local dev reg fail. cloud_id is NULL")
                else:
                    comm_log_record.logger.info("==reg==> local dev reg success. cloud_id [%s]" % cloud_id)

                try:
                    comm_reg.make_reg_dat_by_buf(reg_buf)
                    os.remove(down_file)
                    comm_log_record.logger.info("==reg==> create local dev reg file success.")
                    if cloud_id != '\N':
                        try:
                            ret, stdout, stderr = comm_common.system(str(comm_data.auth_script))
                            comm_log_record.logger.info("dev reg success. exec auth script[%s]: ret[%s] stdout[%s] stderr[%s]" % (comm_data.auth_script, ret, stdout, stderr))
                        except Exception as ex:
                            comm_log_record.logger.warn("dev reg success. exec auth script[%s] fail. %s." % (comm_data.auth_script, str(ex)))
                except IOError, ex:
                    comm_log_record.logger.info(ex)
                    comm_log_record.logger.info("==reg==> create local dev reg file fail.")
            else:
                # 3. 如果不是自身文件，根据UUID 查找拓扑图， 得到对应下级IP
                ip = device_manage.get_comm_topo_ip(reg_uuid)
                if (ip == "no_value"):
                    comm_log_record.logger.info("==reg==> child uuid[%s] is not in topo, delete it!");
                    os.remove(down_file)
                    continue

                # 因为这个函数可能在处理同一个注册文件的时候多次调用。所以在这里判断一下。
                for i in comm_data.sub_dev_list:
                    if i.ip == ip and i.down_cmd_info == None:
                        comm_log_record.logger.info('==reg==> process child reg file. uuid[%s] ip[%s]' % (reg_uuid, ip))
                        # 4. 查看cloud_id 是否为空,如果为空，则消除此topo
                        if (cloud_id == '\N'):
                            comm_log_record.logger.info("******************* cloud_id == '\n' *****************")
                            #del_comm_topo_ip(reg_uuid)

                        # 5. 根据特到的IP进行命令下发
                        down_file_transfer_cmd(ip, comm_cmd_proc.device_reg_down, comm_cmd_proc.notify, down_file)
                        break;

        except:
            comm_log_record.logger.info('%s' % traceback.format_exc())
            if os.path.exists(down_file):
                os.rename(down_file, down_file + '.%s' % time.time())

# 在顶级管理结点把通讯目录下的文件拷贝到入库目录下
def mv_upfile_to_dbdir():
    comm_log_record.logger.info("======== mv upfile to dbdir thread start. ==========")
    file_cnt = 0
    dump_cnt = 0
    error_cnt = 0
    xml_time = os.stat(PATH).st_mtime
    dir_conf_list = [
                {'src_dir': comm_data.comm_mtd_up_path,
                 'ali_dir': comm_data.comm_me_ali_up_path},

                {'src_dir': comm_data.comm_ve_up_path,
                 'db_dir': comm_data.db_ve_path,
                 'ali_dir': comm_data.comm_ve_ali_path,
                 'ftp_dir': comm_data.ftp_virus_path},

                {'src_dir': comm_data.comm_ae_up_path,
                 'db_dir':  comm_data.db_ae_path,
                 'ali_dir': comm_data.comm_ae_ali_path,
                 'ftp_dir': comm_data.ftp_abb_path},

                {'src_dir': comm_data.comm_surl_up_path,
                 'db_dir': comm_data.db_surl_path,
                 'ali_dir': comm_data.comm_surl_ali_path,
                 'ftp_dir': comm_data.ftp_surl_path},

                {'src_dir': comm_data.comm_flow_up_path,
                 'db_dir': comm_data.db_flow_path,
                 'ali_dir': comm_data.comm_flow_ali_path},
                # old evnet flow
                {'src_dir': comm_data.comm_old_event_flow_up_path,
                 'db_dir': comm_data.db_old_event_flow_path},

                {'src_dir': comm_data.comm_devstat_up_path,
                 'db_dir': comm_data.db_devstat_path,
                 'ali_dir': comm_data.comm_devstat_ali_path},

                {'src_dir': comm_data.comm_query_flow_up_path,
                 'ali_dir': comm_data.comm_query_flow_ali_path},

                {'src_dir': comm_data.comm_query_dns_up_path,
                 'ali_dir': comm_data.comm_query_dns_ali_path},

                {'src_dir': comm_data.comm_query_url_up_path,
                 'ali_dir': comm_data.comm_query_url_ali_path},
                ]

    if comm_data.intranet_mode != "0":
        # 只在启动的时候，判断内网模式，复制本机的 topo 文件一次
        import comm_init
        topo_file = comm_init.interface_conf_file
        if os.path.exists(topo_file):
            try:
                dev_ip = socket.ntohl(struct.unpack("I", socket.inet_aton(comm_data.dev_ip))[0])
                dstname = 'root_%d_%s_%s.xml' % (dev_ip, comm_data.comm_cmd_port, comm_data.comm_file_port)
                dstname = os.path.join(comm_data.comm_topo_up_src_path, dstname)
                safe_copy(topo_file, dstname)
            except:
                comm_log_record.logger.info(traceback.format_exc())
                error_cnt += 1
            finally:
                file_cnt += 1

    while comm_common.is_continue():
        if comm_data.intranet_mode != "0":
            # 内网模式的顶级结点(130)，没有界面，所以让通讯去拷贝这个东西
            for src_file in file_filter_bysuffix(comm_data.comm_topo_up_dst_path, [".xml"]):
                if comm_common.is_continue():
                    try:
                        safe_copy(src_file, comm_data.comm_topo_up_src_path)
                    except:
                        comm_log_record.logger.info(traceback.format_exc())
                        error_cnt += 1
                    finally:
                        os.remove(src_file)
                        file_cnt += 1

        for dir_conf in dir_conf_list:
            if not comm_common.is_continue():
                break

            for src_file in file_filter_bysuffix([dir_conf['src_dir']], [".ok"]):
                if not comm_common.is_continue():
                    break
                if not os.path.exists(src_file):
                    continue

                if comm_data.intranet_mode != "0":
                    if comm_data.isroot == '1' and dir_conf['src_dir'] == comm_data.comm_query_flow_up_path:
                        filename = os.path.basename(src_file)
                        basename = os.path.splitext(filename)[0]
                        dirname = os.path.join(comm_data.host_query_flow_dst_path, basename)
                        if os.path.exists(dirname):
                            comm_log_record.logger.warn('dir [%s] already exists.' % dirname)
                            try:
                                extractzip(src_file, dirname)
                                os.remove(src_file)
                            except:
                                comm_log_record.logger.warn(traceback.format_exc())
                                os.rename(src_file, src_file + '.bak')
                        else:
                            try:
                                os.makedirs(dirname)
                                extractzip(src_file, dirname)
                                os.remove(src_file)
                            except:
                                comm_log_record.logger.warn(traceback.format_exc())
                                os.rename(src_file, src_file + '.bak')
                        continue

                try:
                    if comm_data.ismanager == '0' and comm_data.isroot == '1':
                        # 单点模式执行这里
                        if comm_data.iscopytodb and dir_conf.has_key('db_dir') and dir_conf['src_dir'] != comm_data.comm_flow_up_path and dir_conf['src_dir'] != comm_data.comm_devstat_up_path:
                            # flow 和 devstat 已经入库了。这里不再入库。
                            comm_file_copy(src_file, dir_conf['db_dir'])
                        if comm_data.iscopytoftp and dir_conf.has_key('ftp_dir'):
                            comm_file_copy(src_file, dir_conf['ftp_dir'])
                        zip_file = os.path.join(dir_conf['ali_dir'], os.path.basename(src_file))
                        createzip(zip_file, src_file)
                    else:
                        #if dir_conf['src_dir'] != comm_data.comm_devstat_up_path  and comm_common.is_zipfile(src_file)==False:
                        if islocalstat(src_file) == False and comm_common.is_zipfile(src_file)==False:
                            createzip(src_file, src_file) 
                        if islocalstat(src_file)  and dir_conf['src_dir'] == comm_data.comm_mtd_up_path:
                            createzip(src_file, src_file) 
                        if comm_data.iscopytodb and dir_conf.has_key('db_dir'):
                            #if dir_conf['src_dir'] == comm_data.comm_devstat_up_path  and islocalstat(src_file):
                            if (dir_conf['src_dir'] == comm_data.comm_devstat_up_path or dir_conf['src_dir'] == comm_data.comm_flow_up_path) and islocalstat(src_file):
                                pass
                            else:
                                comm_log_record.logger.info(src_file)
                                extractzip(src_file, dir_conf['db_dir'])
                        if comm_data.iscopytoftp and dir_conf.has_key('ftp_dir'):
                            extractzip(src_file, dir_conf['ftp_dir'])
                        comm_log_record.logger.info("delete*22******************* %s" % src_file)

                        #if dir_conf['src_dir'] == comm_data.comm_devstat_up_path and islocalstat(src_file):
                        if (dir_conf['src_dir'] == comm_data.comm_devstat_up_path or dir_conf['src_dir'] == comm_data.comm_flow_up_path) and comm_common.is_zipfile(src_file)==False and islocalstat(src_file):
                            zip_file = os.path.join(dir_conf['ali_dir'], os.path.basename(src_file))
                            createzip(zip_file, src_file)
                        else:
                            comm_file_copy(src_file, dir_conf['ali_dir'])
                        if dir_conf['src_dir'] == comm_data.comm_old_event_flow_up_path and not islocalstat(src_file):
                            comm_log_record.logger.info(src_file)
                            extractzip(src_file, dir_conf['db_dir'])

                    if dir_conf['src_dir'] == comm_data.comm_devstat_up_path and islocalstat(src_file):
                        #comm_log_record.logger.info("[%s]send device stat email." % xml_time)
                        xml_time = stat_email(xml_time, src_file)
                except:
                    comm_log_record.logger.info(traceback.format_exc())
                    error_cnt += 1
                finally:
                    comm_log_record.logger.info("delete******************** %s" % src_file)
                    if dir_conf['src_dir'] == comm_data.comm_ve_up_path:
                        send_vds_via_syslog(src_file)
                    elif dir_conf['src_dir'] == comm_data.comm_ae_up_path:
                        send_apt_via_syslog(src_file)
                    os.remove(src_file)
                    file_cnt += 1

            try:
                if stat_path():
                    #comm_log_record.logger.info("[%s]send device stat email." % xml_time)
                    xml_time = stat_email(xml_time)
            except Exception,ex:
                comm_log_record.logger.info('stat email:%s' % traceback.format_exc())
             
            for sqlchop_file in file_filter_bysuffix(comm_data.comm_sqlchop_up_path, [".ok"]):
                try:
                    if comm_common.is_zipfile(sqlchop_file)==False:
                        createzip(sqlchop_file,sqlchop_file)
                    extractzip(sqlchop_file, comm_data.db_mtdatt_path)
                    #extractzip(sqlchop_file, comm_data.comm_mtdall_down_path)
                    if comm_data.isroot == "1" or comm_data.link == "0":
                        os.remove(sqlchop_file)
                except Exception, ex:
                    comm_log_record.logger.info("Error:%s" % ex )
                    pass
        dump_cnt += 1
        if dump_cnt == 600:
           dump_cnt = 0
           comm_log_record.logger.info("---------- mv file cnt[%d]. error cnt[%d] ----------" % (file_cnt, error_cnt))
        time.sleep(0.1)

# LOCAL2
def send_apt_via_syslog(fname):
    def get_line_level(line):
        return syslog.LOG_ERR

    syslog.openlog('apt', 0, syslog.LOG_LOCAL2)
    send_file_via_syslog(fname, get_line_level)
    syslog.closelog()

# LOCAL1
def send_vds_via_syslog(fname):
    def get_line_level(line):
        level_dict = {
                '5': syslog.LOG_EMERG,
                '4': syslog.LOG_ALERT,
                '3': syslog.LOG_ALERT,
                '2': syslog.LOG_CRIT,
                '1': syslog.LOG_CRIT,
                '0': syslog.LOG_ERR,
            }
        try:
            risk = line.split('|')[14].strip()
            return level_dict[risk]
        except:
            comm_log_record.logger.warn("get vds syslog level fail. %s" % traceback.format_exc())
            return level_dict['0']

    syslog.openlog('vds', 0, syslog.LOG_LOCAL1)
    send_file_via_syslog(fname, get_line_level)
    syslog.closelog()

# LOCAL0
def send_mtd_via_syslog(fname):
    def get_line_level(line):
        level_dict = {
                '5': syslog.LOG_EMERG,
                '4': syslog.LOG_ALERT,
                '3': syslog.LOG_ALERT,
                '2': syslog.LOG_CRIT,
                '1': syslog.LOG_CRIT,
                '0': syslog.LOG_ERR,
            }
        try:
            risk = line.split('|')[17].strip()
            return level_dict[risk]
        except:
            comm_log_record.logger.warn("get mtd syslog level fail. %s" % traceback.format_exc())
            return level_dict['0']

    syslog.openlog('mtd', 0, syslog.LOG_LOCAL0)
    send_file_via_syslog(fname, get_line_level)
    syslog.closelog()

def send_file_via_syslog(fname, get_line_level):
    def read_lines(fname):
        try:
            lines = []
            if comm_common.is_zipfile(fname):
                data = comm_common.readzip(fname)
                lines = data.split('\n')
            else:
                with open(fname) as fp:
                    lines = fp.readlines()
            return lines
        except:
            comm_log_record.logger.warn("%s" % traceback.format_exc())
            return []

    # 暂时确定只有根结点发送 syslog，在这个函数里边判断。
    # 如果将来所有类型结点都发送，只要把这句注释掉就可以了。
    if comm_data.isroot == '1':
        for line in read_lines(fname):
            if line.strip() != '':
                #comm_log_record.logger.warn("syslog: %s %s" % (get_line_level(line), line))
                syslog.syslog(get_line_level(line), line)

if __name__ == "__main__":
    mv_upfile_to_dbdir()

