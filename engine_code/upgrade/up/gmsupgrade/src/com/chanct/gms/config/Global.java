package com.chanct.gms.config;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.chanct.gms.model.UpModel;

/**
 * 
 * @ClassName: Global
 * @Description: 全局配置类
 * @author shf
 * @date Dec 4, 2013 11:00:36 AM
 * 
 */
public class Global {

    /** **************升级程序配置路径*********************** */
    /** 升级程序配置文件路径-相对路径 */
    public static final String CONFIG = "./conf/conf.xml";
    /** 程序主路径 */
    public static String HOMEDIR = "";

    /** **************升级程序配置文件XML元素定义*********************** */
    /** 数据库标签 */
    public static final String DATABASE = "database";
    /** 数据库标签驱动类标签 */
    public static final String DRIVERCLASS = "driverclass";
    /** 数据库链接地址标签 */
    public static final String URL = "url";
    /** 数据库用户标签 */
    public static final String USER = "user";
    /** 数据库标签 */
    public static final String PASSWORD = "password";
    /** 升级相关目录标签 */
    public static final String PATH = "path";
    /** 升级包目录标签 */
    public static final String PACKDIRECTORY = "packdirectory";
    /** 解包目录标签 */
    public static final String UNPACKDIRECTORY = "unpackdirectory";
    /** 备份目录标签 */
    public static final String BACKDIRECTORY = "backdirectory";
    /** 系统版本配置文件标签 */
    public static final String SYSVERSIONFILE = "sysversionfile";
    /** comm标签 */
    public static final String COMM = "comm";
    /** 端口标签 */
    public static final String PORT = "port";

    /** **************数据库配置*********************** */
    /** 数据库驱动类 */
    public static String DB_DRIVER_CLASS = "";
    /** 数据库连接URL */
    public static String DB_CONN_URL = "";
    /** 数据库用户 */
    public static String DB_USER = "";
    /** 数据库密码 */
    public static String DB_PASSWORD = "";

    /** **************目录配置*********************** */
    /** 升级包存放目录 */
    public static String PACK_DIRECTORY = "";
    /** 解包临时目录 */
    public static String UNPACKING_DIRECTORY = "";
    /** 备份文件临时目录 */
    public static String BACK_DIRECTORY = "";
    /** 系统版本配置文件 */
    public static String SYS_VERSION_FILE = "";

    /** **************加密定义*********************** */
    /** DES加密密钥 */
    public final static String DES_PASSWORD_KEY = "chanct-gms";
    /** 魔法棒原始值 */
    public final static String MAGIC_STRING = "chanct-gms-upgrade";
    /** 加密算命名称 */
    public final static String ALGORITHM = "DES";

    /** **************升级配置文件定义*********************** */
    /** 升级包类型：engine */
    public final static String PACKAGE_TYPE_ENGINE = "engine";
    /** 升级包类型：page */
    public final static String PACKAGE_TYPE_PAGE = "page";
    /** 升级方式：normal */
    public final static String UPMode_NORMAOL = "normal";
    /** 升级方式：emergency */
    public final static String UPMode_EMERGENCY = "emergency";
    /** 升级方动作定义:stop */
    public final static String ACTION_SOPT = "stop";
    /** 升级方动作定义:start */
    public final static String ACTION_START = "start";
    /** 升级方动作定义:add */
    public final static String ACTION_ADD = "add";
    /** 升级方动作定义:del */
    public final static String ACTION_DEL = "del";
    /** 升级方动作定义:replace */
    public final static String ACTION_REPLACE = "replace";
    /** 升级方动作定义:execute */
    public final static String ACTION_EXECUTE = "execute";

    /** **************文件分隔符*********************** */
    /** 升级包文件名前缀 */
    public final static String UP_FILE_PRE = "GMS-";
    /** 文件分割符-UNIX */
    public final static String FILE_SEPARATE_UNIX = "/";
    /** 文件分割符-WIN */
    public final static String FILE_SEPARATE_WIN = "\\";
    /** 文件分割符-Replace */
    public final static String FILE_SEPARATE_REPLACE = "\\\\";

    /** **************升级类型定义*********************** */
    /** 升级方式:自动 */
    public final static Integer UP_TYPE_AUTO = 1;
    /** 升级方式:手工 */
    public final static Integer UP_TYPE_HAND = 2;

    /** 升级状态:0"未处理" */
    public final static Integer UP_STATUS_NO = 0;
    /** 升级状态:1"以后升级" */
    public final static Integer UP_STATUS_LATER = 1;
    /** 升级状态:2"立即升级" */
    public final static Integer UP_STATUS_IMMEDIATE = 2;
    /** 升级状态:3"升级成功" */
    public final static Integer UP_STATUS_OK = 3;
    /** 升级状态:4"升级失败" */
    public final static Integer UP_STATUS_FAILT = 4;

    /** **************升级内容定义*********************** */
    /** 升级内容:engine */
    public final static Integer UP_CONT_ENGINE = 1;
    /** 升级内容:page */
    public final static Integer UP_CONT_PAGE = 2;

    /** **************系统版本号*********************** */
    /** 系统版本 */
    public static String VERSION = "";
    /** 系統版本标签 */
    public static final String SYS_VERSION = "SysVersion";

    /** **************升级全局存储对象*********************** */
    /** 解包Map<version,absolutePath> */
    public static ConcurrentHashMap<String, String> unPackMap = new ConcurrentHashMap<String, String>();
    /** 升级包Map<version,absolutePath> */
    public static ConcurrentHashMap<String, String> packMap = new ConcurrentHashMap<String, String>();
    /** 备份Map<version,absolutePath> */
    public static ConcurrentHashMap<String, String> backMap = new ConcurrentHashMap<String, String>();
    /** 升级队列(包括普通升级和紧急升级)<version> */
    public static ConcurrentLinkedQueue<UpModel> upQueue = new ConcurrentLinkedQueue<UpModel>();

    /** **************其他*********************** */
    /** 字符格式 */
    public final static String CHARSETNAME = "UTF-8";

    /** 升级配置文件名 */
    public final static String UPGRADE_FILE_NAME = "gmsupgrade.xml";

    /** 通信端口 */
    public static Integer COMM_PORT = -1;
}
