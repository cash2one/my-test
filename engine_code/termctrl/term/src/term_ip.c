#include "term_main.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <libxml/xmlmemory.h> 
#include <libxml/parser.h> 

void parseStory(xmlDocPtr doc, xmlNodePtr cur,const char *ip,const char *gateway,const char *netmask,const char *dns){  
    cur=cur->xmlChildrenNode;  
    while(cur != NULL){  
        /* ¿¿keyword¿¿¿ */  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"gms_network")){ 
		   break;	
        }  
        cur=cur->next; /* ¿¿¿¿¿¿ */  
    } 
    cur=cur->xmlChildrenNode;  
    while(cur != NULL){  
        /* ¿¿keyword¿¿¿ */  
		//printf("%s\n",cur->name);
        if(!xmlStrcmp(cur->name, (const xmlChar *)"net")){ 
			xmlSetProp (cur, (const xmlChar *)"ip", (const xmlChar *)ip); 
			xmlSetProp (cur, (const xmlChar *)"gateway", (const xmlChar *)gateway); 
			xmlSetProp (cur, (const xmlChar *)"netmask", (const xmlChar *)netmask); 
		    	
        }  
        if(!xmlStrcmp(cur->name, (const xmlChar *)"dns")){ 
			xmlSetProp (cur, (const xmlChar *)"dns1", (const xmlChar *)dns); 
		    	
        }  
        cur=cur->next; /* ¿¿¿¿¿¿ */  
    } 

  
    return;  
}  
xmlDocPtr parseDoc(char *docname, const char *ip,const char *gateway,const char *netmask,const char *dns) { 
	xmlDocPtr doc; 
	xmlNodePtr cur; 
	doc = xmlParseFile(docname); 
	if (doc == NULL ) { 
		fprintf(stderr,"Document not parsed successfully. \n"); 
		return (NULL); 
	} 
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL) { 
		fprintf(stderr,"empty document\n"); 
		xmlFreeDoc(doc); 
		return (NULL); 
	} 
	if (xmlStrcmp(cur->name, (const xmlChar *) "gmsconf")) { 
		fprintf(stderr,"document of the wrong type, root node != gmsconf"); 
		xmlFreeDoc(doc); 
		return (NULL); 
	}
    parseStory(doc, cur,ip,gateway,netmask,dns);  
   //cur=cur->xmlChildrenNode;	
	//newnode = xmlNewTextChild (cur, NULL, "reference", NULL); 
	//newattr = xmlNewProp (newnode, "uri", uri); 
	return(doc); 
} 
int write_gmsconf(const char *ip,const char *gateway,const char *netmask,const char *dns)
{
	char *docname; 
	docname=DOC_NAME;
	xmlDocPtr doc; 
	doc = parseDoc (docname, ip,gateway,netmask,dns); 
	if (doc != NULL) { 
		xmlSaveFormatFile (docname, doc, 1); 
		xmlFreeDoc(doc); 
	} 
	return (1); 

}


int get_ethernet_msg(int eth,char * ip,char * netmask)
{
	FILE * fd;
	char  filename[128];
	char  linebuf[128];
	int iflag ;
	int retl, find;
	char   gb_gateway_temp[16];

	bzero(filename,128 * sizeof(char));
	strncpy(filename,ETH_CFG_FILE,127);
	filename[strlen(ETH_CFG_FILE)]=itochar(eth);


	fd = fopen(filename,"r");
	if (fd == NULL ) 
	{
		return 0;
	}

	iflag = 0;
	find = 0;
	while(feof(fd)==0)
	{
		bzero(linebuf,128 * sizeof(char));
		retl = line_input(fd,linebuf);//´ÓÎÄ¼þfdÖÐ¶ÁÈ¡Ò»ÐÐµ½linebufÖÐ
		if (retl>0)
		{
			if (is_record(linebuf,"IPADDR\0",'='))
			{
				if (get_record(linebuf,"IPADDR\0",'=','\"',ip)<=0)
					get_record(linebuf,"IPADDR\0",'=','\0',ip);
				if (IsValidIpaddr(ip))
					iflag++;
				ip_t=ip;


			}	   
			if (is_record(linebuf,"NETMASK\0",'='))
			{
				if (get_record(linebuf,"NETMASK\0",'=','\"',netmask)<=0)
					get_record(linebuf,"NETMASK\0",'=','\0',netmask);
				if (IsValidIpaddr(netmask))
					iflag++; 

			}
			if (is_record(linebuf,"GATEWAY\0",'='))
			{
				if(get_record(linebuf,"GATEWAY\0",'=','\"',gb_gateway_temp)<=0)
					get_record(linebuf,"GATEWAY\0",'=','\0',gb_gateway_temp);
				find++;	
			}		

		}
		if (iflag==3)
			break;
	}
	fclose(fd);	
	return iflag; 
}
int set_comm_xml(const char *ip)
{
	FILE*ip_r;
	FILE*ip_w;
	char cstrip[100];
	char *cmdgv[]={"cp",TMP_NETPORT,NETPORT,NULL};
	size_t s;
	char ch;
	int i=0;
	pid_t pid;
	ip_r=fopen(NETPORT,"r");
	if(ip_r==NULL)
	{
		sprintf(cstrip, "fopen(%s):%s", NETPORT,strerror(errno));
		SendOneLine(cstrip);
		return -1;
	}
	ip_w=fopen(TMP_NETPORT,"w");
	if(ip_w==NULL)
	{
		sprintf(cstrip, "fopen(%s):%s", TMP_NETPORT,strerror(errno));
		SendOneLine(cstrip);
		fclose(ip_r);
		return -1;
	}
	while(1)
	{
		s=fread(&ch,1,1,ip_r);
		if(s<=0)
		{
			break;
		}
		fwrite(&ch,1,1,ip_w);
		if(ch=='i')
		{
			s=fread(&ch,1,1,ip_r);
			fwrite(&ch,1,1,ip_w);
			if(ch=='p')
			{
				s=fread(&ch,1,1,ip_r);
				fwrite(&ch,1,1,ip_w);
				if(ch=='>')
				{
					if(i==0)
					{
						while(ch!='<')
						{
							s=fread(&ch,1,1,ip_r);

						}
						fprintf(ip_w,ip);
						fwrite(&ch,1,1,ip_w);
					}
					i++;

				}	

			}

		}
	}
	fclose(ip_r);
	fclose(ip_w);
	pid=fork();
	if(pid==0)
	{
		execvp(cmdgv[0],cmdgv);
		exit(1);
	}
	wait(NULL);
    unlink(TMP_NETPORT);
	return 1;
}
int netmask_str2len(const char* mask)
{
    int netmask = 0;
    unsigned int mask_tmp;

    mask_tmp = ntohl((int)inet_addr(mask));
    while (mask_tmp & 0x80000000)
    {
        netmask++;
        mask_tmp = (mask_tmp << 1);
    }

    return netmask;    
}
int set_ethernet_msg(int eth,const char * ip, const char * netmask,const char *gateway,const char *dns)
{
	//FILE * fd;
	FILE * tm;
	char   filename[128];
	char   dev[20];
	char   buf[100];
//	char * linestr=NULL;
//	size_t t=0;
//	ssize_t size=0;
//	char hwadd[50]={0};
//	char uuid[55]={0};
//	char name[60]={0};
//	char prefix[50]={0};
//	char dns[30]={0},last[50]={0};

	bzero(filename,128 * sizeof(char));
	strncpy(filename,ETH_CFG_FILE,127);
	filename[strlen(ETH_CFG_FILE)]=itochar(eth); 


	bzero(dev,20 * sizeof(char));
	strncpy(dev,"eth\0",20);
	dev[3]=itochar(eth); 
#if 0
	fd = fopen(filename,"r+");
	if (fd == NULL)
	{
		sprintf(buf,"Can't access %s \n",filename);
		SendOneLine(buf);
		return 0;
	}
	while(1)
	{
		size=getline(&linestr,&t,fd);
		if(size<=0)
		{
			break;
		}
		else
		{
			if(strncmp(linestr,"HWADDR=",7)==0)
			{
				strcpy(hwadd,linestr);
			}
			else if(strncmp(linestr,"UUID=",5)==0)
			{
				strcpy(uuid,linestr);
			}
			else if(strncmp(linestr,"NAME=",5)==0)
			{
				strcpy(name,linestr);
			}
			else if(strncmp(linestr,"PREFIX=",7)==0)
			{
				strcpy(prefix,linestr);
			}
			else if(strncmp(linestr,"DNS1=",5)==0)
			{
				strcpy(dns,linestr);
			}
			else if(strncmp(linestr,"LAST_CONNECT=",12)==0)
			{
				strcpy(last,linestr);
			}
		}	
		bzero(linestr,strlen(linestr));
	}
	fclose(fd);
	if(linestr!=NULL)
	{
		free(linestr);
	}

#endif
	tm = fopen(filename,"w");
	if (tm == NULL)
	{
		sprintf(buf,"Can't access %s \n",filename);
		SendOneLine(buf);
		return 0;
	}
#if 0
	uuid[41]='\n';
	uuid[42]='\0';
	fprintf(tm,"DEVICE=%s\n",dev);
	fprintf(tm,"TYPE=Ethernet\n");
	fprintf(tm,uuid);
	fprintf(tm,"ONBOOT=yes\n");
	fprintf(tm,"BOOTPROTO=none\n");
	fprintf(tm,"DEFROUTE=yes\n");
	fprintf(tm,"IPV4_FAILURE_FATAL=yes\n");
	fprintf(tm,"IPV6INIT=no\n");
	fprintf(tm,name);
	fprintf(tm,hwadd);
	fprintf(tm,"IPADDR=%s\n",ip);
	fprintf(tm,prefix);
	fprintf(tm,"GATEWAY=%s\n",gateway);
	fprintf(tm,dns);
	fprintf(tm,last);
	fprintf(tm,"NETMASK=%s\n",netmask);
#endif
	fprintf(tm,"IPADDR=%s\n",ip);
	fprintf(tm,"GATEWAY=%s\n",gateway);
	fprintf(tm,"PREFIX=%d\n",netmask_str2len(netmask));
	fprintf(tm,"NETMASK=%s\n",netmask);
	fprintf(tm,"DNS=%s\n",dns);
	fclose(tm);
	set_comm_xml(ip);
	char sed_cmd[300];
	bzero(sed_cmd,300);
	sprintf(sed_cmd,"sed -i '/^<?xml/d' %s;sed -i '1i <?xml version=\"1.0\" encoding=\"utf-8\"?>' %s",DOC_NAME,DOC_NAME);
	system(sed_cmd);
	write_gmsconf(ip,gateway,netmask,dns);

	return 1;		
}
#if 0
int set_ethernet_msg(int eth,const char * ip, const char * netmask,const char *gateway)
{
	FILE * fd;
	FILE * tm;
	char   filename[128],tpfilename[30];
	char   dev[20];
	char   buf[100];
	char * linestr=NULL;
	size_t t=0;
	ssize_t size=0;

	bzero(filename,128 * sizeof(char));
	strncpy(filename,ETH_CFG_FILE,127);
	filename[strlen(ETH_CFG_FILE)]=itochar(eth); 


	bzero(dev,20 * sizeof(char));
	strncpy(dev,"eth\0",20);
	dev[3]=itochar(eth); 
	tm = fopen(IP_TMP,"w");
	if (tm == NULL)
	{
		sprintf(buf,"Can't access %s \n",filename);
		SendOneLine(buf);
		return 0;
	}

	fd = fopen(filename,"r+");
	if (fd == NULL)
	{
		sprintf(buf,"Can't access %s \n",filename);
		SendOneLine(buf);
		fclose(tm);
		return 0;
	}
	while(1)
	{
		size=getline(&linestr,&t,fd);
		if(size<=0)
		{
			break;
		}
		else
		{
			if(strncmp(linestr,"DEVICE=",7)==0)
			{
				fprintf(tm,"DEVICE=%s\n",dev);
			}
			else if(strncmp(linestr,"ONBOOT=",7)==0)
			{
				fprintf(tm,"ONBOOT=yes\n");
			}
			else if(strncmp(linestr,"BOOTPROTO=",10)==0)
			{
				fprintf(tm,"BOOTPROTO=none\n");
			}
			else if(strncmp(linestr,"IPADDR=",7)==0)
			{
				fprintf(tm,"IPADDR=%s\n",ip);
			}
			else if(strncmp(linestr,"NETMASK=",8)==0)
			{
				fprintf(tm,"NETMASK=%s\n",netmask);
			}
			else if(strncmp(linestr,"GATEWAY=",8)==0)
			{
				fprintf(tm,"GATEWAY=%s\n",gateway);
			}
			else
			{
				fprintf(tm,linestr);
			}
		}	
	}
	if(linestr!=NULL)
		free(linestr);
	sprintf(tpfilename,"rm -rf %s",filename);
	system(tpfilename);
	sprintf(tpfilename,"mv  %s %s",IP_TMP,filename);
	system(tpfilename);
	fclose(tm);
	fclose(fd);

	return 1;		
}
#endif
int set_ethernet_null(int eth)
{
	FILE * fd;
	char   filename[128],tpfilename[30];
	char   dev[20];
	bzero(filename,128 * sizeof(char));
	strncpy(filename,ETH_CFG_FILE,127);
	filename[strlen(ETH_CFG_FILE)]=itochar(eth); /*get the ethernet's config_file name*/

	bzero(tpfilename,30 * sizeof(char));
	strncpy(tpfilename,"./tmpcfg\0",20); /*default template file */

	bzero(dev,20 * sizeof(char));
	strncpy(dev,"eth\0",20);
	dev[3]=itochar(eth); /*ethernet device_name*/


	fd = fopen(filename,"w");
	if (fd == NULL)
	{
		printf("Can't access %s \n",filename);
		return 0;
	}
	fprintf(fd,"DEVICE=\"%s\"\n",dev);
	fprintf(fd,"ONBOOT=\"yes\"\n");
	fprintf(fd,"BOOTPROTO=\"none\"\n");
	fprintf(fd,"IPXNETNUM_802_2=\"\"\n");
	fprintf(fd,"IPXPRIMARY_802_2=\"no\"\n");
	fprintf(fd,"IPXACTIVE_802_2=\"no\"\n");
	fprintf(fd,"IPXNETNUM_802_3=\"\"\n");
	fprintf(fd,"IPXPRIMARY_802_3=\"no\"\n");
	fprintf(fd,"IPXACTIVE_802_3=\"no\"\n");
	fprintf(fd,"IPXNETNUM_ETHERII=\"\"\n");
	fprintf(fd,"IPXPRIMARY_ETHERII=\"no\"\n");
	fprintf(fd,"IPXACTIVE_ETHERII=\"no\"\n");
	fprintf(fd,"IPXNETNUM_SNAP=\"\"\n");
	fprintf(fd,"IPXPRIMARY_SNAP=\"no\"\n");


	fclose(fd);
	return 1;		

}
int IsValidIpaddr(const char * ip)
{
	struct sockaddr_in myaddr;
	char Ip[128];


	bzero(Ip,128 * sizeof(char));
	strncpy(Ip,ip,127);
	trim_str(Ip);
	if(strlen(Ip)<7)
	{
		return 0;
	}
	myaddr.sin_addr.s_addr=inet_addr(Ip);
	if (myaddr.sin_addr.s_addr==INADDR_NONE)
		return 0;
	return -1;	
}


int get_record(char * buf ,char * property,
		char  seperator,char bracket,char * value)
{
	int i ,length;
	int j; 
	char p[128];

	length = strlen(buf);
	if (length==0) 
		return -1; /*It is a invalid property line */

	bzero(p,128 * sizeof(char));
	for (i = 0;i< length;i++)
		if(buf[i]==seperator)
			break;
		else
			p[i]=buf[i];
	p[i]='\0';

	if (trim_str(p)==0)
		return -1;/*It is a invalid property line */
	if (strcasecmp(p,property)!=0)
		return -1;/*It is a invalid property line */
	if ( i == length)
	{
		value[0]='\0';
		return 0;/* Null Value is return */	
	}

	i ++;
	bzero(p,128 * sizeof(char));
	j = 0;
	if (bracket=='\0')
	{
		for ( ;i<length;i++,j++)
			p[j]=buf[i];
	}
	else
	{
		for ( ;i <length;i ++)
			if (buf[i]==bracket)
				break;
		i++;
		for ( ;i < length;i ++,j++)
			if(buf[i]==bracket)
				break;
			else
				p[j]=buf[i];
		p[j]='\0'; 		
	}
	trim_str(p);
	strncpy(value, p, strlen(p));
	value[strlen(p)]='\0';
	return (strlen(p));
}

int set_record(char * buf, const char * property,
		char  seperator,char bracket,const char * value)
{
	int ret;

	if ( bracket=='\0')
	{
		ret = sprintf(buf,"%s%c%s",property,seperator,value);
	}	
	else
	{
		ret = sprintf(buf,"%s%c%c%s%c",property,seperator,bracket,value,bracket);
	}
	return ret;

}
int is_record(const char * buf , const char * property,char seperator)
{
	int i ,length;
	char p[128];

	length = strlen(buf);
	if (length==0) 
		return 0; /*It is a invalid property line */

	bzero(p,128 * sizeof(char));
	for (i = 0;i< length;i++)
		if(buf[i]==seperator)
			break;
		else
			p[i]=buf[i];
	p[i]='\0';
	//printf("%s \n",p);//debug			
	if (trim_str(p)==0)
		return 0;/*It is a invalid property line */
	if (strcasecmp(p,property)!=0)
		return 0;/*It is not the indicated property */
	return -1;
}
int trim_str(char * buf)
{
	int i ,j;
	int length;

	length = strlen(buf);
	if (length == 0)
		return 0; /*zero length sring*/
	for ( i =0 ;i < length;	i ++)
		if ( isblank(buf[i])==0)
			break;
	j=0;
	for(;i < length;i ++,j++)		
		buf[j]=buf[i];
	buf[j]='\0';  
	length =strlen(buf);  
	for ( i = length-1;length>0 ;i --)
		if(isblank(buf[i]))
			buf[i]='\0';
		else
			break;
	return strlen(buf);	        
}
int line_input(FILE * fd,char * linebuf)
{
	char ch;
	int i =0;
	linebuf[i]='\0';

	while(feof(fd)==0)
	{
		ch=fgetc(fd);
		if (ch==0x0a)
			break;
		linebuf[i]=ch;
		i ++;
	}
	linebuf[i]='\0';
	i = trim_str(linebuf);
	return i;
}
#if 0
int add_file_cont(char *filename, char *siga, char *cont, int pos)
{
	FILE *fd_r, *fd_w;
	char *p, buf[200], ip_rule[30];
	int flag = 0;
	int s = -1;

	if(pos != DIR_BEFORE && pos != DIR_AFTER)
		goto ret;

	fd_r = fopen(filename,"r");
	if (fd_r == NULL)
	{
		goto ret;
	}

	fd_w = fopen(TMP_FILE,"w");
	if (fd_w == NULL)
	{
		goto ret;
	}

	while (1)
	{
		buf[0] = 0;
		p = fgets(buf, 200, fd_r);
		if (p == NULL)
			break;

		if(pos == DIR_AFTER)
			fwrite(buf, strlen(buf), 1, fd_w);

		if (strncasecmp(buf, siga, strlen(siga)) == 0)
		{
			fwrite(cont, strlen(cont), 1, fd_w);
			fwrite("\n", 1, 1, fd_w);
			s = 1;
		}

		if(pos == DIR_BEFORE)
			fwrite(buf, strlen(buf), 1, fd_w);
	}

	fclose (fd_r);
	fclose (fd_w);
	system("sync");
	//	system ("mkdir -p /doc/system/etc/rc.d/init.d; sync");
	sprintf(buf, "cp  %s %s 1> /dev/null 2> /dev/null; sync", TMP_FILE, filename);
	system(buf);

ret:
	return s;
}


int dele_file_cont( char *filename, char *cont)
{
	FILE *fd_r, *fd_w;
	char *p, buf[100], ip_rule[30];
	int flag = 0;
	int s = 1;

	fd_r = fopen(filename,"r");
	if  (fd_r == NULL)
	{
		//        printf_error("file  open error::");
		s = -1;
		goto ret;
	}
	fd_w = fopen(TMP_FILE,"w");
	if  (fd_w == NULL)
	{
		//        printf_error("file tmp open error::");
		s = -1;
		goto ret;
	}
	while (1)
	{
		p = fgets(buf, 100, fd_r);
		if (p == NULL)
			break;
		if (strncmp (buf, cont, strlen(cont)) != 0)
		{
			flag = 1;	
			fwrite(buf, strlen(buf), 1, fd_w);
		}
	}
	fclose (fd_r);
	fclose (fd_w);
	system("sync");

	if (flag)
	{	
		sprintf(buf, "cp %s %s 1>/dev/null; 2>/dev/null; sync", TMP_FILE, filename);
		system(buf);
	}
ret:
	return s;
}

#endif
