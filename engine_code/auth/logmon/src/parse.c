#include <libxml/parser.h>
#include <libxml/tree.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"make_log.h"
#include"auth_cfg.h"
#include"logmon.h"

int parse_xml(char *node_num, char *node_name)
{
	xmlDocPtr doc; //定义解析文档指针
	xmlNodePtr curNode; //定义结点指针(你需要它为了在各个结点间移动)
	xmlChar *szKey; //临时字符串变量
	//char *szDocName = "/gms/comm/conf/comm_interface_conf.xml";
	xmlKeepBlanksDefault(0);  
    	doc = xmlParseFile(auth_conf_cfg.xml_path); //解析文件
    	if (NULL == doc) { 
       		printf("Document not parsed successfully\n"); 
       		CA_LOG(AUTH_MODULE, LOGMON_PROC, "Document not parsed successfully\n");
            return -1;
    	}
    	curNode = xmlDocGetRootElement(doc); //确定文档根元素
    	if (NULL == curNode) {
       		printf("empty document\n");
       		xmlFreeDoc(doc);
       		return -1;
    	}
    	if (xmlStrcmp(curNode->name, BAD_CAST "cominfo")) {
       		printf("document of the wrong type, root node != cominfo\n");
       		xmlFreeDoc(doc);
       		return -1;
    	}

    	curNode = curNode->xmlChildrenNode;
    	curNode = curNode->xmlChildrenNode;
    	curNode = curNode->xmlChildrenNode;
   
	while(curNode != NULL) {
       //取出节点中的内容
		if((!xmlStrcmp(curNode->name, (const xmlChar *)node_name))) {
			szKey = xmlNodeGetContent(curNode);
       			printf("name=%s  content = %s\n", curNode->name, szKey);
			strncpy(node_num, (char *)szKey, 1);
		}
        	curNode = curNode->next;
    }
    	xmlFreeDoc(doc);
    	return 0; 
}

