package com.chanct.gms.utile;

import java.sql.Connection;
import java.sql.DriverManager;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.chanct.gms.config.Global;

/**
 * 
 * @ClassName: DBUtiles
 * @Description: 数据库工具类
 * @author shf
 * @date Dec 9, 2013 10:46:10 AM
 * 
 */
public class DBUtiles {
	private static final Log log = LogFactory.getLog(DBUtiles.class);

	private static Connection conn = null;

	/**
	 * 
	 * @Title: getInstance
	 * @Description: 获取数据库连接
	 * @param
	 * @param
	 * @return
	 * @return Connection 返回类型
	 * @throws
	 */
	public static Connection getInstance() throws Exception {
		if (conn == null) {
			try {
				Class.forName(Global.DB_DRIVER_CLASS);
				conn = DriverManager.getConnection(Global.DB_CONN_URL,
						Global.DB_USER, Global.DB_PASSWORD);
				if (log.isInfoEnabled()) {
					log.info("成功连接数据库. ");
				}
			} catch (Exception e) {
				throw new Exception("数据库连接异常. 原因: " + e.getMessage());
			}
		}
		return conn;
	}
}
