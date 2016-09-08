package com.chanct.gms.utile;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.apache.commons.io.FileUtils;
import org.apache.commons.lang.StringUtils;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.UpModel;

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
            return directory.replaceAll(Global.FILE_SEPARATE_REPLACE, Global.FILE_SEPARATE_UNIX);
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
     * @Title: parserDate2String
     * @Description: 将时间转换为字符串
     * @param
     * @param date
     * @param
     * @return
     * @return String 返回类型
     * @throws
     */
    public static String parserDate2String(Date date) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        if (date == null) {
            return null;
        }
        return sdf.format(date);
    }

    /**
     * 
     * @Title: exitsOrMkdirs
     * @Description: 判断文件是否存在,若不存在则创建
     * @param
     * @param directory
     * @param
     * @return
     * @return boolean 返回类型
     * @throws
     */
    public static boolean exitsOrMkdirs(String directory) {
        if (isDirectory(directory)) {
            return true;
        } else {
            File file = new File(directory);
            file.mkdirs();
            return true;
        }
    }

    /**
     * 
     * @Title: getFileSize
     * @Description: 获取文件的大小(Byte)
     * @param
     * @param file
     * @param
     * @return
     * @param
     * @throws Exception
     * @return Long 返回类型
     * @throws
     */
    public static Long getFileSize(File file) throws Exception {
        if (!file.isFile()) {
            throw new Exception("Get File Size Exception. Reason: The file is not exits!");
        }
        return FileUtils.sizeOf(file);
    }

    /**
     * 
     * @Title: getFileSizeM
     * @Description: 获取文件的大小(M)
     * @param
     * @param file
     * @param
     * @return
     * @param
     * @throws Exception
     * @return Double 返回类型
     * @throws
     */
    public static Double getFileSizeM(File file) throws Exception {
        Long size = getFileSize(file);
        if (size > 0) {
            return new Long(size / 1024 / 1024).doubleValue();
        } else {
            return 0d;
        }
    }

    /**
     * 
     * @Title: getFileSize
     * @Description: 获取文件的大小
     * @param
     * @param filePath
     * @param
     * @return
     * @param
     * @throws Exception
     * @return Long 返回类型
     * @throws
     */
    public static Long getFileSize(String filePath) throws Exception {
        File file = new File(filePath);
        if (!file.isFile()) {
            throw new Exception("Get File Size Exception. Reason: The file is not exits!");
        }
        return FileUtils.sizeOf(file);
    }

    /**
     * 
     * @Title: getFileSizeM
     * @Description: 获取文件的大小(M)
     * @param
     * @param filePath
     * @param
     * @return
     * @param
     * @throws Exception
     * @return Double 返回类型
     * @throws
     */
    public static Double getFileSizeM(String filePath) throws Exception {
        Long size = getFileSize(filePath);
        if (size > 0) {
            return new Long(size / 1024 / 1024).doubleValue();
        } else {
            return 0d;
        }
    }

    /**
     * 
     * @Title: getFreeSpace
     * @Description: 获取磁盘剩余空间容量
     * @param dir
     * @return
     * @param
     * @throws Exception
     * @return Double 返回类型为磁盘容量，单位为(M).
     * @throws
     */
    public static Double getFreeSpace(String dir) throws Exception {
        double totalHD = 0;
        double usedHD = 0;
        double freeHD = 0;
        Runtime rt = Runtime.getRuntime();
        // Process p = rt.exec("df -hl " + dir);
        Process p = rt.exec("df -Ph " + dir);

        BufferedReader in = null;
        in = new BufferedReader(new InputStreamReader(p.getInputStream()));
        String str = null;
        String[] strArray = null;
        while ((str = in.readLine()) != null) {
            int m = 0;
            strArray = str.split(" ");
            for (String tmp : strArray) {
                if (tmp.trim().length() == 0) {
                    continue;
                }
                ++m;
                if (tmp.indexOf("G") != -1) {
                    if (m == 2) {
                        if (!tmp.equals("") && !tmp.equals("0"))
                            totalHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1)) * 1024;
                    }
                    if (m == 3) {
                        if (!tmp.equals("none") && !tmp.equals("0"))
                            usedHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1)) * 1024;

                    }
                    if (m == 4) {
                        if (!tmp.equals("none") && !tmp.equals("0"))
                            freeHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1)) * 1024;

                    }
                }
                if (tmp.indexOf("M") != -1) {
                    if (m == 2) {
                        if (!tmp.equals("") && !tmp.equals("0"))
                            totalHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1));

                    }
                    if (m == 3) {
                        if (!tmp.equals("none") && !tmp.equals("0"))
                            usedHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1));
                    }
                    if (m == 4) {
                        if (!tmp.equals("none") && !tmp.equals("0"))
                            freeHD += Double.parseDouble(tmp.substring(0, tmp.length() - 1));
                    }
                }
            }
        }
        in.close();
        return freeHD;
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
     * @return v1=v2返回值0;v1>v2返回大于0;v1<v2返回小于0；
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

    /**
     * 
     * @Title: contains
     * @Description: 判断是否包含
     * @param
     * @param queue
     * @param
     * @param o
     * @param
     * @return
     * @return boolean 返回类型
     * @throws
     */
    public static boolean contains(ConcurrentLinkedQueue<UpModel> queue, UpModel o) {
        if (queue.isEmpty())
            return false;
        if (o == null)
            return false;

        Object[] upModels = queue.toArray();
        for (Object obj : upModels) {
            UpModel up = (UpModel) obj;
            if (up.getOldVersion().equals(o.getOldVersion()) && up.getNewVersion().equals(o.getNewVersion()) && up.getUpMode().equals(o.getUpMode())) {
                return true;
            }
        }
        return false;
    }

    /**
     * 
     * @Title: remove
     * @Description: 移除对象
     * @param
     * @param queue
     * @param
     * @param o
     * @param
     * @return
     * @return boolean 返回类型
     * @throws
     */
    public static boolean remove(ConcurrentLinkedQueue<UpModel> queue, UpModel o) {
        if (queue.isEmpty())
            return true;
        if (o == null)
            return false;

        Object[] upModels = queue.toArray();
        for (Object obj : upModels) {
            UpModel up = (UpModel) obj;
            if (up.getOldVersion().equals(o.getOldVersion()) && up.getNewVersion().equals(o.getNewVersion())) {
                queue.remove(up);
            }
        }
        return true;
    }
    public static void main(String[] args) {
    	System.out.println(compareVersion("1.1.1.1","1.1.1.5"));
    	
	}
}