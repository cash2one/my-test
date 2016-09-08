package com.chanct.gms.listenser;

import java.io.File;
import java.sql.Timestamp;
import java.util.Date;
import java.util.Random;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import com.chanct.gms.config.Global;
import com.chanct.gms.db.DbServiceImpl;
import com.chanct.gms.db.IDbService;
import com.chanct.gms.model.VersionModel;
import com.chanct.gms.parser.UnPackParser;

/**
 * 
 * @ClassName: FileListener
 * @Description: 文件监听实现
 * @author shf
 * @date Dec 9, 2013 5:00:36 PM
 * 
 */
public class FileListener extends BaseListener implements IFileListener {
    /** log日志 */
    private static final Log log = LogFactory.getLog(FileListener.class);

    /** 解包解析器 */
    private UnPackParser unPackParser = new UnPackParser();
    /**数据库对象**/
    private IDbService dbService = new DbServiceImpl();

    /**
     * Connstructor
     */
    public FileListener() {
        super();
    }

    public void onStart(Object monitoredResource) {
        // On startup
        if (monitoredResource instanceof File) {
            File resource = (File) monitoredResource;
            if (resource.isDirectory()) {
                if (log.isInfoEnabled()) {
                    log.info("开始监听 " + resource.getAbsolutePath());
                }
                /*
                 * File[] files = resource.listFiles(); for (int i = 0; i <
                 * files.length; i++) { File f = (File) files[i]; onAdd(f); }
                 */
            }
        }
    }

    public void onStop(Object notMonitoredResource) {
        if (notMonitoredResource instanceof File) {
            File resource = (File) notMonitoredResource;
            if (log.isInfoEnabled()) {
                log.info("停止监听 " + resource.getAbsolutePath());
            }
        }
    }

    public void onAdd(Object newResource) {
        if (newResource instanceof File) {
            File file = (File) newResource;
            if (file.isFile()) {
                if (log.isInfoEnabled()) {
                    log.info("监听到添加文件" + file.getAbsolutePath());
                }
                       
               
                try{
		                
		                String version = file.getName().split("-")[1];
		                Global.packMap.put(version, file.getAbsolutePath());
		                try {
		                    unPackParser.packFormatValid(file.getAbsolutePath());
		                    unPackParser.unPack(file.getAbsolutePath());
		                    Global.unPackMap.put(version, Global.UNPACKING_DIRECTORY + version + "/");
		                } catch (Exception e) {
		                    if (log.isInfoEnabled()) {
		                        log.info("文件监听发生异常. 原因: " + e.getMessage());
		                    }
		                } finally {
		                    Global.packMap.remove(file.getAbsolutePath());
		                }
                }catch(Exception e){
                	 if (log.isInfoEnabled()) {
                         log.info("升级包错误，无法升级！！ " );
                     }
                	  //-------------------------------------------------
                  //  if (!file.getName().startsWith(Global.UP_FILE_PRE)) {
                   	 //------------------------升级包错误------------------------
                	 Integer i=new Random().nextInt(999);
                       VersionModel ver = new VersionModel();
                       ver.setUpTime(new Date());
                       ver.setUpType(Global.UP_TYPE_HAND);
                       ver.setUpContTypeStr("");
                       ver.setOldVersion(Global.VERSION);
                       ver.setNewVersion("错误保持原版本:"+new Timestamp(ver.getUpTime().getTime()));
                       ver.setNewVerInfo("升级包错误，无法升级！！！"+i.toString());
                       ver.setUpStatus(Global.UP_STATUS_FAILT);
                       try {
                           dbService.addAlertMsg(ver);
                           if (log.isInfoEnabled()) {
                               log.info("升级包错误，无法升级！！！填入数据库成功 " );
                           }
                       } catch (Exception e1) {
                           e1.printStackTrace();
                           if (log.isInfoEnabled()) {
                               log.info("升级包错误，无法升级！！！填入数据库失败！ " );
                           }
                       }
                   	file.delete();
                    if (log.isInfoEnabled()) {
                        log.info("删除错误升级包,成功！" );
                    }
                    return;
              //  }
                    
                //----------------------------------------------------  
                }
            }
        }
    }

    public void onChange(Object changedResource) {
        // if (changedResource instanceof File) {
        // File file = (File) changedResource;
        // if (file.isFile()) {
        // System.out.println(file.getAbsolutePath() + " is changed");
        // }
        // }
    }

    public void onDelete(Object deletedResource) {
        // if (deletedResource instanceof String) {
        // String deletedFile = (String) deletedResource;
        // System.out.println(deletedFile + " is deleted");
        // }
    }
}
