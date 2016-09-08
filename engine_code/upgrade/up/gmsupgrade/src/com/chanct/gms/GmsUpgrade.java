package com.chanct.gms;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.chanct.gms.config.Global;
import com.chanct.gms.listenser.DirectoryWatcher;
import com.chanct.gms.listenser.FileListener;
import com.chanct.gms.listenser.UpgradeMessageWatcher;
import com.chanct.gms.parser.FileParser;
import com.chanct.gms.parser.UnPackParser;
import com.chanct.gms.parser.UpgradeWatcher;
import com.chanct.gms.utile.DBUtiles;

/**
 * 
 * @ClassName: GmsUpgrade
 * @Description: GMS升级生成
 * @author shf
 * @date Dec 9, 2013 11:10:34 AM
 * 
 */
public class GmsUpgrade {
    /** log日志 */
    private static final Log log = LogFactory.getLog(DBUtiles.class);

    /** 文件解析器 */
    private static FileParser fileParser = new FileParser();

    /** 解包解析器 */
    private static UnPackParser unPackParser = new UnPackParser();

    /** 升级指令监听器 */
    @SuppressWarnings("unused")
    private static UpgradeMessageWatcher socketListener;

    /**
     * 
     * @Title: main
     * @Description: 升级程序入口
     * @param
     * @param args
     * @return void 返回类型
     * @throws
     */
    public static void main(String[] args) {
        init();
    }

    /**
     * 
     * @Title: init
     * @Description: 程序初始化
     * @param
     * @return void 返回类型
     * @throws
     */
    private static void init() {
        // 获取程序执行的根目录
        try {
            Global.HOMEDIR = getHomeDir();
        } catch (URISyntaxException e) {
            if (log.isInfoEnabled()) {
                log.info("获取程序启动跟路径异常. 原因: " + e.getMessage());
            }
            System.exit(1);
        }

        // 加载并解析升级程序配置文件
        try {
            if (!fileParser.loadConfFile(Global.HOMEDIR + Global.CONFIG)) {
                if (log.isInfoEnabled()) {
                    log.info("读取升级程序配置文件conf.xml失败. ");
                }
                System.exit(1);
            }
            if (log.isInfoEnabled()) {
                log.info("成功加载升级程序配置文件conf.xml. ");
            }
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("加载升级程序配置文件conf.xml异常. " + e.getMessage());
            }
            System.exit(1);
        }

        // 初始化目录(解包目录、备份目录、升级包目录)、初始化系统版本
        try {
            fileParser.inits();
            if (log.isInfoEnabled()) {
                log.info("初始化所有目录成功(upgrade/unpak/back...). ");
            }
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("初始化所有目录(upgrade/unpak/back...)失败. ");
            }
            System.exit(1);
        }

        // 启动升级包目录看守
        try {
            DirectoryWatcher dw = new DirectoryWatcher(Global.PACK_DIRECTORY, 10);
            dw.addListener(new FileListener());
            dw.setUnPackParser(unPackParser);
            dw.start();
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("升级包目录看守启动失败. 原因: " + e.getMessage());
            }
            System.exit(1);
        }

        // 启动升级指令看守
        UpgradeMessageWatcher upgradeWatcher = new UpgradeMessageWatcher(Global.COMM_PORT);
        try {
            upgradeWatcher.start();
        } catch (IOException e) {
            if (log.isInfoEnabled()) {
                log.info("升级指令看守异常. 原因: " + e.getMessage());
            }
        }

        // 启动升级程序看守
        ScheduledExecutorService scheduledExecutorService = Executors.newScheduledThreadPool(1);
        scheduledExecutorService.scheduleAtFixedRate(new UpgradeWatcher(), 5, 10, TimeUnit.SECONDS);
    }

    /**
     * 
     * @Title: getHomeDir
     * @Description: 获取当前程序的执行路径
     * @param
     * @return
     * @return String 返回类型
     * @throws URISyntaxException
     * @throws
     */
    private static String getHomeDir() throws URISyntaxException {
        StringBuilder sb = new StringBuilder();
        URL path = GmsUpgrade.class.getProtectionDomain().getCodeSource().getLocation();
        File f = new File(path.toURI());
        if (f.getName().endsWith(".jar"))
            sb.append(f.getParentFile().getAbsolutePath());
        else {
            sb.append(f.getParentFile().getAbsolutePath());
        }
        sb.append(File.separator);
        return sb.toString();
    }

}
