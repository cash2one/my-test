package com.chanct.gms.model;

/**
 * 
 * @ClassName: Action
 * @Description: 升级操作类
 * @author shf
 * @date Dec 5, 2013 6:25:20 PM
 * 
 */
public class Action {

	/** 操作名 */
	private String name;

	/** 操作内容 */
	private String value;

	/**
	 * 
	 * @Title: getName
	 * @Description: 获取操作名
	 * @return String 返回类型
	 * @throws
	 */
	public String getName() {
		return name;
	}

	/**
	 * 
	 * @Title: setName
	 * @Description: 设置操作名
	 * @param name
	 *            操作
	 * @return void 返回类型
	 * @throws
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * 
	 * @Title: getValue
	 * @Description: 获得操作内容
	 * @return String 返回类型
	 * @throws
	 */
	public String getValue() {
		return value;
	}

	/**
	 * 
	 * @Title: setValue
	 * @Description: 设置操作内容
	 * @param value
	 *            操作内容
	 * @return void 返回类型
	 * @throws
	 */
	public void setValue(String value) {
		this.value = value;
	}

}
