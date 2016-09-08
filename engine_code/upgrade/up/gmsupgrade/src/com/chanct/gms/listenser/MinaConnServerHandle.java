/** 
 * 版权所有? 2010-2013
 */
package com.chanct.gms.listenser;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.core.service.IoHandlerAdapter;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.UpModel;
import com.chanct.gms.utile.Utiles;

/**
 * <p>
 * 类名:MinaLongConnServerHandle
 * </p>
 * <p>* 描述:
 * </p>
 * <p>* 版权声明: Copyright (c) 2010-2013
 * </p>
 * 
 * @author
 * @Date Aug 16, 2013
 */
public class MinaConnServerHandle extends IoHandlerAdapter {

    /** log日志 */
    private static final Log log = LogFactory.getLog(MinaConnServerHandle.class);

    @Override
    public void sessionOpened(IoSession session) {
        if (log.isInfoEnabled()) {
            log.info("获得新升级指令连接 " + session.getId());
        }
    }

    @Override
    public void messageReceived(IoSession session, Object message) {
        if (message != null) {
            String msg = message.toString();
            if (log.isInfoEnabled()) {
                log.info("接收到升级指令[" + msg + "]");
            }
            if (msg.substring(0, 4).equals("old:")) {
                String old_Ver = msg.split(" ")[0];
                String new_Ver = msg.split(" ")[1];
                if (new_Ver.substring(0, 4).equals("new:")) {
                    if (log.isInfoEnabled()) {
                        log.info("升级指令格式验证成功");
                    }
                    String oVer = old_Ver.split(":")[1];
                    String nVer = new_Ver.split(":")[1];
                    UpModel up = new UpModel();
                    up.setNewVersion(nVer);
                    up.setOldVersion(oVer);
                    up.setUpMode(Global.UPMode_NORMAOL);
                    if (!Utiles.contains(Global.upQueue, up)) {
                        Global.upQueue.offer(up);
                        if (log.isInfoEnabled()) {
                            log.info("添加到升级队列. ");
                        }
                    }
                }
                //session.write("ok");
            }
        }
    }

    @Override
    public void sessionIdle(IoSession session, IdleStatus status) {
    }

    @Override
    public void exceptionCaught(IoSession session, Throwable cause) {
        if (log.isInfoEnabled()) {
            log.info("监听升级指令异常: " + cause.getMessage());
        }
    }
}
