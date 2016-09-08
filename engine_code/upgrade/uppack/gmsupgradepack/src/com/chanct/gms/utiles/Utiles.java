package com.chanct.gms.utiles;

import java.io.File;

import org.apache.commons.lang.StringUtils;

import com.chanct.gms.config.Global;

/**
 * 
 * @ClassName: Utiles
 * @Description: 工具类
 * @author shf
 * @date Dec 4, 2013 10:55:12 AM
 * 
 */
public class Utiles {

	/**
	 * 
	 * @Title: isFile
	 * @Description: 验证是否标准文件
	 * @param filePath
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isFile(String filePath) {
		if (StringUtils.isEmpty(filePath)) {
			return false;
		}

		File file = new File(filePath);
		if (file.isFile()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: isFile
	 * @Description: 验证是否标准文件
	 * @param file
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isFile(File file) {
		if (file.isFile()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: isDirectory
	 * @Description: 判断是否标准路径
	 * @param directoryPath
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isDirectory(String directoryPath) {
		if (StringUtils.isEmpty(directoryPath)) {
			return false;
		}

		File file = new File(directoryPath);
		if (file.isDirectory()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: isDirectory
	 * @Description: 判断是否标准路径
	 * @param directory
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isDirectory(File directory) {
		if (directory.isDirectory()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: isExists
	 * @Description: 验证文件是否存在
	 * @param filePath
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isExists(String filePath) {
		File file = new File(filePath);
		if (file.exists()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: isExists
	 * @Description: 验证文件是否存在
	 * @param
	 * @param file
	 * @param
	 * @return
	 * @return boolean 返回类型
	 * @throws
	 */
	public static boolean isExists(File file) {
		if (file.exists()) {
			return true;
		}

		return false;
	}

	/**
	 * 
	 * @Title: dealDirSeparate
	 * @Description: 将win下分隔符统一替换为unix下分隔符
	 * @param
	 * @param directory
	 * @param
	 * @return
	 * @param
	 * @return String 返回类型
	 * @throws
	 */
	public static String dealDirSeparate(String directory) {
		if (directory.indexOf(Global.FILE_SEPARATE_WIN) > -1) {
			return directory.replaceAll(Global.FILE_SEPARATE_REPLACE,
					Global.FILE_SEPARATE_UNIX);
		}
		return directory;
	}

	/**
	 * 
	 * @Title: dealDirectoryTail
	 * @Description: 处理目录结尾,追加'/'
	 * @param
	 * @param tarDirectory
	 * @param
	 * @return
	 * @param
	 * @return String 返回类型
	 * @throws
	 */
	public static String dealDirectoryTail(String tarDirectory) {
		if (!tarDirectory.endsWith(Global.FILE_SEPARATE_UNIX)) {
			return tarDirectory + Global.FILE_SEPARATE_UNIX;
		}
		return tarDirectory;
	}

	/**
	 * 
	 * @Title: compareVersion
	 * @Description: 版本号比较
	 * @param
	 * @param v1
	 * @param
	 * @param v2
	 * @param
	 * @return
	 * @return int 返回类型 1:v1>v2 -1:v1<v2 0:v1=v2
	 * @throws
	 */
	public static int compareVersion(String v1, String v2) {
		String[] v1s = v1.split("\\.");
		String[] v2s = v2.split("\\.");

		int tag1 = v1s[0].compareTo(v2s[0]);
		if (tag1 == 0) {
			int tag2 = v1s[1].compareTo(v2s[1]);
			if (tag2 == 0) {
				int tag3 = v1s[2].compareTo(v2s[2]);
				if (tag3 == 0) {
					return v1s[3].compareTo(v2s[3]);
				} else {
					return tag3;
				}
			} else {
				return tag2;
			}
		} else {
			return tag1;
		}
	}
}
