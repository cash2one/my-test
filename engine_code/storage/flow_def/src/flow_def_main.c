#include "tcad.h"
char ret_buf[TCAD_RETCODE_FLOW_NOT_EXIST+2][20]={"sucess..","fail..","flow exist","flow not exist"};
char ip_addr_list_buf[IP_ADDR_LIST_BUF_LEN];
ip_info_t tmp_ip_data;

static int make_socket_non_blocking (int sfd)
{
	int flags, s;

	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}

int gms_socket_init()
{
    int sockfd;
	//int opt=1;
    //struct sockaddr_in addr;
    struct sockaddr_in server_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd == -1) {
        printf("Socket Error:%s\n", strerror(errno));
        return -1;
    }
#if 0
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(SERV_LOCAL);
    addr.sin_port = htons(GMS_SRV_PORT);
   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int)); 
    if (-1 == bind(sockfd, (struct sockaddr *)(&addr), sizeof(struct sockaddr))) {
        fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		close(sockfd);
        return -1;
    }
#endif	


    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCAD_SRV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) {
        printf("Connect error:%s\n",strerror(errno));
		close(sockfd);	
        return -1;
    }
    /* 设置 socket 属性为非阻塞方式 */
	/*if(fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl");
		exit(errno);
	}*/
	make_socket_non_blocking(sockfd);

    return sockfd;
}
int gms_send_rcv(int sockfd, void *data, int length, char *task_buf, int task_len)
{
    int ret = 0;
    char *strtmp = (char *)data;
    ssize_t reval, nleft = length;

	printf("socket_gms_send=%d\n",sockfd);
#if 1
    while (nleft > 0)
   	{
		printf("send***************************************************************************\n");
        reval = send(sockfd, strtmp, nleft, 0);
        //reval = write(sockfd, strtmp, nleft);
		//perror("send()");
        if (reval < 0) {
            if (errno == EINTR) {
                continue;
                printf("EINTR\n");
            }
            
            if (errno == EAGAIN) {
                sleep(1);
				printf("EAGAIN\n");
                continue;
            }

            return 1;
        }
        
        nleft -= reval;
        strtmp += reval;
    }
#endif
   printf("rev************************\n"); 
   ret = recv(sockfd, task_buf, task_len, 0);

    if (ret < 0) {
        return 1;
    }
   tcad_retcode_t *retnd=(tcad_retcode_t *)task_buf;
   printf("#######################ret %d %d len = %d\n",ret,retnd->retcode,task_len);

    return 0;
}
int gms_process(tcad_action_t *dp_action)
{
    if (NULL == dp_action) {
        return 0;
    }
#if 0
    if (TCAD_ACTION_BATCH == dp_action->action) {
       parseDoc(XML_FILE_PATH,"gms_def_flow","add_flow"); 
	}
#endif
	printf("gms_process\n");
	switch(dp_action->action)
	{
		case TCAD_ACTION_BATCH: parseDoc(XML_FILE_PATH,"gms_def_flow","add_flow");
								   parseDoc(XML_FILE_PATH,"gms_flow_time","flow_time");
								   parseDoc(XML_FILE_PATH,"gms_flow_time","incr_time"); 
								   parseDoc(XML_FILE_PATH,"gms_flow_restore","subflow"); 
								   //parseDoc(XML_FILE_PATH,"gms_warn","syslog"); 
								   break;
		case TCAD_ACTION_FLOW_ADD:parseDoc(XML_FILE_PATH,"gms_def_flow","add_flow");break;
		case TCAD_ACTION_FLOW_DEL:parseDoc(XML_FILE_PATH,"gms_def_flow","add_flow");break;
		case TCAD_ACTION_SET_FLOW_INTERVAL:parseDoc(XML_FILE_PATH,"gms_flow_time","flow_time");
		case TCAD_ACTION_SET_INCR_INTERVAL:parseDoc(XML_FILE_PATH,"gms_flow_time","incr_time");
		case TCAD_ACTION_RESTORE_ADD:parseDoc(XML_FILE_PATH,"gms_flow_restore","subflow");
		case TCAD_ACTION_IPINFO_ADD:parseconf(IP_LOCATION_CONF_FILE_PATH,"add");break;
		case TCAD_ACTION_IPINFO_DEL:parseconf(IP_LOCATION_CONF_FILE_PATH,"del");break;
		default:printf("rev values not found\n");break;
		
	}
    return 0;
}

void *gms_recv(void *arg)
{
    int sockfd = 0, new_fd = 0, opt = 1;
    char *buffer = NULL;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int ret = 0, iDataNum = 0;
    socklen_t sin_size = 0;

#define BUF_MAX_LEN     16384

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
        return NULL;
    }

    buffer = (char *)malloc(BUF_MAX_LEN);
    if (NULL == buffer) {
        goto err;
    }
    
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(SERV_LOCAL);
    server_addr.sin_port = htons(GMS_SRV_PORT);
    
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(int));
    
    if (-1 == bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))) {
        fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
        goto err;
    }

    if (listen(sockfd, 5) == -1) {
        fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
        goto err;
    }

    while (1) {
        memset(buffer, 0, BUF_MAX_LEN);
        printf("Waiting...\n");
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) == -1) {
            fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
            continue;
        }

        printf("Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));
        iDataNum = recv(new_fd, buffer, BUF_MAX_LEN - 1, 0);
        if (iDataNum < 0) {
            close(new_fd);
            continue;
        }
        
        printf("Recv data len: %d\n", iDataNum);

        ret = gms_process((tcad_action_t *)buffer);
        
        //send(new_fd, &ret, sizeof(int), 0);
        close(new_fd);
    }
    
err:
    if (buffer) {
        free(buffer);
    }
    
    close(sockfd);
    
    return NULL;
}

void gms_recv_thread_start(pthread_t *tid_gms_recv)
{
    int err;
    printf("gms_recv_thread_start!\n");
    
    err = pthread_create(tid_gms_recv, NULL, gms_recv, NULL);
    if (0 != err) {
        printf("can not create gms recv thread\n");
    }
}
int gms_send_tcad(param_struct*param_val )
{
	char taskbuffer[64]={0};
	int sockfd=0;
	flow_defined *pflow_def=NULL;
	int pflow_len=0;
    tcad_retcode_t *retcode = NULL;
	sockfd=gms_socket_init();
	if(sockfd==-1)
	{
		printf("create socket fail ...\n");
		return -1;
	}
	printf("socket=%d\n",sockfd);
	pflow_len=sizeof(flow_defined);
	pflow_def=(flow_defined*)malloc(sizeof(flow_defined));
	if(NULL ==pflow_def)
	{
		printf("malloc fail ..\n");
		return -1;
	}
	memset(pflow_def,0,pflow_len);
	memset(taskbuffer,0,sizeof(taskbuffer));
	pflow_def->action    = param_val->flag;
	pflow_def->start_ip  = ntohl(inet_addr(param_val->start_ip));
	pflow_def->end_ip = ntohl(inet_addr(param_val->end_ip));
	printf("action=%d\n",pflow_def->action);
	printf("start_ip=%u\n",pflow_def->start_ip);
	printf("end_ip=%u\n",pflow_def->end_ip);
	//getchar();
	gms_send_rcv(sockfd,pflow_def,pflow_len,taskbuffer, sizeof(taskbuffer));
	printf("send *************************end\n");
	retcode = (tcad_retcode_t *)taskbuffer;
	switch(retcode->retcode)
	{
		case TCAD_RETCODE_OK:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FAILED:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_NOT_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		default:printf("ret=error\n");break;
	}
	if(pflow_def)
	{
		free(pflow_def);
	}
	close(sockfd);
	return 0;
}
int gmsflow_syslog_send_tcad(udp_server_t *param_val )
{
	char taskbuffer[64]={0};
	int sockfd=0;
	udp_server_t *pflow_def=NULL;
	int pflow_len=0;
    tcad_retcode_t *retcode = NULL;
	sockfd=gms_socket_init();
	if(sockfd==-1)
	{
		printf("create socket fail ...\n");
		return -1;
	}
	printf("socket=%d\n",sockfd);
	pflow_len=sizeof(udp_server_t);
	pflow_def=(udp_server_t*)malloc(sizeof(udp_server_t));
	if(NULL ==pflow_def)
	{
		printf("malloc fail ..\n");
		return -1;
	}
	memset(pflow_def,0,pflow_len);
	*pflow_def=*param_val;
	memset(taskbuffer,0,sizeof(taskbuffer));
	gms_send_rcv(sockfd,pflow_def,pflow_len,taskbuffer, sizeof(taskbuffer));
	printf("send *************************end\n");
	retcode = (tcad_retcode_t *)taskbuffer;
	switch(retcode->retcode)
	{
		case TCAD_RETCODE_OK:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FAILED:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_NOT_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		default:printf("ret=error\n");break;
	}
	if(pflow_def)
	{
		free(pflow_def);
	}
	close(sockfd);
	return 0;
}
int gmsflow_restore_send_tcad(restore_char *param_val )
{
	char  taskbuffer[64]={0};
	char *pudp=NULL;
	char *ptcp=NULL;
	int sockfd=0;
	flow_restore *pflow_res=NULL;
	int pres_len=0;
    tcad_retcode_t *retcode = NULL;
	pres_len=sizeof(flow_restore);
	pflow_res=(flow_restore*)malloc(sizeof(flow_restore));
	if(NULL == pflow_res)
		return -1;
	memset(pflow_res,0,pres_len);
	memset(taskbuffer,0,sizeof(taskbuffer));

	pflow_res->action    = param_val->action;
	strncat(pflow_res->name,param_val->name,strlen(param_val->name)+1);
	strncat(pflow_res->buf,"<START>\nNAME:",strlen("<START>\nNAME:")+1);
	strncat(pflow_res->buf,param_val->name,strlen(param_val->name)+1);
	strncat(pflow_res->buf,"\n",2);
	strncat(pflow_res->buf,"[UDP]\n",strlen("[UDP]\n")+1);
	if(strcmp(param_val->udp,"")){
		pudp=hextochs(param_val->udp);
		strncat(pflow_res->buf,pudp,strlen(pudp)+1);
		strncat(pflow_res->buf,"\n",2);
		if(pudp)
		free(pudp);
	}
		strncat(pflow_res->buf,"[TCP]\n",strlen("[TCP]\n")+1);
	if(strcmp(param_val->tcp,"")){
		ptcp=hextochs(param_val->tcp);
		strncat(pflow_res->buf,ptcp,strlen(ptcp)+1);
		strncat(pflow_res->buf,"\n",2);
		if(ptcp)
		free(ptcp);
	}
	strncat(pflow_res->buf,"<END>\n",strlen("<END>\n")+1);
	printf("restore_flow_buf=%s\n",pflow_res->buf);
#if 1
	sockfd=gms_socket_init();
	if(sockfd==-1)
	{
		printf("create socket fail ...\n");
		return -1;
	}
	gms_send_rcv(sockfd,pflow_res,pres_len,taskbuffer, sizeof(taskbuffer));

	retcode = (tcad_retcode_t *)taskbuffer;
	switch(retcode->retcode)
	{
		case TCAD_RETCODE_OK:printf("ret=%u %s\n",retcode->retcode,ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FAILED:printf("ret= %u %s\n",retcode->retcode,ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_EXIST:printf("ret=%u %s\n",retcode->retcode,ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_NOT_EXIST:printf("ret=%u %s\n",retcode->retcode,ret_buf[retcode->retcode]);break; 
		default:printf("ret=error\n");break;
	}
#endif
	if(pflow_res)
	{
		free(pflow_res);
	}
	close(sockfd);
	return 0;
}
int gms_ip_location_send_tcad()
{
	char taskbuffer[64]={0};
	int sockfd=0;
	tcad_retcode_t *retcode = NULL;
	/*存放ip localtion list的全局buffer*/
	int len;
//	char *p = strchr(ip_addr_list_buf,'#');
	sockfd=gms_socket_init();
	if(sockfd==-1)
	{
		printf("create socket fail ...\n");
		return -1;
	}
	printf("socket=%d\n",sockfd);

//	len = parseconf(IP_LOCATION_CONF_FILE_PATH);
	len = sizeof(ip_info_t);
	memset(taskbuffer,0,sizeof(taskbuffer));
	#if 0
	ip_info_t *r = (ip_info_t *)ip_addr_list_buf;
	printf("start send strunct ip_info_t:action = %c,ip_start = %u,ip_end = %u,province_id = %u\n",r->action,r->ip_start,r->ip_end,r->province_id);
	#endif

	gms_send_rcv(sockfd,ip_addr_list_buf,len,taskbuffer, sizeof(taskbuffer));
//	memset(ip_addr_list_buf,0,IP_ADDR_LIST_BUF_LEN);
	printf("send *************************end\n");
	retcode = (tcad_retcode_t *)taskbuffer;
	switch(retcode->retcode)
	{
		case TCAD_RETCODE_OK:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FAILED:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		case TCAD_RETCODE_FLOW_NOT_EXIST:printf("ret= %s\n",ret_buf[retcode->retcode]);break; 
		default:printf("ret=error\n");break;
	}
/*	if(pflow_def)
	{
		free(pflow_def);
	}*/
	close(sockfd);
	return 0;
}
void gms_send_mess(int argc,char **argv)
{
	char act;
	char ch;
	int ret=0;
	udp_server_t udp_server;
	param_struct param_val;
	restore_char restore_tmp;
	memset(&param_val,0,sizeof(param_struct));
	memset(&udp_server,0,sizeof(udp_server_t));
	memset(&restore_tmp,0,sizeof(restore_char));
	while(1)
	{
		printf("getopt\n");
	ch = getopt(argc, argv, "f:s:e:c:n:t:u:p:i:");
	if ( ch == -1 ) {
		break;
	}
	 //printf("ch=%c(%d) optopt=%c optind=%d optarg=%s\n",ch,ch,  optopt, optind, optarg);
	 switch(ch)
	 {
	 	case 'f':
				if(!strcmp(optarg,"0")){
					param_val.flag=TCAD_ACTION_FLOW_ADD;
				}
				else if(!strcmp(optarg,"1")){
					param_val.flag=TCAD_ACTION_FLOW_DEL;
				}
				ret=1;
				break;
	 	case 's':strcpy(param_val.start_ip,optarg);break;
	 	case 'e':strcpy(param_val.end_ip,optarg);break;
		case 'c':if(!strcmp(optarg,"1")){
					restore_tmp.action=TCAD_ACTION_RESTORE_ADD;	 	
				 }
				else if(!strcmp(optarg,"2")){
					restore_tmp.action=TCAD_ACTION_RESTORE_DEL;	 	
				
				}
				else if(!strcmp(optarg,"3")){
					restore_tmp.action=TCAD_ACTION_RESTORE_MOD;	 	
				
				}
				ret=2;
				break;
		case 'n':strncpy(restore_tmp.name,optarg,BUF_SIZE);break;
		case 't':strncpy(restore_tmp.tcp,optarg,BUF_SIZE);break;
		case 'u':strncpy(restore_tmp.udp,optarg,BUF_SIZE);break;
		case 'p':udp_server.action=TCAD_ACTION_SET_UDP_SERVER_IP;
				 udp_server.server_ip=ntohl(inet_addr(optarg));
				 ret=3;
				 break;
		case 'i':if(!strcmp(optarg,"1")){
				 tmp_ip_data.action = TCAD_ACTION_IPINFO_ADD;
				}
				else if(!strcmp(optarg,"0")){
				 tmp_ip_data.action = TCAD_ACTION_IPINFO_DEL;
				}
				 ret=4;
				 break;
		default:break;
	 }
	}
	//printf("param=%c\n",param_val.flag);
	if(ret==1)
		gms_send_tcad(&param_val);
	else if(ret==2)
		gmsflow_restore_send_tcad(&restore_tmp);
	else if(ret==3)
		gmsflow_syslog_send_tcad(&udp_server);
	else if(ret==4){
		if (tmp_ip_data.action == TCAD_ACTION_IPINFO_ADD)
		parseconf(IP_LOCATION_CONF_FILE_PATH,"add");
		if (tmp_ip_data.action == TCAD_ACTION_IPINFO_DEL)
		parseconf(IP_LOCATION_CONF_FILE_PATH,"del");
	}
	return;
}
int main(int argc, char *argv[])
{
	int retn=0;
	tcad_action_t resfault;
	pthread_t tid_gms_recv;
    printf("GMS!\n");
	if(argc==1)
	{
		resfault.action=TCAD_ACTION_BATCH;
		gms_process(&resfault);
		gms_recv_thread_start(&tid_gms_recv);
		while(1)
		{
			retn = pthread_tryjoin_np(tid_gms_recv, (void **)NULL);
			if(retn==0)
			{
				gms_recv_thread_start(&tid_gms_recv);
			}
		}
	}
	else
    gms_send_mess(argc,argv);

    return 0;
}
