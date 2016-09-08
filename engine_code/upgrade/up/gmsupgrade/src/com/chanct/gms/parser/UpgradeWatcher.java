package com.chanct.gms.parser;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Deque;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.locks.ReentrantLock;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;

import com.chanct.gms.config.Global;
import com.chanct.gms.db.DbServiceImpl;
import com.chanct.gms.db.IDbService;
import com.chanct.gms.model.Action;
import com.chanct.gms.model.ConfigFileModel;
import com.chanct.gms.model.UpModel;
import com.chanct.gms.model.VersionModel;
import com.chanct.gms.utile.Utiles;
import com.chanct.gms.utile.XMLUtiles;

/**
 * 
 * @ClassName: UpgradeWatcher
 * @Description: 升级看守
 * @author shf
 * @date Dec 20, 2013 4:37:13 PM
 * 
 */
public class UpgradeWatcher implements Runnable {

    /** log日志 */
    private static final Log log = LogFactory.getLog(UpgradeWatcher.class);
    /** 防重入锁 */
    private ReentrantLock lock = new ReentrantLock();
    /** 回滚列表 */
    private List<Action> rollbackActions = new ArrayList<Action>();
    /** 数据库服务 */
    private IDbService dbService = new DbServiceImpl();

    /** 跨版本升级栈 */
    private static Deque<UpModel> crossVerStack = new LinkedBlockingDeque<UpModel>();

    @Override
    public void run() {
        lock.lock();
        try {
            if (lock.isLocked()) {
                if (!Global.upQueue.isEmpty()) {
                    UpModel up = Global.upQueue.peek();
                    /** 判断是否跨版本升级 */
                    if (Utiles.compareVersion(up.getOldVersion(), Global.VERSION) < 0 || Utiles.compareVersion(up.getNewVersion(), Global.VERSION) < 0) {// 升级版本过低
                        Utiles.remove(Global.upQueue, up);
                        if (log.isInfoEnabled()) {
                            log.info("升级过程失败. 原因: 目标版本或目标版本的依赖版本不能低于系统当前版本.");
                        }
                        return;
                    } else if (Utiles.compareVersion(up.getNewVersion(), Global.VERSION) == 1) {// 非跨版本正常升级
                        doUpOnceVersion(up);
                    } else {// 跨版本升级
                        if (log.isInfoEnabled()) {
                            log.info("=======跨版本升级:" + Global.VERSION + " To " + up.getNewVersion() + "==========");
                        }
                        if (!crossVerStack.isEmpty()) {
                            crossVerStack.clear();
                        }
                        crossVerStack.push(up);
                        getCrossVersion(up);
                        doCrossUpgrade(crossVerStack);
                    }
                }
            }
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("升级失败. 原因:" + e.getMessage());
            }
        } finally {
            lock.unlock();
        }
    }

    /**
     * 
     * @Title: doCrossUpgrade
     * @Description: 执行跨版本升级
     * @param
     * @param crossVerStack
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void doCrossUpgrade(Deque<UpModel> crossVerStack) throws Exception {
        if (log.isInfoEnabled()) {
            log.info("开始执行跨版本升级......");
        }
        UpModel temTop=crossVerStack.peek();
      	 if (log.isInfoEnabled()) {
               log.info("获取最后压站的UpModel、、、、、、、、、、、"+ temTop.getOldVersion() );
           }
      	if(Utiles.compareVersion(temTop.getOldVersion(), Global.VERSION) != 0){
      		 
      		 if (log.isInfoEnabled()) {
                   log.info("跨版本递归,获取依赖版本不全！"  );
               }
      		 crossVerStack.clear();
      		 Utiles.remove(Global.upQueue, temTop);
             updateUpgradeStatus(dbService, temTop, Global.UP_STATUS_FAILT);
      		 throw new RuntimeException("获取依赖版本不全");
      	}
        while (!crossVerStack.isEmpty()) {
            UpModel task = crossVerStack.pop();
            doUpOnceVersion(task);
        }
    }

    /**
     * 
     * @Title: getCrossVersion
     * @Description: 针对跨版本,递归找到依赖版本
     * @param
     * @param up
     * @param
     * @return
     * @return void 返回类型
     * @throws
     */
    private void getCrossVersion(UpModel up) throws Exception {
        if (Global.unPackMap.containsKey(up.getOldVersion())) {
            try {
                ConfigFileModel conf = getUpConfFileModel(Global.unPackMap.get(up.getOldVersion()) + "/" + Global.UPGRADE_FILE_NAME);
                UpModel temp = null;
                if (Utiles.compareVersion(conf.getOldVersion(), Global.VERSION) >= 0) {
                    temp = new UpModel();
                    temp.setNewVersion(conf.getSoftVersion());
                    temp.setOldVersion(conf.getOldVersion());
                    temp.setUpMode(conf.getUpMode());
                    if (log.isInfoEnabled()) {
                        log.info("跨版本递归,获取依赖版本 " + conf.getOldVersion() + ".");
                    }
                    crossVerStack.push(temp);
                    getCrossVersion(temp);
               }
                
            } catch (Exception e) {
                if (log.isInfoEnabled()) {
                    throw e;
                }
            }
        }
    }

    /**
     * 
     * @Title: doUpOnceVersion
     * @Description: 执行一次版本升级
     * @param
     * @param up
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void doUpOnceVersion(UpModel up) throws Exception {
        ConfigFileModel conf = null;
        try {
            conf = getUpConfFileModel(Global.UNPACKING_DIRECTORY + up.getNewVersion() + "/" + Global.UPGRADE_FILE_NAME);
            if (conf == null) {
                Utiles.remove(Global.upQueue, up);
                updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
                return;
            }
        } catch (Exception e) {
            Utiles.remove(Global.upQueue, up);
            if (log.isInfoEnabled()) {
                log.info("升级过程失败. 原因: 解析升级配置文件失败. ");
            }
            updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
            return;
        }

        try {
            if (!checkSpace(conf.getMainDir(), conf.getMinSpace()))
                return;
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("升级过程中磁盘验证异常. " + e.getMessage());
            }
            updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
            return;
        }

        try {
            if (!backFile(conf)) {
                Utiles.remove(Global.upQueue, up);
                updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
                return;
            }
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("升级过程中备份文件异常. " + e.getMessage());
            }
            updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
            return;
        }

        try {
            if (!upProcess(conf)) {
                if (log.isInfoEnabled()) {
                    log.info("升级过程失败. 从版本" + up.getOldVersion() + "到" + up.getNewVersion());
                }
                updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
                throw new Exception("升级失败.");
            } else {
                if (log.isInfoEnabled()) {
                    log.info("升级过程成功. 从版本" + up.getOldVersion() + "到" + up.getNewVersion());
                }
                Utiles.remove(Global.upQueue, up);
                updateUpgradeStatus(dbService, up, Global.UP_STATUS_OK);
                updateSysVersion(dbService, up.getNewVersion());
                // 删除备份文件
                FileUtils.forceDelete(new File(Global.BACK_DIRECTORY + conf.getSoftVersion()));
                return;
            }
        } catch (Exception e) {
            if (log.isInfoEnabled()) {
                log.info("升级失败. 原因:" + e.getMessage());
            }
            Utiles.remove(Global.upQueue, up);
            updateUpgradeStatus(dbService, up, Global.UP_STATUS_FAILT);
            roolBack(rollbackActions, conf);
        }
    }

    /**
     * 
     * @Title: doAlertUpStatus
     * @Description: 更新数据库升级状态
     * @param
     * @param dbService
     * @param
     * @param up
     * @param
     * @param status
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void updateUpgradeStatus(IDbService dbService, UpModel up, Integer status) throws Exception {
        VersionModel ver = new VersionModel();
        ver.setUpStatus(status);
        ver.setOldVersion(up.getOldVersion());
        ver.setNewVersion(up.getNewVersion());
        dbService.updateUpStatus(ver);
        if (log.isInfoEnabled()) {
            log.info("更新升级状态为" + status);
        }
    }

    /**
     * 
     * @Title: doUpdateSysVersion
     * @Description: 更新版本信息(数据库和系统配置文件)
     * @param
     * @param version
     * @param dbService
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void updateSysVersion(IDbService dbService, String version) throws Exception {
        if (StringUtils.isEmpty(version)) {
            return;
        }
        dbService.updateSysVersion(version);
        if (log.isInfoEnabled()) {
            log.info("更新数据库系统版本为" + version);
        }

        // 更新配置文件的版本号
        File verFile = new File(Global.SYS_VERSION_FILE);
        SAXReader saxReader = new SAXReader();
        Document document = saxReader.read(verFile);
        document.getRootElement().element(Global.SYS_VERSION).setText(version);

        Writer writer = new FileWriter(verFile);
        OutputFormat format = OutputFormat.createPrettyPrint();// 格式化
        XMLWriter xmlWriter = new XMLWriter(writer, format);
        xmlWriter.write(document);
        xmlWriter.close();
        if (log.isInfoEnabled()) {
            log.info("更新系统配置文件中系统版本为" + version);
        }

        Global.VERSION = version;
        if (log.isInfoEnabled()) {
            log.info("更新全局版本变量为" + version);
        }
    }

    /**
     * 
     * @Title: getUpConfFileModel
     * @Description: 解析升级配置文件
     * @param
     * @param filePath
     * @param
     * @return
     * @return ConfigFileModel 返回类型
     * @throws Exception
     */
    @SuppressWarnings("unchecked")
    private ConfigFileModel getUpConfFileModel(String filePath) throws Exception {
        Document doc = XMLUtiles.readXML(filePath);
        Element root = doc.getRootElement();
        ConfigFileModel fileModel = new ConfigFileModel();
        int tag = Utiles.compareVersion(XMLUtiles.getElementValue(root, "SoftVersion"), XMLUtiles.getElementValue(root, "OldVersion"));
        if (tag < 1) {
            if (log.isInfoEnabled()) {
                log.info("目标版本不能低于系统当前版本.");
            }
            return null;
        }

        fileModel.setMainDir(XMLUtiles.getElementValue(root, "MainDir"));
        fileModel.setOldVersion(XMLUtiles.getElementValue(root, "OldVersion"));
        fileModel.setSoftVersion(XMLUtiles.getElementValue(root, "SoftVersion"));
        fileModel.setPackageType(XMLUtiles.getElementValue(root, "PackageType"));
        fileModel.setUpMode(XMLUtiles.getElementValue(root, "UpMode"));
        fileModel.setMinSpace(Integer.parseInt(XMLUtiles.getElementValue(root, "MinSpace")));

        List acTemps = XMLUtiles.getElements(XMLUtiles.getElement(root, "Actions"), "Action");
        List<Action> actions = new ArrayList<Action>();
        Element acEle;
        Action action;
        for (Iterator it = acTemps.iterator(); it.hasNext();) {
            acEle = (Element) it.next();
            action = new Action();
            action.setName(XMLUtiles.getElementAttribute(acEle, "name"));
            action.setValue(XMLUtiles.getElementAttribute(acEle, "value"));
            actions.add(action);
        }
        fileModel.setActions(actions);
        fileModel.setReturnAction(XMLUtiles.getElementValue(root, "Return"));
        fileModel.setDescription(XMLUtiles.getElementValue(root, "Description"));
        return fileModel;
    }

    /**
     * 
     * @Title: checkSpace
     * @Description: 磁盘空间验证
     * @param
     * @param dir
     * @param
     * @param minSpace
     * @param
     * @return
     * @return boolean 返回类型
     * @throws Exception
     */
    private boolean checkSpace(String dir, Integer minSpace) throws Exception {
        Double freeSpace = Utiles.getFreeSpace(dir);
        if (freeSpace.intValue() <= minSpace) {
            if (log.isInfoEnabled()) {
                log.info("升级过程中磁盘空间验证未通过. ");
            }
            return false;
        }
        if (log.isInfoEnabled()) {
            log.info("升级过程中磁盘空间验证通过. ");
        }
        return true;
    }

    /**
     * 
     * @Title: backFile
     * @Description: 执行备份
     * @param
     * @param conf
     * @param
     * @return
     * @return boolean 返回类型
     * @throws Exception
     */
    private boolean backFile(ConfigFileModel conf) throws Exception {
        if (conf == null) {
            if (log.isInfoEnabled()) {
                log.info("备份过程中,升级配置文件加载失败.");
            }
            return false;
        }
        rollbackActions.clear();

        // 创建备份目录
        String backDir = Global.BACK_DIRECTORY + conf.getSoftVersion();
        File backDirFile = new File(backDir);
        FileUtils.forceMkdir(backDirFile);

        // 执行备份
        List<Action> actions = conf.getActions();
        if (actions != null && actions.size() > 0) {
            for (Action ac : actions) {
                if (ac.getName().equals(Global.ACTION_DEL)) {// delete->add
                    File f = new File(ac.getValue());
                    Action roolAction = new Action();
                    roolAction.setName(Global.ACTION_ADD);
                    roolAction.setValue(ac.getValue());
                    if (f.isFile()) {
                        FileUtils.copyFileToDirectory(f, backDirFile);
                        rollbackActions.add(roolAction);
                    } else if (f.isDirectory()) {
                        FileUtils.copyDirectoryToDirectory(f, backDirFile);
                        rollbackActions.add(roolAction);
                    } else {
                        if (log.isInfoEnabled()) {
                            log.info("备份失败,文件不存在[" + ac.getValue() + "]");
                        }
                        return false;
                    }
                    continue;
                }
                if (ac.getName().equals(Global.ACTION_REPLACE)) {// replace->del&add
                    File f = new File(ac.getValue());
                    Action roolAction = new Action();
                    roolAction.setName(Global.ACTION_REPLACE);
                    roolAction.setValue(ac.getValue());
                    if (f.isFile()) {
                        FileUtils.copyFileToDirectory(f, backDirFile);
                        rollbackActions.add(roolAction);
                    } else if (f.isDirectory()) {
                        FileUtils.copyDirectoryToDirectory(f, backDirFile);
                        rollbackActions.add(roolAction);
                    } else {
                        if (log.isInfoEnabled()) {
                            log.info("备份失败,文件不存在[" + ac.getValue() + "]");
                        }
                        return false;
                    }
                    rollbackActions.add(roolAction);
                    continue;
                }
                if (ac.getName().equals(Global.ACTION_ADD)) {// add->del
                    Action roolAction = new Action();
                    roolAction.setName(Global.ACTION_DEL);
                    roolAction.setValue(ac.getValue());
                    rollbackActions.add(roolAction);
                    continue;
                }
            }
        }
        if (log.isInfoEnabled()) {
            log.info("升级前备份成功. ");
        }
        return true;
    }

    /**
     * 
     * @Title: upProcess
     * @Description: 升级
     * @param
     * @param conf
     * @param
     * @return
     * @return boolean 返回类型
     * @throws Exception
     */
    private boolean upProcess(ConfigFileModel conf) throws Exception {
        if (conf == null) {
            if (log.isInfoEnabled()) {
                log.info("升级过程中,升级配置文件加载失败.");
            }
            return false;
        }
        List<Action> actions = conf.getActions();
        if (actions != null && actions.size() > 0) {
            try {
                for (Action action : actions) {
                    if (action.getName().equals(Global.ACTION_ADD)) {
                        String temp = action.getValue();
                        if (temp.endsWith("/")) {
                            temp = temp.substring(0, temp.length() - 1);
                        }
                        String name = FilenameUtils.getName(temp);
                        String fullPath = FilenameUtils.getFullPath(temp);
                        File srcFile = new File(Global.UNPACKING_DIRECTORY + conf.getSoftVersion() + "/" + name);
                        if (srcFile.isFile()) {
                            FileUtils.copyFileToDirectory(srcFile, new File(fullPath));
                            if (log.isInfoEnabled()) {
                                log.info("成功执行" + action.getName() + "->" + action.getValue());
                            }
                            continue;
                        }
                        if (srcFile.isDirectory()) {
                            FileUtils.copyDirectoryToDirectory(srcFile, new File(fullPath));
                            if (log.isInfoEnabled()) {
                                log.info("成功执行" + action.getName() + "->" + action.getValue());
                            }
                            continue;
                        }
                    }
                    if (action.getName().equals(Global.ACTION_DEL)) {
                        File f = new File(action.getValue());
                        FileUtils.forceDelete(f);
                        if (log.isInfoEnabled()) {
                            log.info("成功执行" + action.getName() + "->" + action.getValue());
                        }
                    }
                    if (action.getName().equals(Global.ACTION_REPLACE)) {
                        String temp = action.getValue();
                        if (temp.endsWith("/")) {
                            temp = temp.substring(0, temp.length() - 1);
                        }
                        String name = FilenameUtils.getName(temp);
                        String fullPath = FilenameUtils.getFullPath(temp);
                        File file = new File(action.getValue());
                        if (file.isFile()) {
                            FileUtils.forceDelete(file);
                            FileUtils.copyFileToDirectory(new File(Global.UNPACKING_DIRECTORY + conf.getSoftVersion() + "/" + name), new File(fullPath));
                            if (log.isInfoEnabled()) {
                                log.info("成功执行" + action.getName() + "->" + action.getValue());
                            }
                            continue;
                        }
                        if (file.isDirectory()) {
                            FileUtils.forceDelete(file);
                            FileUtils.copyDirectoryToDirectory(new File(Global.UNPACKING_DIRECTORY + conf.getSoftVersion() + "/" + name), new File(fullPath));
                            if (log.isInfoEnabled()) {
                                log.info("成功执行" + action.getName() + "->" + action.getValue());
                            }
                            continue;
                        }
                    }
                    if (action.getName().equals(Global.ACTION_EXECUTE)) {
                        Runtime rt = Runtime.getRuntime();
                        Process p = rt.exec(action.getValue());
                        p.waitFor();
                        if (p.exitValue() != 0) {
                            if (log.isInfoEnabled()) {
                                log.info("升级命令执行失败. [" + action.getValue() + "]");
                            }
                            return false;
                        }
                        if (log.isInfoEnabled()) {
                            log.info("成功执行" + action.getName() + "->" + action.getValue());
                        }
                    }
                    if (action.getName().equals(Global.ACTION_SOPT)) {
                        Runtime rt = Runtime.getRuntime();
                        Process p = rt.exec(action.getValue());
                        p.waitFor();
                        if (p.exitValue() != 0) {
                            if (log.isInfoEnabled()) {
                                log.info("升级命令执行失败. [" + action.getValue() + "]");
                            }
                            return false;
                        }
                        if (log.isInfoEnabled()) {
                            log.info("成功执行" + action.getName() + "->" + action.getValue());
                        }
                    }
                    if (action.getName().equals(Global.ACTION_START)) {
                    	 if (log.isInfoEnabled()) {
                             log.info("升级命令开始执行. [" + action.getValue() + "]");
                         }
                        Runtime rt = Runtime.getRuntime();
                        Process p = rt.exec(action.getValue());
                        InputStreamReader ir=new
                        InputStreamReader(p.getInputStream());
                                   
                        BufferedReader input = new BufferedReader (ir);
                                   
                        String line;
                                   
                        while ((line = input.readLine ()) != null){
                                       
                        System.out.println(line);
                        if (log.isInfoEnabled()) {
                            log.info(line);
                        }
                                
                        }
                        p.waitFor();
                        if (p.exitValue() != 0) {
                            if (log.isInfoEnabled()) {
                                log.info("升级命令执行失败. [" + action.getValue() + "]");
                            }
                            return false;
                        }
                        if (log.isInfoEnabled()) {
                            log.info("成功执行" + action.getName() + "->" + action.getValue());
                        }
                    }
                }
            } catch (RuntimeException e1) {
                throw e1;
            }
            try {
                if (StringUtils.isNotEmpty(conf.getReturnAction())) {
                    Runtime rt = Runtime.getRuntime();
                    rt.exec(conf.getReturnAction());
                }
            } catch (RuntimeException e) {
                throw e;
            }
        }
        return true;
    }

    /**
     * 
     * @Title: roolBack
     * @Description: 回滚
     * @param
     * @param actions
     * @param conf
     * @return
     * @return void 返回类型
     * @throws Exception
     */
    private void roolBack(List<Action> actions, ConfigFileModel conf) throws Exception {
        if (log.isInfoEnabled()) {
            log.info("开始启动回滚.");
        }
        if (actions != null && actions.size() > 0 && conf != null) {
            for (Action ac : actions) {
                if (ac.getName().equals(Global.ACTION_ADD)) {
                    String temp = ac.getValue();
                    if (temp.endsWith("/")) {
                        temp = temp.substring(0, temp.length() - 1);
                    }
                    String name = FilenameUtils.getName(temp);
                    String fullPath = FilenameUtils.getFullPath(temp);
                    File srcFile = new File(Global.BACK_DIRECTORY + conf.getSoftVersion() + "/" + name);
                    if (srcFile.isFile()) {
                        FileUtils.copyFileToDirectory(srcFile, new File(fullPath));
                        continue;
                    }
                    if (srcFile.isDirectory()) {
                        FileUtils.copyDirectoryToDirectory(srcFile, new File(fullPath));
                        continue;
                    }
                    continue;
                } else if (ac.getName().equals(Global.ACTION_DEL)) {
                    File file = new File(ac.getValue());
                    if (file.exists()) {
                        FileUtils.forceDelete(file);
                    }
                    continue;
                } else if (ac.getName().equals(Global.ACTION_REPLACE)) {
                    String temp = ac.getValue();
                    if (temp.endsWith("/")) {
                        temp = temp.substring(0, temp.length() - 1);
                    }
                    String name = FilenameUtils.getName(temp);
                    String fullPath = FilenameUtils.getFullPath(temp);
                    File file = new File(ac.getValue());
                    if (file.isFile()) {
                        FileUtils.forceDelete(file);
                        FileUtils.copyFileToDirectory(new File(Global.BACK_DIRECTORY + conf.getSoftVersion() + "/" + name), new File(fullPath));
                        continue;
                    }
                    if (file.isDirectory()) {
                        FileUtils.forceDelete(file);
                        FileUtils.copyDirectoryToDirectory(new File(Global.BACK_DIRECTORY + conf.getSoftVersion() + "/" + name), new File(fullPath));
                        continue;
                    }
                    continue;
                }
            }
        }
        if (log.isInfoEnabled()) {
            log.info("回滚成功. 从版本 " + conf.getSoftVersion() + " 回滚到 " + conf.getOldVersion());
        }
    }
}
