package com.chanct.gms.db;

import java.util.List;

import com.chanct.gms.model.VersionModel;

/**
 * 
 * @ClassName: IDbServici
 * @Description: 数据库操作接口
 * @author shf
 * @date Dec 11, 2013 8:42:02 AM
 * 
 */
public interface IDbService {

    /**
     * 
     * @Title: insertAlertMsg
     * @Description: 添加升级提醒消息
     * @param
     * @param ver
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean addAlertMsg(VersionModel ver) throws Exception;

    /**
     * 
     * @Title: updateUpStatus
     * @Description: 更升级状态新
     * @param
     * @param oldVer
     * @param
     * @param newVer
     * @param
     * @param upStatus
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean updateUpStatus(VersionModel ver) throws Exception;

    /**
     * 
     * @Title: getUpList
     * @Description: 获取升级指令列表，按照升级版本号进行排序
     * @param
     * @return
     * @param
     * @throws Exception
     * @return List<VersionModel> 返回类型
     * @throws
     */
    public List<VersionModel> getUpList() throws Exception;

    /**
     * 
     * @Title: updateSysVersion
     * @Description: 更新系统本版本
     * @param
     * @param version
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean updateSysVersion(String version) throws Exception;

    /**
     * 
     * @Title: synSysLibVersion
     * @Description: 同步系统版本号和特征库版本号
     * @param
     * @param sysVersion
     * @param
     * @param libVersion
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean synSysLibVersion(String sysVersion, String libVersion) throws Exception;
}
