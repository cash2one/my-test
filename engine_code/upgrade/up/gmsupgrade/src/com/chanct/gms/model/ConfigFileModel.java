package com.chanct.gms.model;

import java.util.List;

/**
 * 
 * @ClassName: ConfigFileModel
 * @Description: 升级配置对象类
 * @author shf
 * @date Dec 5, 2013 6:28:08 PM
 * 
 */
public class ConfigFileModel {
	/** 旧版本主目录 */
	private String mainDir;

	/** 旧版本 */
	private String oldVersion;

	/** 版本 */
	private String softVersion;

	/** 升级包类型 */
	private String packageType;

	/** 升级方式 */
	private String upMode;

	/** 磁盘最小空间 */
	private Integer minSpace;
	/** 操作集 */
	private List<Action> actions;

	/** 完成升级执行操作 */
	private String returnAction;

	/** 描述 */
	private String description;

	/**
	 * 
	 * @Title: getOldVersion
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getOldVersion() {
		return oldVersion;
	}

	/**
	 * 
	 * @Title: setOldVersion
	 * @Description:
	 * @param
	 * @param oldVersion
	 * @return void 返回类型
	 * @throws
	 */
	public void setOldVersion(String oldVersion) {
		this.oldVersion = oldVersion;
	}

	/**
	 * 
	 * @Title: getSoftVersion
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getSoftVersion() {
		return softVersion;
	}

	/**
	 * 
	 * @Title: setSoftVersion
	 * @Description:
	 * @param
	 * @param softVersion
	 * @return void 返回类型
	 * @throws
	 */
	public void setSoftVersion(String softVersion) {
		this.softVersion = softVersion;
	}

	/**
	 * 
	 * @Title: getPackageType
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getPackageType() {
		return packageType;
	}

	/**
	 * 
	 * @Title: setPackageType
	 * @Description:
	 * @param
	 * @param packageType
	 * @return void 返回类型
	 * @throws
	 */
	public void setPackageType(String packageType) {
		this.packageType = packageType;
	}

	/**
	 * 
	 * @Title: getUpMode
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getUpMode() {
		return upMode;
	}

	/**
	 * 
	 * @Title: setUpMode
	 * @Description:
	 * @param
	 * @param upMode
	 * @return void 返回类型
	 * @throws
	 */
	public void setUpMode(String upMode) {
		this.upMode = upMode;
	}

	/**
	 * 
	 * @Title: getMinSpace
	 * @Description:
	 * @param
	 * @return
	 * @return Integer 返回类型
	 * @throws
	 */
	public Integer getMinSpace() {
		return minSpace;
	}

	/**
	 * 
	 * @Title: setMinSpace
	 * @Description:
	 * @param
	 * @param minSpace
	 * @return void 返回类型
	 * @throws
	 */
	public void setMinSpace(Integer minSpace) {
		this.minSpace = minSpace;
	}

	/**
	 * 
	 * @Title: getActions
	 * @Description:
	 * @param
	 * @return
	 * @return List<Action> 返回类型
	 * @throws
	 */
	public List<Action> getActions() {
		return actions;
	}

	/**
	 * 
	 * @Title: setActions
	 * @Description:
	 * @param
	 * @param actions
	 * @return void 返回类型
	 * @throws
	 */
	public void setActions(List<Action> actions) {
		this.actions = actions;
	}

	/**
	 * 
	 * @Title: getDescription
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getDescription() {
		return description;
	}

	/**
	 * 
	 * @Title: setDescription
	 * @Description:
	 * @param
	 * @param description
	 * @return void 返回类型
	 * @throws
	 */
	public void setDescription(String description) {
		this.description = description;
	}

	/**
	 * 
	 * @Title: getReturnAction
	 * @Description:
	 * @param
	 * @return
	 * @return String 返回类型
	 * @throws
	 */
	public String getReturnAction() {
		return returnAction;
	}

	/**
	 * 
	 * @Title: setReturnAction
	 * @Description:
	 * @param
	 * @param returnAction
	 * @return void 返回类型
	 * @throws
	 */
	public void setReturnAction(String returnAction) {
		this.returnAction = returnAction;
	}

	public String getMainDir() {
		return mainDir;
	}

	public void setMainDir(String mainDir) {
		this.mainDir = mainDir;
	}

}
