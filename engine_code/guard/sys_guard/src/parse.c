#include <libxml/parser.h>
#include <libxml/tree.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include"guard_cfg.h"
#include"file_path.h"

int parse_xml(char *node_num, char *node_name)
{
	xmlDocPtr doc; //定义解析文档指针
	xmlNodePtr curNode; //定义结点指针(你需要它为了在各个结点间移动)
	xmlChar *szKey; //临时字符串变量
//	char *szDocName = "/home/xiedongling/svn/GMS/trunk/engine_code/guard/conf/version.xml";
	xmlKeepBlanksDefault(0);  
    	doc = xmlParseFile(guard_conf_cfg.sysversion_path); //解析文件
        printf("*******:%s", guard_conf_cfg.sysversion_path);
    	if (NULL == doc) { 
       		printf("Document not parsed successfully\n"); 
       		return -1;
    	}
    	curNode = xmlDocGetRootElement(doc); //确定文档根元素
    	if (NULL == curNode) {
       		printf("empty document\n");
       		xmlFreeDoc(doc);
       		return -1;
    	}
    	if (xmlStrcmp(curNode->name, BAD_CAST "gms")) {
       		printf("document of the wrong type, root node != gms\n");
       		xmlFreeDoc(doc);
       		return -1;
    	}

    	curNode = curNode->xmlChildrenNode;
    //	curNode = curNode->xmlChildrenNode;
    //	curNode = curNode->xmlChildrenNode;
   
	while(curNode != NULL) {
       //取出节点中的内容
		if((!xmlStrcmp(curNode->name, (const xmlChar *)node_name))) {
			szKey = xmlNodeGetContent(curNode);
       			printf("-----------name=%s  content = %s\n", curNode->name, szKey);
			strncpy(node_num, (const char *)szKey, (size_t)strlen((const char *)szKey));
		}
        	curNode = curNode->next;
    }
    	xmlFreeDoc(doc);
    	return 0; 
}

#if 0
int main(void)
{
    char node_name[20] = {0};
    char node_num[20] = {0};
    strncpy(node_name, "SysVersion", (int)strlen("SysVersion"));
    parse_xml(node_num, node_name);
    return 0;
}
#endif
