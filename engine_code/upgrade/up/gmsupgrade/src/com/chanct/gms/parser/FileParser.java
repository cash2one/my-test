package com.chanct.gms.parser;

import java.io.File;
import java.io.FileFilter;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dom4j.Document;
import org.dom4j.Element;

import com.chanct.gms.config.Global;
import com.chanct.gms.utile.DBUtiles;
import com.chanct.gms.utile.Utiles;
import com.chanct.gms.utile.XMLUtiles;

/**
 * 
 * @ClassName: FileParser
 * @Description: 文件解析器
 * @author shf
 * @date Dec 5, 2013 2:11:14 PM
 * 
 */
public class FileParser {
	private static final Log log = LogFactory.getLog(DBUtiles.class);
    /**
     * 
     * <p>
     * Title:FileParser
     * </p>
     * <p>
     * Description:Construct Method
     * </p>
     */
    public FileParser() {

    }

    /**
     * 
     * @Title: loadConfFile
     * @Description: 加载并解析升级程序的配置文件
     * @param
     * @param file
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean loadConfFile(String file) throws Exception {
        Document doc = XMLUtiles.readXML(file);
        Element root = XMLUtiles.getRootElement(doc);
        Element dataBase = root.element(Global.DATABASE);
        Global.DB_DRIVER_CLASS = dataBase.elementText(Global.DRIVERCLASS);
        Global.DB_CONN_URL = dataBase.elementText(Global.URL);
        Global.DB_USER = dataBase.elementText(Global.USER);
        Global.DB_PASSWORD = dataBase.elementText(Global.PASSWORD);

        Element path = root.element(Global.PATH);
        Global.PACK_DIRECTORY = path.elementText(Global.PACKDIRECTORY);
        Global.PACK_DIRECTORY = Utiles.dealDirSeparate(Global.PACK_DIRECTORY);
        Global.PACK_DIRECTORY = Utiles.dealDirectoryTail(Global.PACK_DIRECTORY);

        Global.UNPACKING_DIRECTORY = path.elementText(Global.UNPACKDIRECTORY);
        Global.UNPACKING_DIRECTORY = Utiles.dealDirSeparate(Global.UNPACKING_DIRECTORY);
        Global.UNPACKING_DIRECTORY = Utiles.dealDirectoryTail(Global.UNPACKING_DIRECTORY);

        Global.BACK_DIRECTORY = path.elementText(Global.BACKDIRECTORY);
        Global.BACK_DIRECTORY = Utiles.dealDirSeparate(Global.BACK_DIRECTORY);
        Global.BACK_DIRECTORY = Utiles.dealDirectoryTail(Global.BACK_DIRECTORY);

        Global.SYS_VERSION_FILE = path.elementText(Global.SYSVERSIONFILE);
        Global.SYS_VERSION_FILE = Utiles.dealDirSeparate(Global.SYS_VERSION_FILE);

        Utiles.exitsOrMkdirs(Global.PACK_DIRECTORY);
        Utiles.exitsOrMkdirs(Global.UNPACKING_DIRECTORY);
        Utiles.exitsOrMkdirs(Global.BACK_DIRECTORY);

        if (!Utiles.isFile(Global.SYS_VERSION_FILE)) {
            throw new Exception("没有找到设备的版本配置信息. ");
        }

        Element comm = root.element(Global.COMM);
        Global.COMM_PORT = Integer.parseInt(comm.elementText(Global.PORT));
        return true;
    }

    /**
     * 
     * @Title: inits
     * @Description: 初始化加载相关目录(包括：解包目录、备份目录以及当前版本号)
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean inits() throws Exception {
        initSystemVersion();
        initUnPackDirectory();
        // initBakDerectory();
        initUpPackDerectory();
        return true;
    }

    /**
     * 
     * @Title: initSystemVersion
     * @Description: 从系统配置文件获取系统当前版本
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void initSystemVersion() throws Exception {
    	Document doc = XMLUtiles.readXML(Global.SYS_VERSION_FILE);
    	try{
        Global.VERSION = XMLUtiles.getElementValue(doc.getRootElement(), Global.SYS_VERSION);
    	}catch (Exception e) {
    		e.printStackTrace();	
		}
        if (log.isInfoEnabled()) {
            log.info(Global.VERSION+"--------------version--------------ok ");
        }
    }

    /**
     * 
     * @Title: initUnPackDirectory
     * @Description: 初始化解包目录
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void initUnPackDirectory() throws Exception {
        File upd = new File(Global.UNPACKING_DIRECTORY);
        File[] dirs = upd.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                if (pathname.isDirectory()) {
                    String dirName = pathname.getName();
                    if (!(dirName.split("\\.").length == 4)) {
                        return false;
                    }
                    return true;
                }
                return false;
            }
        });
        // 目录入Map
        for (File file : dirs) {
            Global.unPackMap.put(file.getName(), file.getAbsolutePath());
        }
    }

    /**
     * 
     * 
     * @Title: initBakDerectory
     * @Description: 初始化备份目录
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    /**
     * private void initBakDerectory() throws Exception { File backDir = new
     * File(Global.BACK_DIRECTORY); File[] dirs = backDir.listFiles(new
     * FileFilter() {
     * 
     * @Override public boolean accept(File pathname) { if
     *           (pathname.isDirectory()) { String dirName = pathname.getName();
     *           if (!dirName.substring(0, 4).equals("GMS-")) { return false; }
     *           return true; } return false; } }); // 目录入Map for (File file :
     *           dirs) { Global.backMap.put(file.getName().split("-")[1], file
     *           .getAbsolutePath()); } }
     */

    /**
     * 
     * @Title: initUpPackDerectory
     * @Description: 初始化升级包目录
     * @param
     * @throws Exception
     * @return void 返回类型
     * @throws
     */
    private void initUpPackDerectory() throws Exception {
        File upPackDir = new File(Global.PACK_DIRECTORY);
        File[] dirs = upPackDir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                if (pathname.isFile()) {
                    String fileName = pathname.getName();
                    if (!fileName.substring(0, 4).equals("GMS-")) {
                        return false;
                    }
                    return true;
                }
                return false;
            }
        });
        // 目录入Map
        for (File file : dirs) {
            Global.packMap.put(file.getName().split("-")[1], file.getAbsolutePath());
        }
        
    }

}