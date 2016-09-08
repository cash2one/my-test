package com.chanct.gms.parser;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.List;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.Action;
import com.chanct.gms.utiles.DesUtiles;
import com.chanct.gms.utiles.Utiles;

/**
 * 
 * @ClassName: BytebufferParser
 * @Description: 打包解析器
 * @author shf
 * @date Dec 5, 2013 2:19:37 PM
 * 
 */
public class PackParser {

    /**
     * 
     * @Title: packMagicString
     * @Description: 魔法棒封装
     * @param srcStr
     * @throws Exception
     * @return byte[] 返回类型
     */
    private byte[] packMagicString() throws Exception {
        byte[] result = DesUtiles.encrypt(Global.MAGIC_STRING.getBytes(), Global.DES_PASSWORD_KEY.getBytes());
        return result;
    }

    /**
     * 
     * @Title: packConfigFile
     * @Description: 封装升级配置文件
     * @throws Exception
     * @return byte[] 返回类型
     */
    private byte[] packConfigFile() throws Exception {
        byte[] configFile = FileUtils.readFileToByteArray(new File(Global.CONFIG_FILE));
        return configFile;
    }

    /**
     * 
     * @Title: packUpgradePackHead
     * @Description: 封装包头
     * @throws Exception
     * @return byte[] 返回类型
     */
    public byte[] packHeader(int packFileCount) throws Exception {
        byte[] magicStr = packMagicString();
        byte[] configFile = packConfigFile();

        ByteBuffer buffer = ByteBuffer.allocate(magicStr.length + 4 + configFile.length + 4);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put(magicStr);
        buffer.putInt(configFile.length);
        buffer.put(configFile);
        buffer.putInt(packFileCount);
        buffer.flip();
        return buffer.array();
    }

    /**
     * 
     * @Title: bytesAddBytes
     * @Description: byte[]合并，dst放到src后.
     * @param src
     * @param dst
     * @throws Exception
     * @return byte[] 返回类型
     */
    public byte[] bytesAddBytesFile(byte[] src, byte[] dst) throws Exception {
        ByteBuffer buffer;
        if (src == null) {
            buffer = ByteBuffer.allocate(dst.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            buffer.put(dst);
            buffer.flip();
        } else {
            buffer = ByteBuffer.allocate(src.length + dst.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            buffer.put(src);
            buffer.put(dst);
            buffer.flip();
        }
        return buffer.array();
    }

    /**
     * 
     * @Title: bytesAddBytesPack
     * @Description: 封装升级包
     * @param
     * @param header
     * @param
     * @param body
     * @param
     * @return
     * @param
     * @throws Exception
     * @return byte[] 返回类型
     * @throws
     */
    public byte[] bytesAddBytesPack(byte[] header, byte[] body) throws Exception {
        ByteBuffer buffer;
        if (body == null) {
            return header;
        } else {
            buffer = ByteBuffer.allocate(header.length + body.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            buffer.put(header);
            buffer.put(body);
            buffer.flip();
        }
        return buffer.array();
    }

    /**
     * 
     * @Title: packSignleFile
     * @Description: 单个文件打包
     * @param filePath
     * @throws Exception
     * @return byte[] 返回类型
     */
    @SuppressWarnings("static-access")
    private byte[] packSignleFile(String filePath) throws Exception {
        File f = new File(filePath);
        if (Utiles.isFile(f)) {// 处理文件
            byte[] file = FileUtils.readFileToByteArray(f);
            ByteBuffer buffer = ByteBuffer.allocate(100 + 4 + file.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            byte[] fileName = f.getName().getBytes();

            ByteBuffer fileNameBuff = ByteBuffer.allocate(100);
            fileNameBuff.order(ByteOrder.LITTLE_ENDIAN);
            fileNameBuff.put(fileName);
            fileNameBuff.flip();
            buffer.put(fileNameBuff.array());
            buffer.putInt(file.length);
            buffer.put(file);
            buffer.flip();
            return buffer.array();
        }
        if (Utiles.isDirectory(f)) {// 处理目录,若为目录则创建一个“目录名.isTmpDir”的文件，然后打进包
            if (filePath.endsWith("/")) {
                filePath = filePath.substring(0, filePath.length() - 1);
            }
            String name = FilenameUtils.getName(filePath);
            String fullPath = FilenameUtils.getFullPath(filePath);
            String tmpDirFile = fullPath + name + ".isTmpDir";
            File tmpFile = new File(tmpDirFile);
            FileUtils.writeStringToFile(tmpFile, "");
            byte[] file = FileUtils.readFileToByteArray(tmpFile);
            ByteBuffer buffer = ByteBuffer.allocate(100 + 4 + file.length);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            byte[] fileName = tmpFile.getName().getBytes();

            ByteBuffer fileNameBuff = ByteBuffer.allocate(100);
            fileNameBuff.order(ByteOrder.LITTLE_ENDIAN);
            fileNameBuff.put(fileName);
            fileNameBuff.flip();
            buffer.put(fileNameBuff.array());
            buffer.putInt(file.length);
            buffer.put(file);
            buffer.flip();
            FileUtils.forceDeleteOnExit(tmpFile);
            return buffer.array();
        }
        return null;
    }

    /**
     * 
     * @Title: packBody
     * @Description: 封装包体
     * @param actionFiles
     * @throws Exception
     * @return byte[] 返回类型
     */
    public byte[] packBody(List<Action> actionFiles) throws Exception {
        if (actionFiles == null || actionFiles.size() < 1) {
            return null;
        }
        byte[] result = null;
        for (Action ac : actionFiles) {
            byte[] singleFile = packSignleFile(ac.getValue());
            result = bytesAddBytesFile(result, singleFile);
        }
        return result;
    }
}
