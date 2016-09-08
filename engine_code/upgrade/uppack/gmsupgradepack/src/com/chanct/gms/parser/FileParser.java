package com.chanct.gms.parser;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;

import com.chanct.gms.config.Global;
import com.chanct.gms.model.Action;
import com.chanct.gms.model.ConfigFileModel;
import com.chanct.gms.utiles.Utiles;
import com.chanct.gms.utiles.XMLUtiles;

/**
 * 
 * @ClassName: FileParser
 * @Description: 文件解析器
 * @author shf
 * @date Dec 5, 2013 2:11:14 PM
 * 
 */
public class FileParser {
    /**
     * 
     * @Title: getConfigFileModel
     * @Description: 解析升级配置文件
     * @param element
     * @throws Exception
     * @return ConfigFileModel 返回类型
     */
    @SuppressWarnings("unchecked")
    public ConfigFileModel getConfigFileModel(Element element) throws Exception {
        ConfigFileModel fileModel = new ConfigFileModel();
        int tag = Utiles.compareVersion(XMLUtiles.getElementValue(element, "SoftVersion"), XMLUtiles.getElementValue(element, "OldVersion"));
        if (tag < 1) {
            throw new Exception("SoftVersion is less than OldVersion.");
        }

        fileModel.setMainDir(XMLUtiles.getElementValue(element, "MainDir"));
        fileModel.setOldVersion(XMLUtiles.getElementValue(element, "OldVersion"));
        fileModel.setSoftVersion(XMLUtiles.getElementValue(element, "SoftVersion"));
        fileModel.setPackageType(XMLUtiles.getElementValue(element, "PackageType"));
        fileModel.setUpMode(XMLUtiles.getElementValue(element, "UpMode"));
        fileModel.setMinSpace(Integer.parseInt(XMLUtiles.getElementValue(element, "MinSpace")));

        List acTemps = XMLUtiles.getElements(XMLUtiles.getElement(element, "Actions"), "Action");
        List<Action> actions = new ArrayList<Action>();
        Element acEle;
        Action action;
        for (Iterator it = acTemps.iterator(); it.hasNext();) {
            acEle = (Element) it.next();
            action = new Action();
            action.setName(XMLUtiles.getElementAttribute(acEle, "name"));
            action.setValue(XMLUtiles.getElementAttribute(acEle, "value"));
            actions.add(action);
        }
        fileModel.setActions(actions);
        fileModel.setReturnAction(XMLUtiles.getElementValue(element, "Return"));
        fileModel.setDescription(XMLUtiles.getElementValue(element, "Description"));
        return fileModel;
    }

    /**
     * 
     * @Title: getFileCount
     * @Description: 获取升级配置文件中涉及到打包文件的操作
     * @param config
     * @throws Exception
     * @return List 返回类型
     */
    public List<Action> getFileCount(ConfigFileModel config) throws Exception {
        if (config == null) {
            return null;
        }
        List<Action> actions = config.getActions();
        List<Action> result = null;
        if (actions != null && actions.size() > 0) {
            result = new ArrayList<Action>();
            for (Action ac : actions) {
                if (ac.getName().toLowerCase().equals(Global.ACTION_ADD.toLowerCase()) || ac.getName().toLowerCase().equals(Global.ACTION_REPLACE.toLowerCase())) {
                    if (!(Utiles.isFile(ac.getValue()) || Utiles.isDirectory(ac.getValue()))) {
                        throw new Exception("升级包配置文件配置错误，请验证. 请确定" + ac.getValue() + "是否存在?");
                    }
                    if (Utiles.isFile(ac.getValue()) || Utiles.isDirectory(ac.getValue())) {
                        result.add(ac);
                    }
                }
            }
            return result;
        }
        return null;
    }
}
