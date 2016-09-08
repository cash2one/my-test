# _*_ coding: utf8 _*_

import os
import time
import socket
import struct
import shutil
import zipfile
import traceback
import subprocess
import comm_data
import comm_file_tran
import comm_cmd_proc
import comm_reg
import comm_common
import device_manage
import comm_log_record
from comm_common import file_filter_bysuffix, createzip, extractzip, listzip, comm_file_copy, safe_copy

def up_file_transfer_cmd(filetype, eventtype, filename):
    if comm_data.up_cmd_info == None:
        comm_data.up_cmd_info = comm_cmd_proc.comm_make_cmdinfo(filetype, eventtype, filename)
        #comm_log_record.logger.debug("make up file cmd [%s]" % comm_data.up_cmd_info)
    else:
        # 如果走到这里说明上面的某些流程出问题了。
        comm_log_record.logger.warn("make up file cmd. up_cmd_info != None" % comm_data.up_cmd_info)

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
        if zipfile.is_zipfile(mtx_conf_file) and len(listzip(mtx_conf_file)) == 1:
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

    elif comm_data.comm_mtdatt_down_path in fname:
        db_dir = comm_data.db_mtdatt_path
        ftp_dir = comm_data.ftp_mtdatt_path
        bak_dir = comm_data.comm_me_backup_path
        copytodbftp(fname, db_dir, ftp_dir, bak_dir)

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
                    if comm_data.iscopytoftp == True:
                        extractzip(down_file, comm_data.ftp_mtd3rd_path)
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
                    comm_log_record.logger.info("extract zipfile[%s] fail. backup to dir[%s]" % (down_file, comm_data.comm_me_backup_path))
                    shutil.copy(down_file, comm_data.comm_me_backup_path)
                finally:
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
                    if comm_data.iscopytodb == True:
                        extractzip(down_file, comm_data.db_mtdatt_path)
                    if comm_data.iscopytoftp == True:
                        extractzip(down_file, comm_data.ftp_mtdatt_path)
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
                    comm_log_record.logger.info("extract zipfile[%s] fail. backup to dir[%s]" % (down_file, comm_data.comm_me_backup_path))
                    shutil.copy(down_file, comm_data.comm_me_backup_path)
                finally:
                    os.remove(down_file)
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.mtx_evt_down, comm_cmd_proc.notify, down_file)

        for down_file in file_filter_bysuffix([comm_data.comm_ue_down_path], [".ok"]):
            if not comm_common.is_continue():
                break
            reg_down_file()
            sub_dev_uuid = os.path.splitext(os.path.basename(down_file))[0].split('_')[1]
            if sub_dev_uuid == comm_data.uuid:
                try:
                    comm_data.inc_recv_local_cnt(comm_cmd_proc.ue_file_down)
                    # 对 GMS-1.0.0.2-7b5d04c86c822bf5e73548540187c985-emergency_f9d86946-21fd-11e4-b78e-d5007112775c.ok 形式的文件名
                    # 解压缩，重命名为 GMS-1.0.0.2-7b5d04c86c822bf5e73548540187c985-emergency 放到 /gms/updata/pack 目录下。
                    # 这里依赖压缩包和原文件同名。否则会出现压缩包 a.ok 解出 b.ok ，然后把 a.ok 移到 pack 目录下，下一轮 b.ok uuid 无效。
                    extractzip(down_file, comm_data.comm_ue_down_path)
                    dst_file = os.path.join(comm_data.comm_ue_ui_path, os.path.splitext(os.path.basename(down_file))[0].split('_')[0])
                    shutil.copy(down_file, dst_file)
                    os.remove(down_file)
                except zipfile.BadZipfile:
                    comm_log_record.logger.info("File[%s] is not a zip file" % down_file)
                    if os.path.exists(down_file):
                        os.rename(down_file, down_file + '.bak')
                except Exception:
                    comm_log_record.logger.info(traceback.format_exc())
            else:
                sub_dev_ip = device_manage.get_comm_topo_ip(sub_dev_uuid)
                if sub_dev_ip == "no_value":
                    comm_log_record.logger.info("=========> get addr from file[%s] fail. delete." % down_file)
                    os.remove(down_file)
                else:
                    down_file_transfer_cmd(sub_dev_ip, comm_cmd_proc.ue_file_down, comm_cmd_proc.notify, down_file)

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

def travel_path(path, filetype, suffix=".ok", eventtype=comm_cmd_proc.notify):
    for up_file in file_filter_bysuffix([path], [suffix]):
        if comm_common.is_continue():
            # 确保有注册文件的时候，先传注册文件
            reg_up_file()

            if not os.path.exists(up_file):
                # 最可能的情况是取到几百个文件，然后测试时手动删除了文件，
                # 在这里进行判断，不然会连续报出几百个异常
                continue

            try:
                if filetype == comm_cmd_proc.devstat_evt_up and islocalstat(up_file):
                    if zipfile.is_zipfile(up_file):
                        comm_log_record.logger.info("!!! up_file[%s] is zip file." % up_file)
                    else:
                        createzip(up_file, up_file)
                elif comm_data.ismonitor == '1' and filetype != comm_cmd_proc.topo_evt_up:
                    # topo 文件不压缩。界面无法解压缩。
                    if zipfile.is_zipfile(up_file):
                        comm_log_record.logger.info("!!! up_file[%s] is zip file." % up_file)
                    else:
                        createzip(up_file, up_file)

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
    dev_uuid = os.path.splitext(os.path.basename(fname))[0].split('_')[1]
    if dev_uuid == comm_data.uuid:
        return True
    else:
        return False

def send_up_file_seccuss_action(fname):
    def copytodbftp(fname, dbpath, ftppath):
        try:
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
    elif comm_data.comm_ae_up_path in fname:
        copytodbftp(fname, comm_data.db_ae_path, comm_data.ftp_abb_path)
    elif comm_data.comm_surl_up_path in fname:
        copytodbftp(fname, comm_data.db_surl_path, comm_data.ftp_surl_path)
    elif comm_data.comm_devstat_up_path in fname and not islocalstat(fname):
        # 设备状态在检测点已经入库
        copytodbftp(fname, comm_data.db_devstat_path, None)
    elif comm_data.comm_flow_up_path in fname and comm_data.ismonitor != '1':
        # 流量在检测点已经入库
        copytodbftp(fname, comm_data.db_flow_path, None)

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

        travel_path(comm_data.comm_surl_up_path, comm_cmd_proc.surl_evt_up)

        travel_path(comm_data.comm_query_flow_up_path, comm_cmd_proc.query_flow_up_evt)

        travel_path(comm_data.comm_query_dns_up_path, comm_cmd_proc.query_dns_up_evt)

        travel_path(comm_data.comm_query_url_up_path, comm_cmd_proc.query_url_up_evt)

def reg_up_file():
    for up_file in file_filter_bysuffix([comm_data.comm_reg_up_path], [".ok"]):
        if comm_common.is_continue():
            # 注册文件不需要压缩。以前直接走命令。
            up_file_transfer_cmd(comm_cmd_proc.device_reg_up, comm_cmd_proc.notify, up_file)
            while comm_common.is_continue() and comm_data.up_cmd_info != None:
                time.sleep(0.1)
        else:
            break

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
                try:
                    if comm_data.ismonitor == '1' and comm_data.isroot == '1':
                        # 单点模式执行这里
                        if comm_data.iscopytodb and dir_conf.has_key('db_dir') and dir_conf['src_dir'] != comm_data.comm_flow_up_path and dir_conf['src_dir'] != comm_data.comm_devstat_up_path:
                            # flow 和 devstat 已经入库了。这里不再入库。
                            comm_file_copy(src_file, dir_conf['db_dir'])
                        if comm_data.iscopytoftp and dir_conf.has_key('ftp_dir'):
                            comm_file_copy(src_file, dir_conf['ftp_dir'])
                        zip_file = os.path.join(dir_conf['ali_dir'], os.path.basename(src_file))
                        createzip(zip_file, src_file)
                    else:
                        if comm_data.iscopytodb and dir_conf.has_key('db_dir'):
                            if dir_conf['src_dir'] == comm_data.comm_devstat_up_path and islocalstat(src_file):
                                pass
                            else:
                                extractzip(src_file, dir_conf['db_dir'])
                        if comm_data.iscopytoftp and dir_conf.has_key('ftp_dir'):
                            extractzip(src_file, dir_conf['ftp_dir'])

                        if dir_conf['src_dir'] == comm_data.comm_devstat_up_path and islocalstat(src_file):
                            zip_file = os.path.join(dir_conf['ali_dir'], os.path.basename(src_file))
                            createzip(zip_file, src_file)
                        else:
                            comm_file_copy(src_file, dir_conf['ali_dir'])
                except:
                    comm_log_record.logger.info(traceback.format_exc())
                    error_cnt += 1
                finally:
                    os.remove(src_file)
                    file_cnt += 1

        dump_cnt += 1
        if dump_cnt == 600:
           dump_cnt = 0
           comm_log_record.logger.info("---------- mv file cnt[%d]. error cnt[%d] ----------" % (file_cnt, error_cnt))
        time.sleep(0.1)

if __name__ == "__main__":
    mv_upfile_to_dbdir()

