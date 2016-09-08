/** 
 * 版权所有? 2010-2013
 */
package com.chanct.gms.listenser;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.charset.Charset;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.mina.core.service.IoAcceptor;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.filter.codec.textline.TextLineCodecFactory;
import org.apache.mina.filter.logging.LoggingFilter;
import org.apache.mina.transport.socket.nio.NioSocketAcceptor;

/**
 * <p>
 * 类名:MinaLongConnServer
 * </p>
 * <p>* 描述:
 * </p>
 * <p>* 版权声明: Copyright (c) 2010-2013
 * </p>
 * 
 * @author
 * @Date Aug 16, 2013
 */
public class UpgradeMessageWatcher {

    /**
     * 日志
     */
    private static final Log log = LogFactory.getLog(UpgradeMessageWatcher.class);

    /**
     * 端口
     */
    private Integer port;

    public UpgradeMessageWatcher(Integer port) {
        this.port = port;
    }

    public void start() throws IOException {
        IoAcceptor acceptor = new NioSocketAcceptor();
        acceptor.getFilterChain().addLast("logger", new LoggingFilter());
        acceptor.getFilterChain().addLast("codec", new ProtocolCodecFilter(new TextLineCodecFactory(Charset.forName("UTF-8"))));
        MinaConnServerHandle minaHandle = new MinaConnServerHandle();
        acceptor.setHandler(minaHandle);
        acceptor.getSessionConfig().setReadBufferSize(2048);
        acceptor.getSessionConfig().setIdleTime(IdleStatus.BOTH_IDLE, 10);
        acceptor.bind(new InetSocketAddress(this.port));
        if (log.isInfoEnabled()) {
            log.info("开始监听端口 " + this.port);
        }
    }
}
