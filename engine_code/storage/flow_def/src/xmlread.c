#include "tcad.h"

extern char ip_addr_list_buf[IP_ADDR_LIST_BUF_LEN];
extern ip_info_t tmp_ip_data;

int get_add_flow(xmlNodePtr cur,char *tagtwo){
	xmlChar *uri;
	cur = cur->xmlChildrenNode;
	if (cur == NULL)
		return 0;
	char taskbuffer[64] = {0};
	char tmp_buf[BUF_SIZE]={0};
	flow_defined *todpbuf = NULL;
	flow_interval flow_time;
	incr_interval incr_time;		
	flow_restore  restore;
	udp_server_t udp_server;
	char *pudp=NULL;
	char *ptcp=NULL;
	int todpbuflen = 0;
	int sockfd = 0;
	tcad_retcode_t *retcode = NULL;

	sockfd = gms_socket_init();
	if (-1 == sockfd) {
		return -1;
	}

	todpbuflen = sizeof(flow_defined);

	todpbuf = (flow_defined *)malloc(todpbuflen);
	if (NULL == todpbuf) {
		return 0;
	}
	if(!strcmp((char *)tagtwo,(char *)"add_flow"))
	{
	
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagtwo))) { 
			memset(todpbuf, 0, todpbuflen);

			uri = xmlGetProp(cur, (xmlChar*)"switch");
		    if(!strcmp((char *)uri,"0"))	
			todpbuf->action = TCAD_ACTION_FLOW_ADD;
		    if(!strcmp((char *)uri,"1"))	
			todpbuf->action = TCAD_ACTION_FLOW_DEL;
			xmlFree(uri);
			uri = xmlGetProp(cur, (xmlChar*)"start_ip"); 
			todpbuf->start_ip = ntohl(inet_addr((char *)uri));
			xmlFree(uri);
			uri = xmlGetProp(cur, (xmlChar *)"end_ip"); 
			todpbuf->end_ip = ntohl(inet_addr((char *)uri));
			xmlFree(uri);
			printf("action=%d\n start_ip:%u \n end_ip:%u\n ",todpbuf->action,todpbuf->start_ip,todpbuf->end_ip);
			gms_send_rcv(sockfd, todpbuf, todpbuflen, taskbuffer, sizeof(taskbuffer));

			retcode = (tcad_retcode_t *)taskbuffer;

			if (TCAD_RETCODE_OK != retcode->retcode) {
				printf("send fail argin send ....\n");
			}

			usleep(5000);

		} 
		cur = cur->next; 
	}
	}
	else if(!strcmp((char *)tagtwo,(char *)"subflow"))
	{
		todpbuflen=sizeof(flow_restore);
	
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagtwo))) { 
			memset(&restore, 0, sizeof(flow_restore));
			memset(tmp_buf,0,BUF_SIZE);
			strncat(tmp_buf,"<START>\nNAME:",strlen("<START>\nNAME:")+1);

			uri = xmlGetProp(cur, (xmlChar*)"name");
			strncat(restore.name,(char *)uri,strlen((char*)uri)+1);
			strncat(tmp_buf,(char *)uri,strlen((char*)uri)+1);
			strncat(tmp_buf,"\n",2);
			xmlFree(uri);
			uri = xmlGetProp(cur, (xmlChar*)"udp");
			strncat(tmp_buf,"[UDP]\n",8);
			if(strcmp((char *)uri,""))
			{ 
			pudp=hextochs(uri);
			strncat(tmp_buf,pudp,strlen(pudp)+1);
			strncat(tmp_buf,"\n",2);
			if(pudp)
			free(pudp);
			}
			xmlFree(uri);
			uri = xmlGetProp(cur, (xmlChar *)"tcp");
			strncat(tmp_buf,"[TCP]\n",8);
			if(strcmp((char *)uri,""))
			{ 
			ptcp=hextochs(uri);
			strncat(tmp_buf,ptcp,strlen(ptcp)+1);
			strncat(tmp_buf,"\n",2);
			if(ptcp)
				free(ptcp);
			}
			xmlFree(uri);
			strncat(tmp_buf,"<END>\n",strlen("<END>\n")+1);
			if(strlen(tmp_buf)>=BUF_SIZE)
			{
				fprintf(stderr,"rev pack ge BUF_SIZE %d\n",BUF_SIZE);
				return 0;
			}
			printf("buf=%s\n",tmp_buf);
			restore.action=TCAD_ACTION_RESTORE_ADD;
			strncpy(restore.buf,tmp_buf,strlen(tmp_buf)+1);
			
			gms_send_rcv(sockfd, &restore, todpbuflen, taskbuffer, sizeof(taskbuffer));

			retcode = (tcad_retcode_t *)taskbuffer;

			if (TCAD_RETCODE_OK != retcode->retcode) {
				printf("send fail argin send ....\n");
			}

			usleep(5000);

		} 
		cur = cur->next; 
	}
	}
	else if(!strcmp((char *)tagtwo,(char *)"flow_time"))
	{
		todpbuflen=sizeof(flow_interval);
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagtwo))) { 
			memset(&flow_time, 0, todpbuflen);
			flow_time.action=TCAD_ACTION_SET_FLOW_INTERVAL;
			uri = xmlGetProp(cur, (xmlChar*)"delay"); 
			flow_time.itime = atoi((char *)uri);
			xmlFree(uri);
			//printf("flow_time=%d\n",flow_time.itime);
			if(flow_time.itime!=60)
			{
				printf("flow_time\n");
			gms_send_rcv(sockfd, (void *)&flow_time, todpbuflen, taskbuffer, sizeof(taskbuffer));

			retcode = (tcad_retcode_t *)taskbuffer;

			if (TCAD_RETCODE_OK != retcode->retcode) {
				printf("send fail argin send ....\n");
			}
			}
			usleep(5000);

		} 
		cur = cur->next; 
	}
	
	}
	else if(!strcmp((char *)tagtwo,(char *)"incr_time"))
	{
		todpbuflen=sizeof(incr_interval);
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagtwo))) { 
			memset(&incr_time, 0, todpbuflen);
			incr_time.action=TCAD_ACTION_SET_INCR_INTERVAL;
			uri = xmlGetProp(cur, (xmlChar*)"delay"); 
			incr_time.itime = atoi((char *)uri);
			xmlFree(uri);
			//printf("incr=%d\n",incr_time.itime);
			if(incr_time.itime!=1800)
			{
			printf("send_incr=%d\n",incr_time.itime);
			gms_send_rcv(sockfd,(char *)&incr_time, todpbuflen, taskbuffer, sizeof(taskbuffer));

			retcode = (tcad_retcode_t *)taskbuffer;

			if (TCAD_RETCODE_OK != retcode->retcode) {
				printf("send fail argin send ....\n");
			}
			}
			usleep(5000);

		} 
		cur = cur->next; 
	}
	
	}	
#if 1
	else if(!strcmp((char *)tagtwo,(char *)"syslog"))
	{
		todpbuflen=sizeof(udp_server_t);
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagtwo))) { 
			memset(&udp_server, 0, todpbuflen);
			udp_server.action=TCAD_ACTION_SET_UDP_SERVER_IP;
			uri = xmlGetProp(cur, (xmlChar*)"ip"); 
			if(strcmp((char *)uri,""))
			{
			udp_server.server_ip = ntohl(inet_addr((char *)uri));
			}
			xmlFree(uri);
			//printf("flow_time=%d\n",flow_time.itime);
			gms_send_rcv(sockfd, (void *)&udp_server, todpbuflen, taskbuffer, sizeof(taskbuffer));

			retcode = (tcad_retcode_t *)taskbuffer;

			if (TCAD_RETCODE_OK != retcode->retcode) {
				printf("send fail argin send ....\n");
			}

			usleep(5000);

		} 
		cur = cur->next; 
	}
	
	}
#endif
	if(todpbuf)
		free(todpbuf);
	close(sockfd);
	return 0;
}
void 
getReference (xmlDocPtr doc, xmlNodePtr cur,char*tagone,char*tagtwo) { 
	//xmlChar *uri; 
		//printf("tagone :tags=%s\n",tagone);
	cur = cur->xmlChildrenNode; 
	while (cur != NULL) { 
		if ((!xmlStrcmp(cur->name, (const xmlChar *)tagone))) { 
			/*uri = xmlGetProp(cur, "uri"); 
			printf("uri: %s\n", uri); 
			xmlFree(uri); */
			//printf("while ====%s",cur->name);
			break;
		} 
		cur = cur->next; 
	} 
	if(cur != NULL)
	get_add_flow(cur,tagtwo);
	return; 
} 
void parseDoc(char *docname,char *tagone,char *tagtwo) { 
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(docname); 
	if (doc == NULL ) { 
		fprintf(stderr,"Document not parsed successfully. \n"); 
		goto ERR; 
	} 
	cur = xmlDocGetRootElement(doc); 
	if (cur == NULL) { 
		fprintf(stderr,"empty document\n"); 
		xmlFreeDoc(doc); 
		doc=NULL;
		goto ERR; 
	}   
	if (xmlStrcmp(cur->name, (const xmlChar *) "gmsconf")) { 
		fprintf(stderr,"document of the wrong type, root node != gmsconf"); 
		xmlFreeDoc(doc); 
		doc=NULL;
		goto ERR; 
	} 
	printf("parseDoc\n");
	getReference(doc, cur,tagone,tagtwo); 
	xmlFreeDoc(doc); 
ERR:
	return; 
} 
int parseconf(char *path,char *act)
{
	ip_info_t *tmp_ip_data = NULL;
	tmp_ip_data = (ip_info_t *)malloc(sizeof(ip_info_t));
	char onedata_buf[100];
	/*列表中的一行解析后的指针数组*/
	char *ip_info_buf[7];
	int line_num = 0,i = 0;
	char *p;
	char *q;
	char *p_tmp;
//	char *r = ip_addr_list_buf;
	int len = 0;
	FILE * readstream = fopen(path,"r+");
	if(readstream == NULL){
		printf("open ip_location_list.conf error!\n");
		return -1;
	}
//	memset(ip_addr_list_buf,0,sizeof(ip_addr_list_buf));
	while(fgets(onedata_buf,100,readstream) != NULL){
		printf("onedata:%s\n",onedata_buf);
		//跳过注释
		if(onedata_buf[0]=='#') 
		{	
			memset(onedata_buf,0,sizeof(onedata_buf));
			continue;
		}
		line_num++;
		q = onedata_buf;
	//	ip_info_buf[0] = onedata_buf;
		for(i =0;i<7;i++){
			p = strchr(q,'|');
			ip_info_buf[i] = p+1;
			*p = '\0';
			q = p+1;
		}
		/*action*/
//		int action_int = TCAD_ACTION_IPINFO_ADD;
//		p_tmp = (char*)&action_int;
//		tmp_ip_data.action = *p_tmp;
		if(!strncmp(act,"add",3)){
			tmp_ip_data->action = TCAD_ACTION_IPINFO_ADD;
			printf("add!\n");
		}
		if(!strncmp(act,"del",3)){
			tmp_ip_data->action = TCAD_ACTION_IPINFO_DEL;
			printf("del!\n");
		}
		/*ip_start ip_end*/
		tmp_ip_data->ip_start = ntohl(inet_addr(ip_info_buf[0]));
		tmp_ip_data->ip_end = ntohl(inet_addr(ip_info_buf[1]));
		/*country*/
		int country_int = atoi(ip_info_buf[2]);
		p_tmp =  (char*)&country_int;
		tmp_ip_data->country = *p_tmp;
		/*res*/
/*		int res_int = atoi(ip_info_buf[3]);
		p_tmp = (char*)&res_int;
		tmp_ip_data->res = *p_tmp;*/
		/*isp_id*/
		tmp_ip_data->isp_id =(unsigned short)atoi(ip_info_buf[4]);
		/*province_id*/
		tmp_ip_data->province_id = (unsigned int)atoi(ip_info_buf[5]);
		/*city_id*/
		tmp_ip_data->city_id = (unsigned int)atoi(ip_info_buf[6]);
		
		for(i = 0;i<7;i++){
			printf("%s\n",ip_info_buf[i]);
		}
		memcpy(ip_addr_list_buf,tmp_ip_data,sizeof(ip_info_t));
//		printf("need to send ip_addr_list_buf :");
/*		for(i = 0;i<20;i++){
			printf("%c\n",ip_addr_list_buf[i]);
		}*/

		printf("\n-------------ip location start send-------------\n");
		gms_ip_location_send_tcad();
//		gms_send_rcv(sockfd,pflow_def,pflow_len,taskbuffer, sizeof(taskbuffer));

/*		memcpy(r,(char *)&tmp_ip_data,sizeof(ip_info_t));
		r+=sizeof(ip_info_t);
		*r = '\0';
		r++;
		len += 	sizeof(ip_info_t)+1;
		printf("r = 0x%x  ",r);*/

	}
	free(tmp_ip_data);
	fclose(readstream);
//	printf("ip_addr_list_buf is:%s",ip_addr_list_buf);
	return len;
}
#if 0
int 
main(int argc, char **argv) { 
	char *docname; 
	if (argc <= 1) { 
		printf("Usage: %s docname\n", argv[0]); 
		return 0;
	}
	docname = argv[1]; 
	parseDoc (docname); 
	return (1); 

}	
#endif
