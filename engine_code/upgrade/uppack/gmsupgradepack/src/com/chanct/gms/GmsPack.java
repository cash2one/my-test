package com.chanct.gms;

import java.io.File;
import java.io.IOException;
import java.util.List;

import org.apache.commons.io.FileUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dom4j.Element;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.Action;
import com.chanct.gms.model.ConfigFileModel;
import com.chanct.gms.parser.ArgsParser;
import com.chanct.gms.parser.PackParser;
import com.chanct.gms.parser.FileParser;
import com.chanct.gms.utiles.DesUtiles;
import com.chanct.gms.utiles.MD5Utiles;
import com.chanct.gms.utiles.XMLUtiles;

/**
 * 
 * @ClassName: GmsPack
 * @Description: 打包程序主类
 * @author shf
 * @date Dec 4, 2013 9:31:50 AM
 * 
 */
public class GmsPack {

	/**
	 * log
	 */
	private static final Log log = LogFactory.getLog(GmsPack.class);

	/** 文件解析器 */
	private static FileParser fileParser = new FileParser();

	/** 参数解析器 */
	private static ArgsParser argsParser = new ArgsParser();

	/** 打包解析器 */
	private static PackParser packParser = new PackParser();

	/**
	 * 
	 * @Title: main
	 * @Description: 程序入口
	 * @param
	 * @param args
	 * @return void 返回类型
	 * @throws
	 */
	public static void main(String[] args) {
		if (!argsParser.getOpts(args)) {
			System.exit(1);
		}

		ConfigFileModel configFile = null;
		try {
			Element root = XMLUtiles.getRootElement(XMLUtiles
					.readXML(Global.CONFIG_FILE));
			configFile = fileParser.getConfigFileModel(root);
			if (log.isInfoEnabled()) {
				log.info("成功解析升级配置文件: " + Global.CONFIG_FILE);
			}
		} catch (Exception e) {
			if (log.isInfoEnabled()) {
				log.info("解析升级配置文件失败. Reson: " + e.getMessage());
				System.exit(1);
			}
		}

		int packFileCount = 0;
		byte[] packHeader = null;
		byte[] packBody = null;
		byte[] packAll = null;
		String md5Str = "";
		String upPackFileName = "";
		try {
			List<Action> opFiles = fileParser.getFileCount(configFile);
			if (opFiles != null && opFiles.size() > 0) {
				packFileCount = opFiles.size();
			}
			packHeader = packParser.packHeader(packFileCount);

			if (log.isInfoEnabled()) {
				log.info("升级包头封装成功. ");
			}
			if (packFileCount > 0) {
				packBody = packParser.packBody(opFiles);
				if (log.isInfoEnabled()) {
					log.info("升级包体封装成功. ");
				}
				packAll = packParser.bytesAddBytesPack(packHeader, packBody);
				if (log.isInfoEnabled()) {
					log.info("升级包封装完成. ");
				}
			} else {
				packAll = packHeader;
				if (log.isInfoEnabled()) {
					log.info("升级包封装完成，其中包体为空. ");
				}
			}

			// 将整个包再加一次密
			packAll = DesUtiles.encrypt(packAll, Global.DES_PASSWORD_KEY
					.getBytes());

			// 获取MD5值
			md5Str = MD5Utiles.MD5Encode(packAll);
			upPackFileName = Global.UP_FILE_PRE + configFile.getSoftVersion()
					+ "-" + md5Str + "-" + configFile.getUpMode();
		} catch (Exception e) {
			if (log.isInfoEnabled()) {
				log.info("升级包封装失败. Reson: " + e.getMessage());
			}
			e.printStackTrace();
			System.exit(1);
		}

		try {
			FileUtils.writeByteArrayToFile(new File(Global.TARGET_FILE + "/"
					+ upPackFileName), packAll);
			if (log.isInfoEnabled()) {
				log.info("升级包制作完成. " + Global.TARGET_FILE + upPackFileName);
			}
		} catch (IOException e) {
			if (log.isInfoEnabled()) {
				log.info("升级包制作失败. Reson: " + e.getMessage());
			}
			System.exit(1);
		}
	}
}
