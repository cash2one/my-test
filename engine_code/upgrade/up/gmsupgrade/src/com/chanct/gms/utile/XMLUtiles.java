package com.chanct.gms.utile;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

/**
 * 
 * @ClassName: XMLUtiles
 * @Description: XML工具类
 * @author shf
 * @date Dec 4, 2013 4:09:14 PM
 * 
 */
public class XMLUtiles {

    /**
     * 
     * @Title: openXML
     * @Description: 读取XML文件
     * @param xmlPath
     * @return Document 返回类型
     * @throws DocumentException
     * @throws FileNotFoundException
     */
    public static Document readXML(String xmlPath) throws DocumentException, FileNotFoundException {
        try {
            FileInputStream in = new FileInputStream(xmlPath);
            Reader reader = new InputStreamReader(in, "UTF-8");
            SAXReader saxReader = new SAXReader();
            return saxReader.read(reader);
        } catch (Exception e) {
            // e.printStackTrace();
        }
        return null;
    }

    /**
     * 
     * @Title: getRootElement
     * @Description: 获取doc的根Element
     * @param doc
     * @return Element 返回类型
     * @throws
     */
    public static Element getRootElement(Document doc) {
        return doc.getRootElement();
    }

    /**
     * 
     * @Title: getElement
     * @Description: 根据名称从父Element得到子Element
     * @param pElement
     * @param cElementName
     * @return Element 返回类型
     */
    public static Element getElement(Element pElement, String cElementName) {
        return pElement.element(cElementName);
    }

    /**
     * 
     * @Title: getElementValue
     * @Description: 从父Element获得子Element的值
     * @param pElement
     * @param cElementName
     * @return String 返回类型
     */
    public static String getElementValue(Element pElement, String cElementName) {
        return pElement.elementText(cElementName);
    }

    /**
     * 
     * @Title: getElementAttribute
     * @Description: 获取Element的Attribute值
     * @param element
     * @param attributeName
     * @return String 返回类型
     */
    public static String getElementAttribute(Element element, String attributeName) {
        return element.attributeValue(attributeName);
    }

    /**
     * 
     * @Title: getElements
     * @Description: 根据cElementName获取pElement的Element列表
     * @param pElement
     * @param cElementName
     * @return List 返回类型
     */
    @SuppressWarnings("unchecked")
    public static List getElements(Element pElement, String cElementName) {
        return pElement.elements(cElementName);
    }
}
