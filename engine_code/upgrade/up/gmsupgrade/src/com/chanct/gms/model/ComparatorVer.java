package com.chanct.gms.model;

import java.util.Comparator;

/**
 * 
 * @ClassName: ComparatorVer
 * @Description: 比较器
 * @author shf
 * @date Dec 12, 2013 1:32:07 PM
 * 
 */
@SuppressWarnings("unchecked")
public class ComparatorVer implements Comparator {

	@Override
	public int compare(Object o1, Object o2) {
		VersionModel v1 = (VersionModel) o1;
		VersionModel v2 = (VersionModel) o2;
		String[] v1s = v1.getNewVersion().split("\\.");
		String[] v2s = v2.getNewVersion().split("\\.");

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
