package com.chanct.gms.parser;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Random;

import org.apache.commons.io.FileUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dom4j.Document;
import org.dom4j.Element;

import com.chanct.gms.config.Global;
import com.chanct.gms.db.DbServiceImpl;
import com.chanct.gms.db.IDbService;
import com.chanct.gms.model.UpModel;
import com.chanct.gms.model.VersionModel;
import com.chanct.gms.utile.DesUtiles;
import com.chanct.gms.utile.MD5Utiles;
import com.chanct.gms.utile.Utiles;
import com.chanct.gms.utile.XMLUtiles;

/**
 * 
 * @ClassName: UnPackParser
 * @Description: 解包解析器
 * @author shf
 * @date Dec 14, 2013 2:19:37 PM
 * 
 */
public class UnPackParser {

    /** log日志 */
    private static final Log log = LogFactory.getLog(UnPackParser.class);

    private IDbService dbService = new DbServiceImpl();

    /**
     * 
     * @Title: packFormatValid
     * @Description: 验证升级包格式是否正确
     * @param
     * @param file
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public boolean packFormatValid(String file) throws Exception {
        File pack = new File(file);
        // 验证升级包是否存在
        if (!pack.isFile()) {
            throw new Exception("升级包格式验证失败. 原因:'" + pack.getName() + "' 不存在!");
        }

        // 升级包名格式验证
        if (!pack.getName().startsWith(Global.UP_FILE_PRE)) {
        	 //------------------------升级包错误------------------------
        	Integer i=new Random().nextInt(999);
            VersionModel ver = new VersionModel();
            ver.setUpTime(new Date());
            ver.setUpType(Global.UP_TYPE_HAND);
            ver.setUpContTypeStr("");
            ver.setOldVersion(Global.VERSION);
            ver.setNewVersion("错误保持原版本:"+new Timestamp(ver.getUpTime().getTime()));
            ver.setNewVerInfo("升级包错误，名称不合法！！！"+i.toString());
            ver.setUpStatus(Global.UP_STATUS_FAILT);
            try {
                dbService.addAlertMsg(ver);
                if (log.isInfoEnabled()) {
                    log.info("升级包错误，无法升级！！！填入数据库成功 " );
                }
            } catch (Exception e) {
                e.printStackTrace();
               // Global.unPackMap.remove(version);
                throw new Exception("升级包错误,数据库添加升级消息失败 ");
            }
            pack.delete();
            if (log.isInfoEnabled()) {
                log.info("---------错误文件删除------------------ " );
            }
        	
            throw new Exception("升级包格式验证失败. 原因:'" + pack.getName() + "' 名称不合法!");
        }

        // 升级包MD5验证
        String md5New = MD5Utiles.MD5Encode(FileUtils.readFileToByteArray(pack));
        String md5Old = pack.getName().split("-")[2];
        if (!md5New.equals(md5Old)) {
        	
            throw new Exception("升级包格式验证失败. 原因:'" + pack.getName() + "' MD5值验证失败!");
        }
        if (log.isInfoEnabled()) {
            log.info("[" + file + "] 解包前格式验证成功. ");
        }
        return true;
    }

    /**
     * 
     * @Title: unPack
     * @Description: 解包
     * @param
     * @param filePath
     * @param
     * @return
     * @param
     * @throws Exception
     * @return boolean 返回类型
     * @throws
     */
    public void unPack(String filePath) throws Exception {
        File pack = new File(filePath);

        // 分解升级包名称
        String[] pNameArray = pack.getName().split("-");
        String version = pNameArray[1];
        String upType = pNameArray[3];

        // 检测是否已经解包
        if (Global.unPackMap.contains(version)) {
            // 已经解包成功，删除原始文件
            if (log.isInfoEnabled()) {
                log.info("[" + filePath + "] 已经解过包. ");
            }
            FileUtils.forceDelete(pack);
            return;
        }

        // 磁盘空间验证(剩余空间大于升级包2倍)
        Double packSize = Utiles.getFileSizeM(pack);
        Double freeSpace = Utiles.getFreeSpace(Global.UNPACKING_DIRECTORY);
        if (packSize * 2 >= freeSpace) {
            throw new Exception("解包失败. 原因: 空间不足.");
        }

        // 升级包DES解密
        byte[] packBytes = FileUtils.readFileToByteArray(pack);
        packBytes = DesUtiles.decrypt(packBytes, Global.DES_PASSWORD_KEY.getBytes());
        ByteBuffer buffer = ByteBuffer.allocate(packBytes.length);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put(packBytes);
        buffer.flip();

        // 魔法棒验证
        byte[] magic = new byte[24];
        buffer.get(magic, 0, 24);
        magic = DesUtiles.decrypt(magic, Global.DES_PASSWORD_KEY.getBytes());
        if (!new String(magic).equals(Global.MAGIC_STRING)) {
            throw new Exception("解包失败. 原因:'" + pack.getName() + "'魔法棒验证失败!");
        }

        if (log.isInfoEnabled()) {
            log.info("[" + filePath + "] 解包中魔法棒/磁盘/DES/MD5验证通过. ");
        }

        // 在解包目录下,为当前升级包创建解包目录
        Utiles.exitsOrMkdirs(Global.UNPACKING_DIRECTORY + version);

        // 还原升级配置文件
        int up_conf_file_len = buffer.getInt();
        byte[] up_conf_file = new byte[up_conf_file_len];
        buffer.get(up_conf_file);
        FileUtils.writeByteArrayToFile(new File(Global.UNPACKING_DIRECTORY + version + "/" + Global.UPGRADE_FILE_NAME), up_conf_file);

        // 还原升级文件
        int file_count = buffer.getInt();
        for (int i = 1; i <= file_count; i++) {
            byte[] file_name_array = new byte[100];
            buffer.get(file_name_array);
            String file_name = StringUtils.trim(new String(file_name_array));

            if (file_name.endsWith(".isTmpDir")) {
                file_name = file_name.substring(0, file_name.length() - 9);
                int file_len = buffer.getInt();
                byte[] file = new byte[file_len];
                buffer.get(file);
                FileUtils.forceMkdir(new File(Global.UNPACKING_DIRECTORY + version + "/" + file_name));
            } else {
                int file_len = buffer.getInt();
                byte[] file = new byte[file_len];
                buffer.get(file);
                FileUtils.writeByteArrayToFile(new File(Global.UNPACKING_DIRECTORY + version + "/" + file_name), file);
            }
        }

        if (log.isInfoEnabled()) {
            log.info("[" + filePath + "] 解包成功. ");
        }

        // 添加解包文件
        Global.unPackMap.put(version, Global.UNPACKING_DIRECTORY + version + "/");

        // 判断是否紧急升级
        Document doc = XMLUtiles.readXML(Global.UNPACKING_DIRECTORY + version + "/" + Global.UPGRADE_FILE_NAME);
        Element root = doc.getRootElement();
        if (upType.equals(Global.UPMode_EMERGENCY)) {// 紧急升级
            UpModel up = new UpModel();
            up.setNewVersion(version);
            up.setOldVersion(root.elementText("OldVersion"));
            up.setUpMode(Global.UPMode_EMERGENCY);
            if (!Utiles.contains(Global.upQueue, up)) {
                Global.upQueue.offer(up);
                FileUtils.forceDelete(pack);
                if (log.isInfoEnabled()) {
                    log.info("紧急升级,成功加入升级队列. From " + up.getOldVersion() + " To " + version);
                }
            } else {
                if (log.isInfoEnabled()) {
                    log.info("紧急升级,成功加入已在升级队列中. From " + up.getOldVersion() + " To " + version);
                }
            }
            //------------------------紧急升级入库------------------------
            VersionModel ver = new VersionModel();
            ver.setUpTime(new Date());
            ver.setUpType(Global.UP_TYPE_HAND);
            ver.setUpContTypeStr(root.elementText("PackageType"));
            ver.setOldVersion(root.elementText("OldVersion"));
            ver.setNewVersion(version);
            ver.setNewVerInfo(root.elementText("Description"));
            ver.setUpStatus(Global.UP_STATUS_NO);
            try {
                dbService.addAlertMsg(ver);
                if (log.isInfoEnabled()) {
                    log.info("紧急升级,数据库添加升级消息成功. From " + ver.getOldVersion() + " To " + version);
                }
            } catch (Exception e) {
                e.printStackTrace();
               // Global.unPackMap.remove(version);
                throw new Exception("紧急升级,数据库添加升级消息失败. From " + ver.getOldVersion() + " To " + version);
            }
            
            
          //------------------------------------------------
            
            
        } else {// 添加升级提醒消息
            VersionModel ver = new VersionModel();
            ver.setUpTime(new Date());
            ver.setUpType(Global.UP_TYPE_HAND);
            ver.setUpContTypeStr(root.elementText("PackageType"));
            ver.setOldVersion(root.elementText("OldVersion"));
            ver.setNewVersion(version);
            ver.setNewVerInfo(root.elementText("Description"));
            ver.setUpStatus(Global.UP_STATUS_NO);
            try {
                dbService.addAlertMsg(ver);
                if (log.isInfoEnabled()) {
                    log.info("普通升级,数据库添加升级消息成功. From " + ver.getOldVersion() + " To " + version);
                }
            } catch (Exception e) {
                e.printStackTrace();
                Global.unPackMap.remove(version);
                throw new Exception("普通升级,数据库添加升级消息失败. From " + ver.getOldVersion() + " To " + version);
            }
            FileUtils.forceDelete(pack);
        }
    }
}
