package com.chanct.gms.config;

/**
 * 
 * @ClassName: Global
 * @Description: 全局配置类
 * @author shf
 * @date Dec 4, 2013 11:00:36 AM
 * 
 */
public class Global {
	/** 升级配置文件全路径 */
	public static String CONFIG_FILE = "";

	/** 升级包存放路径 */
	public static String TARGET_FILE = "";

	/** DES加密密钥 */
	public final static String DES_PASSWORD_KEY = "chanct-gms";

	/** 魔法棒原始值 */
	public final static String MAGIC_STRING = "chanct-gms-upgrade";

	/** 加密算命名称 */
	public final static String ALGORITHM = "DES";

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

	/** 升级包文件名前缀 */
	public final static String UP_FILE_PRE = "GMS-";

	/** 文件分割符-UNIX */
	public final static String FILE_SEPARATE_UNIX = "/";

	/** 文件分割符-WIN */
	public final static String FILE_SEPARATE_WIN = "\\";

	/** 文件分割符-Replace */
	public final static String FILE_SEPARATE_REPLACE = "\\\\";

	/** 升级配置文件名 */
	public final static String UPGRADE_FILE_NAME = "gmsupgrade.xml";

	/** 字符格式 */
	public final static String CHARSETNAME = "UTF-8";
}
