package com.chanct.gms.db;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.postgresql.util.PSQLException;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.ComparatorVer;
import com.chanct.gms.model.VersionModel;
import com.chanct.gms.utile.DBUtiles;

/**
 * 
 * @ClassName: DbServiceImpl
 * @Description: 数据库操作实现
 * @author shf
 * @date Dec 11, 2013 11:19:04 AM
 * 
 */
public class DbServiceImpl implements IDbService {

    @SuppressWarnings("unchecked")
    @Override
    public List<VersionModel> getUpList() throws Exception {
        List<VersionModel> result = new ArrayList<VersionModel>();
        String sql = "select utype,ucontent_type,old_version,new_version,ustatus from t_log_update where ustatus=?";
        PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
        ps.setInt(1, Global.UP_STATUS_IMMEDIATE);
        ResultSet rs = ps.executeQuery();
        while (rs.next()) {
            VersionModel ver = new VersionModel();
            ver.setUpType(rs.getInt("utype"));
            ver.setUpContType(rs.getInt("ucontent_type"));
            ver.setOldVersion(rs.getString("old_version"));
            ver.setNewVersion(rs.getString("new_version"));
            ver.setUpStatus(rs.getInt("ustatus"));
            result.add(ver);
        }
        Collections.sort(result, new ComparatorVer());
        ps.close();
        rs.close();
        ps = null;
        rs = null;
        return result;
    }

    @Override
    public boolean addAlertMsg(VersionModel ver) throws Exception {
        try {
            String sql = "insert into t_log_update(utime,utype,ucontent_type,old_version,new_version,new_verinfo,ustatus) " + "values(?,?,?,?,?,?,?)";
            PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
            ps.setTimestamp(1, new Timestamp(ver.getUpTime().getTime()));
            ps.setInt(2, ver.getUpType());
            ps.setInt(3, ver.getUpContType());
            ps.setString(4, ver.getOldVersion());
            ps.setString(5, ver.getNewVersion());
            ps.setString(6, ver.getNewVerInfo());
            ps.setInt(7, ver.getUpStatus());
            ps.executeUpdate();
        } catch (PSQLException e) {
            // 捕获数据重复异常，不做处理
            if (!(e.getMessage().indexOf("ERROR: duplicate key") > -1)) {
                throw e;
            }
        }
        return true;
    }

    @Override
    public boolean updateUpStatus(VersionModel ver) throws Exception {
        String sql = "";
        // if (ver.getUpStatus() == Global.UP_STATUS_OK) {
        sql = "update t_log_update set ustatus=? where old_version=? and new_version=?";
        PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
        ps.setInt(1, ver.getUpStatus());
        ps.setString(2, ver.getOldVersion());
        ps.setString(3, ver.getNewVersion());
        ps.executeUpdate();
        ps.close();
        ps = null;
        return true;
        // } else if (ver.getUpStatus() == Global.UP_STATUS_FAILT) {
        // sql = "update t_log_update set ustatus=? where old_version=? and
        // new_version=?";
        // PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
        // ps.setInt(1, ver.getUpStatus());
        // ps.setString(2, ver.getOldVersion());
        // ps.setString(3, ver.getNewVersion());
        // ps.executeUpdate();
        // ps.close();
        // ps = null;
        // return true;
        // } else {
        // }
        // return false;
    }

    @Override
    public boolean updateSysVersion(String version) throws Exception {
        String sql = "select 1 from t_conf_curver";
        PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
        ResultSet rs = ps.executeQuery();
        if (rs.next()) {
            sql = "update t_conf_curver set cur_system_ver=?";
            ps = DBUtiles.getInstance().prepareStatement(sql);
            ps.setString(1, version);
            ps.executeUpdate();
        } else {
            sql = "insert into t_conf_curver(cur_system_ver) values(?,?)";
            ps = DBUtiles.getInstance().prepareStatement(sql);
            ps.setString(1, version);
            ps.executeUpdate();
        }
        ps.close();
        rs.close();
        ps = null;
        rs = null;
        return true;
    }

    @Override
    public boolean synSysLibVersion(String libVersion, String sysVersion) throws Exception {
        String sql = "select 1 from t_conf_curver";
        PreparedStatement ps = DBUtiles.getInstance().prepareStatement(sql);
        ResultSet rs = ps.executeQuery();
        if (rs.next()) {
            sql = "update t_conf_curver set cur_feature_ver=?,cur_system_ver=?";
            ps = DBUtiles.getInstance().prepareStatement(sql);
            ps.setString(1, libVersion);
            ps.setString(2, sysVersion);
            ps.executeUpdate();
        } else {
            sql = "insert into t_conf_curver values(?,?)";
            ps = DBUtiles.getInstance().prepareStatement(sql);
            ps.setString(1, libVersion);
            ps.setString(2, sysVersion);
            ps.executeUpdate();
        }
        ps.close();
        rs.close();
        ps = null;
        rs = null;
        return true;
    }

}
