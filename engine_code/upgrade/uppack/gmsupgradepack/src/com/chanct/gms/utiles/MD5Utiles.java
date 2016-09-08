package com.chanct.gms.utiles;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * 
 * @ClassName: MD5Utiles
 * @Description: MD5工具类
 * @author shf
 * @date Dec 6, 2013 8:52:15 AM
 * 
 */
public class MD5Utiles {

	/** 十六进制编码 */
	private final static String[] hexDigits = { "0", "1", "2", "3", "4", "5",
			"6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };

	/**
	 * 
	 * @Title: byteArrayToHexString
	 * @Description: byte[]转换为String
	 * @param b
	 * @return String 返回类型
	 * @throws
	 */
	private static String byteArrayToHexString(byte[] src) {
		StringBuffer resultSb = new StringBuffer();
		for (int i = 0; i < src.length; i++) {
			resultSb.append(byteToHexString(src[i]));
		}
		return resultSb.toString();
	}

	/**
	 * 
	 * @Title: byteToHexString
	 * @Description: byte转换为String
	 * @param b
	 * @return String 返回类型
	 * @throws
	 */
	private static String byteToHexString(byte b) {
		int n = b;
		if (n < 0)
			n = 256 + n;
		int d1 = n / 16;
		int d2 = n % 16;
		return hexDigits[d1] + hexDigits[d2];
	}

	/**
	 * 
	 * @Title: MD5Encode
	 * @Description: 获取MD5值
	 * @param origin
	 * @return String 返回类型
	 * @throws NoSuchAlgorithmException
	 * @throws
	 */
	public static String MD5Encode(String origin)
			throws NoSuchAlgorithmException {
		String resultString = null;
		resultString = new String(origin);
		MessageDigest md = MessageDigest.getInstance("MD5");
		resultString = byteArrayToHexString(md.digest(resultString.getBytes()));
		return resultString;
	}

	/**
	 * 
	 * @Title: MD5Encode
	 * @Description: 获取MD5值
	 * @param origin
	 * @throws NoSuchAlgorithmException
	 * @return String 返回类型
	 * @throws
	 */
	public static String MD5Encode(byte[] origin)
			throws NoSuchAlgorithmException {
		String resultString = null;
		MessageDigest md = MessageDigest.getInstance("MD5");
		resultString = byteArrayToHexString(md.digest(origin));
		return resultString;
	}
}
