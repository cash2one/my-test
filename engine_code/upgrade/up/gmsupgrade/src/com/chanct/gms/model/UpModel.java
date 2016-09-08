package com.chanct.gms.model;

/**
 * 
 * @ClassName: UpModel
 * @Description: 升级对象
 * @author shf
 * @date Dec 11, 2013 11:05:38 AM
 * 
 */
public class UpModel {
    public String oldVersion;
    public String newVersion;
    public String upMode;

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

    public String getUpMode() {
        return upMode;
    }

    public void setUpMode(String upMode) {
        this.upMode = upMode;
    }
}
