package com.chanct.gms.model;

import java.text.SimpleDateFormat;
import java.util.Date;

import com.chanct.gms.config.Global;

/**
 * 
 * @ClassName: VersionModel
 * @Description: 升级版本对象
 * @author shf
 * @date Dec 11, 2013 11:05:38 AM
 * 
 */
public class VersionModel {
	public Date upTime;
	public String upTimeStr;
	public Integer upType;
	public Integer upContType;
	public String upContTypeStr;
	public String oldVersion;
	public String newVersion;
	public String newVerInfo;
	public Integer upStatus;

	public Integer getUpType() {
		return upType;
	}

	public void setUpType(Integer upType) {
		this.upType = upType;
	}

	public Integer getUpContType() {
		return upContType;
	}

	public void setUpContType(Integer upContType) {
		this.upContType = upContType;
	}

	public String getOldVersion() {
		return oldVersion;
	}

	public void setOldVersion(String oldVersion) {
		this.oldVersion = oldVersion;
	}

	public String getNewVersion() {
		return newVersion;
	}

	public void setNewVersion(String newVersion) {
		this.newVersion = newVersion;
	}

	public String getNewVerInfo() {
		return newVerInfo;
	}

	public void setNewVerInfo(String newVerInfo) {
		this.newVerInfo = newVerInfo;
	}

	public Integer getUpStatus() {
		return upStatus;
	}

	public void setUpStatus(Integer upStatus) {
		this.upStatus = upStatus;
	}

	public String getUpTimeStr() {
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		return sdf.format(this.upTime);
	}

	public void setUpTimeStr(String upTimeStr) {
		this.upTimeStr = upTimeStr;
	}

	public Date getUpTime() {
		return upTime;
	}

	public void setUpTime(Date upTime) {
		this.upTime = upTime;
	}

	public String getUpContTypeStr() {
		return upContTypeStr;
	}

	public void setUpContTypeStr(String upContTypeStr) {
		if (upContTypeStr.equals(Global.PACKAGE_TYPE_ENGINE)) {
			this.upContType = Global.UP_CONT_ENGINE;
		} else {
			this.upContType = Global.UP_CONT_PAGE;
		}
		this.upContTypeStr = upContTypeStr;
	}
}
