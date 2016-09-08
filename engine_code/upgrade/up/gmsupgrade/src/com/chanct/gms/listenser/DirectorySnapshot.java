package com.chanct.gms.listenser;

import java.util.Map;
import java.util.HashMap;

/**
 * 
 * @ClassName: DirectorySnapshot
 * @Description: 目录快照
 * @author shf
 * @date Dec 9, 2013 5:20:31 PM
 * 
 */
public class DirectorySnapshot {

	@SuppressWarnings("unchecked")
	private static Map files = new HashMap();

	@SuppressWarnings("unchecked")
	public static void addFile(String fileName) {
		files.put(fileName, fileName);
	}

	public static void removeFile(String fileName) {
		files.remove(fileName);
	}

	public static boolean containsFile(String fileName) {
		return files.containsKey(fileName);
	}
}
