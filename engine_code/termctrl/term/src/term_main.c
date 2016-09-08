#include "term_main.h"

#define NIDS_PORT	0
#define AUDIT_PORT	1
#define IDMEF_PORT	2
int acc(char*);
int raw_mode = 0;

extern char s_upret[1000];
int load_conf(char *file)
{
	char *p;
	//char s;
	char buf[100];
	FILE *fd;
	int ret, i=0, eth, pos;

	ret = 0;
	fd = fopen(file,"r");
	if (fd == NULL)
{
    perror("fopen()");
		return ret;
}
	bzero (eth_pos, sizeof(eth_pos));

	while (i<total_ethernet_number)
	{
		bzero(buf,100);
		p = fgets(buf,100,fd);
		if ( p==NULL )
			break;	
		if (!strncmp(buf, "neteth", strlen("neteth")))
		{	
			ret = 1;
			buf[strlen(buf)]='\0';
			eth = atoi(buf+7);	
			if (eth < 0 || eth>=total_ethernet_number)
				continue;	
		}
		//var = rentry.re_varlist;
		//for (i=0; i<rentry.re_var_num; i++)	
		{
			if (!strncmp(buf, "netpostion", strlen("netpostion")))	
			{
				
                pos=atoi(buf+13);
				if (pos <= 0 || pos > MAX_ETH)
					break;
				eth_pos[eth] = pos;
				i++;
			}		
			if (!strncmp(buf, "netstat", strlen("netstat")))	
			{
				if ( !strncmp((buf+10), "unlink", 6) )	
					pgb_var[0].eth_have_ip[eth] =  TYPE_CAP;
				else if ( !strncmp((buf+10), "link", 4) )	
					pgb_var[0].eth_have_ip[eth] =  TYPE_COM;
			}		
		}
	}
	fclose(fd);
	return ret;
}
#define OP_SET_IPMASK     2   
#define OP_CLEAR_PASSWD   5   
#define OP_SHOW_CONF      1   
#define OP_SET_INTERFACE  4
#define OP_EXIT_PROG      9  
#define OP_INIT_KEY       8  
#define OP_RESTORE_PARAM  7
#define OP_SET_SSH        6
#define OP_SET_ETH        3 


typedef struct com_menu_t{
	int	id;
	char*	text;
}com_menu_t;
void addspace(char *tmp)
{
	int i;
	char *s=tmp;
	//printf("len=%d\n",strlen(tmp));
	if(strlen(tmp)<50)
	{
		for(i=strlen(tmp);i<50;i++)
		{
			s[i]=' ';
		}
		s[i]='*';
		s[i+1]='\0';
	}
}
void show_menu()
{
	unsigned int index;
	char text[256] = {0};

	com_menu_t menu[] = {
		{-1, " 功能选项"},
		{OP_SHOW_CONF, " 设备信息展示"},
		{OP_SET_IPMASK, " 设置ip和端口"},
		{OP_SET_ETH, " 选择监测口"},
		{OP_SET_INTERFACE, " 设置ftp服务"},
		{OP_CLEAR_PASSWD, " 开启直连网段"},
		{OP_SET_SSH, " 设置ssh服务"},
		{OP_RESTORE_PARAM, " 恢复出厂设置"},
		{OP_INIT_KEY, " 重置界面admin密码"},
		{OP_EXIT_PROG, "  退    出"}
	};
	SendOneLine("\n请选择一项:");
	SendOneLine("\n(<h>：显示操作说明；<q>：取消当前操作");
	for(index = 0; index < sizeof(menu) / sizeof(com_menu_t); index++) {
		if(menu[index].id != -1) {
			snprintf(text, sizeof(text), "    %d.	%s", menu[index].id, menu[index].text);
		}else {
			snprintf(text, sizeof(text), "  %s:", menu[index].text);
		}
		SendOneLine(text);
	}


}

void show_menu_help()
{	
	unsigned int index;
	char text[256] = {0};

	com_menu_t menu[] = {
		{-1, " 功能选项"},
		{OP_SHOW_CONF, "显示当前配置信息，包括IP地址、端口、子网掩码、uuid等信息"},
		{OP_SET_IPMASK, "更改网口的ip、子网掩码和端口号"},
		{OP_SET_ETH, "选择使用哪一个口监测"},
		{OP_SET_INTERFACE, "是否开启ftp接口"},
		{OP_CLEAR_PASSWD, "是否开启直连网段"},
		{OP_SET_SSH, "是否开启ssh服务"},
		{OP_RESTORE_PARAM, "恢复设备的出厂参数，包括通讯网口的IP地址、子网掩码、UUID等信息"},
		{OP_INIT_KEY, "如果用户忘记界面管理员admin的密码，可以通过这个选项重置密码"},
		{OP_EXIT_PROG, "退出串口配置程序；如果不退出，用户可以通过串口无需登录而直接操作串口配置程序"}
	};

	SendOneLine("\n请选择一项：(<h>：显示帮助信息；<q>：取消当前操");


	for(index = 0; index < sizeof(menu) / sizeof(com_menu_t); index++) {
		if(menu[index].id != -1) {
			snprintf(text, sizeof(text), "    %d.	%s", menu[index].id, menu[index].text);
		}else {
			snprintf(text, sizeof(text), "  %s:", menu[index].text);
		}
		SendOneLine(text);
	}

}
int get_fileinformation(char *filename,char *temp_file)
{
	FILE *serial_fd;
	int rdsize;
	char  cstrnumber[1000];
			serial_fd=fopen(filename,"r");
			if(serial_fd==NULL)
			{
				sprintf(cstrnumber, "fopen():%s", strerror(errno));

				SendOneLine(cstrnumber);
				return -1;
			}
			rdsize=fread(temp_file,127,1,serial_fd);
			if(rdsize<0)
			{
				sprintf(cstrnumber,"fread():%s",strerror(errno));
				SendOneLine(cstrnumber);
				return -1;
			}
	
}
int get_machine_type(char *filename)
{
    int fd;
    int ret;
   char num;
    fd=open(filename,O_RDONLY);
    if(fd<=0)
    {
        SendOneLine("machine type file not exit");
        return 0;
    }
    ret=read(fd,&num,1);
        return atoi(&num);
}
int choose_eth()
{
    char temp[128],tp[128];
    int i,res,tret,ethtmp,j,rettmp;
    for(i=manger_ethernet_number;i<total_ethernet_number;i++)
    {
        bzero(temp,128);
        if(pgb_var->eth_have_ip[i] == TYPE_CAP)
        {
            sprintf(temp,"监测口%d: ",eth_pos[i]-manger_ethernet_number);
            SendString(temp);
        }
        if(((i+1-manger_ethernet_number)%4) == 0)
            SendOneLine("\n");
    }
    bzero(temp,128);
    sprintf(temp,"共有%d个网口，请输入需要监测口的数量(1-%d):",total_ethernet_number-manger_ethernet_number,total_ethernet_number-manger_ethernet_number);
        SendString(temp);
    /***********************************/
    bzero(tp, 128 * sizeof(char));
    if(ReceiveOneLine(tp)<0) 
        return -1;
    tret=trim_str(tp);
    res = atoi(tp);
    if(res<=0||res>(total_ethernet_number-manger_ethernet_number))
        return 0;
    bzero(temp,128);
    sprintf(temp,"请选择%d个监测口",res);
    SendOneLine(temp);
    for(i=1;i<=res;i++)
    {
        bzero(temp,128);
        sprintf(temp,"请选择第%d个监测口:",i);
        SendOneLine(temp);
        bzero(tp, 128 * sizeof(char));
        if(ReceiveOneLine(tp)<0) 
            return -1;
        tret=trim_str(tp);
        ethtmp = atoi(tp)+manger_ethernet_number-1;

        if(ethtmp<manger_ethernet_number||ethtmp>total_ethernet_number)
        {
            SendOneLine("输入的监测口不存在请重新输入");
            i--;
            continue;
        }
        for(j=manger_ethernet_number;j<=total_ethernet_number;j++)
        {
            if(eth_pos[j]==ethtmp+1)
                break;
        }
#if 0
        if(manger_ethernet_number==1)
        {
            switch (j)
            {
                case 5:j=7;break;
                case 6:j=8;break;
                case 7:j=5;break;
                case 8:j=6;break;
                default:break;
            }
        }
#endif
		rettmp=i;
        bzero(temp,128);
        if(i==1)
        {
    		bzero(temp,128);
            sprintf(temp,"sed -i '/dev_name/d' %s ;sed -i '1i dev_name = eth%d' %s",MTXPAG,j,MTXPAG);
            system(temp);
   			 bzero(temp,128);
            sprintf(temp,"sed -i '/dev_name/d' %s ;sed -i '1i dev_name = eth%d' %s ",APTPAG,j,APTPAG);
            system(temp);
    		bzero(temp,128);
            sprintf(temp,"sed -i '/dev_name/d' %s ;sed -i '1i dev_name = eth%d' %s",VDSPAG,j,VDSPAG);
            system(temp);
        }
        else
        {
    	bzero(temp,128);
        sprintf(temp,"sed -i '1i dev_name = eth%d' %s",j,MTXPAG);
        system(temp);
    	bzero(temp,128);
        sprintf(temp,"sed -i '1i dev_name = eth%d' %s ",j,APTPAG);
        system(temp);
    	bzero(temp,128);
        sprintf(temp,"sed -i '1i dev_name = eth%d' %s",j,VDSPAG);
        system(temp);
        }
		i=rettmp;
	}

    SendOneLine("选择监测口完成");
    return 1;
}
int Auto_ret(char *filename)
{
	int ret;
	ret=(int)(system(AUTH_CMD)/256);
	if(ret==16)
		return 1;
	else
		return 0;

}
#if 0
int Auto_ret(char *filename)
{
    FILE *fd;
    char *p=NULL;
    char buf[50];
    int i;
    fd=fopen(filename,"r");
    if(fd==NULL)
    {
        return -1;
    }
    for(i=0;i<4;i++)
    {
    bzero(buf,50);
    p=fgets(buf,50,fd);
    }
    for(i=0;buf[i]!=44;i++)
            ;
    if(!strncmp(buf+i,"SUCCESS",7))
        return 1;
    else
        return -1;
    
}
#endif
int get_dev_id(char *src,char *dst)
{
	FILE *fp;
	memset(dst,0,20);
	fp=popen("python /gms/gapi/modules/auth/genkey.pyc","r");
	fgets(dst,20,fp);
	pclose(fp);
	

	return 0;
}
void set_env()
{
	setenv("PYTHONHOME","/usr",1);
	//setenv("PYTHONPATH",".:/usr:/usr/lib64/python2.6:/usr/lib64/python2.6/site-packages:/usr/lib64/python2.6/lib-dynload:/usr/lib/python2.6/site-packages",1);
	setenv("PYTHONPATH",".:/usr:/usr/lib64/python2.6:/usr/lib64/python2.6/site-packages:/usr/lib64/python2.6/lib-dynload:/usr/lib/python2.6/site-packages:/usr/local/lib/python2.6:/usr/local/lib/python2.6/site-packages:/usr/local/lib/python2.6/lib-dynload",1);
	setenv("PATH","/gms/jdk1.6/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr:.:/usr:/usr/lib64/python2.6:/usr/lib64/python2.6/site-packages:/usr/lib64/python2.6/lib-dynload:/usr/lib/python2.6/site-packages/lib64/python2.6",1);
	
}
int main()
{
	char ip[100];
	char  tp[500];
	char netmask[200];
	char cstrip[1000];
	char cstrmask[1000];
	char cstrway[50];
	char cstrdns[50];
	char  cstraccredit[1000];
	char  cstrnumber[1000];
	char  fileport[1000];
	char  comdport[1000];
	char temp[128];
	char temp_file[128];
	//char key_cmd[100];
	int  i;
	int ret,ret1;
	int res;
	//int userloginflag; 
	int tret;
	int retkey;
	//int tty;  
	int ret_len;	
	//char pattern[128]={0};
	//glob_t globres;
    system("stty erase ^H");
	Nport typort;
	set_env();
#if 0			  
	tty = ini_tty(COM1,9600);//在Venus_ini()中设置ttys0,波特率9600，数据位8，不设置奇偶校验位
	if(tty<0)
		return -1; 

	userloginflag=userlogin();
	if(userloginflag<0)
		exit(-1);
#endif
	setechoflag();
	print_welcome();
	system("clear");

	bzero(ip,128 * sizeof(char));
	bzero(netmask,128 * sizeof(char));

	total_ethernet_number=get_ethernet_number();  //Get max ethernet card number
    //printf("ethnumber=%d\n",total_ethernet_number);
    ret = get_machine_type(MACHINE_TYPE);
    if(ret==2)
	manger_ethernet_number=2;//自己预设值
    else
	manger_ethernet_number=1;//自己预设值
    
	 pgb_var =(struct global_var * ) &gb_var;
	if (!load_conf("/gms/termctrl/conf/netsetfile.conf"))
	{
    SendOneLine("netsetfile.conf not open\n"); 
		exit(1);
	}
    if(manger_ethernet_number==1)
    {
        pgb_var-> eth_have_ip[1]=TYPE_CAP;
    }

	//ret = read_system_msg(); 
	if (total_ethernet_number<=0)
	{
		exit (1);
	}
setuid(0);

main_menu_label:
	while(1)
	{
		show_menu();
		SendString(VENUS_PROMPT);
		bzero(tp, 128 * sizeof(char));
		tret=ReceiveOneLine(tp);
		if (tret <= 0)
		{
			system("clear");
			goto main_menu_label;
		}
		tret=trim_str(tp);

		ret = 0;
		if (tret==0) 
			goto main_menu_label;

		if(tp[0] == 'h' || tp[0] == 'H')
		{
			show_menu_help();
			SendOneLine("按回车键退出...");
			bzero(tp, 128 * sizeof(char));
			if ( ReceiveOneLine(tp)<0)
				goto main_menu_label;
		}


		ret = atoi(tp);

		if (ret == OP_SET_IPMASK)
		{
ip_menu_label:
			system("clear");
			SendOneLine("  请选择一项进行设置:");
			SendOneLine("\t(1)IP/子网掩码设置");
			SendOneLine("\t(2)端口号设置");
			SendString("chanct:");

			tret=ReceiveOneLine(tp);
			if (tret <= 0)
				goto main_menu_label;
			tret=trim_str(tp);

			ret = 0;
			if (tret==0) 
				goto ip_menu_label;

			ret = atoi(tp);

			/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
			if(ret==1)
			{
                system("clear");
                bzero(gb_ip_temp,16);
                bzero(gb_netmask_temp,16);
                SendOneLine("以下显示的网卡可以被修改:");
                SendOneLine("\0");	
                if(manger_ethernet_number==1)
                {
                    for(i=0;i<manger_ethernet_number;i++)
                    {
                        bzero(temp,128);
                        if(pgb_var->eth_have_ip[i] == TYPE_COM)
                        {
                            ip[0] = 0;	
                            get_ethernet_msg(i,ip,netmask);
                            if (ip[0])
                            {
                                sprintf(temp,"%d 管理口:  %s/%s ", eth_pos[i],  ip, netmask);}
                            else if ( !ip[0] )
                                sprintf(temp,"%d 管理口%d: no/no ",eth_pos[i]);
                            SendOneLine(temp);
                        }
                    }
                }
                    else
                    {
                        for(i=0;i<manger_ethernet_number;i++)
                        {
                            bzero(temp,128);
                            if(i==0)
                            {	
                                if(pgb_var->eth_have_ip[i] == TYPE_COM)
                                {
                                    ip[0] = 0;	
                                    get_ethernet_msg(i,ip,netmask);
                                    if (ip[0])
                                    {
                                        sprintf(temp,"%d 备用口:  %s/%s ", eth_pos[i],  ip, netmask);}
                                    else if ( !ip[0] )
                                        sprintf(temp,"%d 备用口: no/no ",eth_pos[i]);
                                    SendOneLine(temp);
                                }
                            }

                            else
                            {	
                                if(pgb_var->eth_have_ip[i] == TYPE_COM)
                                {
                                    ip[0] = 0;	
                                    get_ethernet_msg(i,ip,netmask);
                                    if (ip[0])
                                    {
                                        sprintf(temp,"%d 管理口:  %s/%s ", eth_pos[i],  ip, netmask);}
                                    else if ( !ip[0] )
                                        sprintf(temp,"%d 管理口: no/no ",eth_pos[i]);
                                    SendOneLine(temp);
                                }
                            }

                        }
                    }
				SendString("请在以上网口中选择一个网口,<q>取消:");
input2:
				bzero(tp, 128 * sizeof(char));
				if(ReceiveOneLine(tp)<0) 
					goto main_menu_label;
				tret=trim_str(tp);
				res = atoi(tp);

				// 按pos查找ethi 
				for (i=0; i<manger_ethernet_number; i++)
				{
					//if (!eth_pos[i])	
					//	break;	
					if (eth_pos[i]==res)	
						break;	
				}
				res = i;

				if (!isdigit(tp[0]) || tret<0 || res>=manger_ethernet_number || pgb_var->eth_have_ip[res] != TYPE_COM)
				{
					SendOneLine("当前输入无效，请输入正确的网口");	
					goto input2;
				}
				else
				{
					SendString("请输入ip地址:");
                    bzero(tp, 128 * sizeof(char));
                    ret_len = ReceiveOneLine(tp);
                    if(ret_len<=0)
                        goto main_menu_label;
                    trim_str(tp);
                    strncpy(gb_ip_temp,tp,16);

                    SendString("请输入子网掩码:");
                    bzero(tp,128 * sizeof(char));
                    ret_len = ReceiveOneLine(tp);
                    if(ret_len<=0)
                        goto main_menu_label;
                    trim_str(tp);
                    strncpy(gb_netmask_temp,tp,16);

                    SendString("请输入默认网关:");
                    bzero(tp,128 * sizeof(char));
                    ret_len = ReceiveOneLine(tp);
                    if(ret_len<=0)
                        goto main_menu_label;
                    trim_str(tp);
                    strncpy(gb_gateway_temp,tp,16);

                    SendString("请输入DNS  :");
                    bzero(tp,128 * sizeof(char));
                    ret_len = ReceiveOneLine(tp);
                    if(ret_len<=0)
                        goto main_menu_label;
                    trim_str(tp);
                    strncpy(gb_dns,tp,16);
                    which=res;
                }	
                if ( IsValidIpaddr(gb_ip_temp) && IsValidIpaddr(gb_netmask_temp)&&IsValidIpaddr(gb_gateway_temp)&&IsValidIpaddr(gb_dns))
                {
                    SendOneLine("Ip地址/子网掩码将被设置成以下值:");
                    sprintf(cstrip,"IP 地址  :%s",gb_ip_temp);
                    SendOneLine(cstrip);
                    sprintf(cstrmask,"子网掩码  :%s",gb_netmask_temp);
                    SendOneLine(cstrmask);
                    sprintf(cstrway,"默认网关  :%s",gb_gateway_temp);
                    SendOneLine(cstrway);
                    sprintf(cstrdns,"  DNS  :%s",gb_dns);
                    SendOneLine(cstrdns);

                    SendOneLine("你同意修改吗?");
                    SendOneLine("	(1)==是");
                    SendOneLine("	(2)==否");
                    SendString("chanct:");

                    bzero(tp, 128 * sizeof(char));
                    if(ReceiveOneLine(tp)<0) goto main_menu_label;
                    tret=trim_str(tp);
                    res=0;
                    if (tret>0)
                        res=atoi(tp);
                    if (res==1)
                    {
                        if(IPCHANGED==0)
                            SendOneLine("输入的IP或子网掩码是无效的");
                        else
                        {
                            system("/bin/sh /sbin/service network restart ");
                            SendOneLine("IP地址/子网掩码设置成功");
                            //unlink(IP_TMP);					
                            }	
                    }
                    else
                        SendOneLine("操作被取消");
                }
                else
                    SendOneLine("输入无效");
            }
            else if(ret==2)
            {
#if 1
                system("clear");
                if(set_port()<0)
                {
                    unlink(TMP_NETPORT);
                    SendOneLine("操作被取消");
                    goto main_menu_label;
                }
                unlink(TMP_NETPORT);

#endif
            }
            SendString("按回车键继续...");
            ReceiveOneLine(tp);
            system("clear");

        }
        else if(ret == OP_SET_ETH)
        {
            system("clear");
            int eth_flag;
ethernet_flag:
            eth_flag=choose_eth();
            if(eth_flag<0)
                goto main_menu_label;
            else if(eth_flag==0)
                goto ethernet_flag;
            else
                SendOneLine("设备需要重启才能完成打包口设置");
            SendString("按回车键继续...");
            ReceiveOneLine(tp);
                system("reboot");
            system("clear");
        }

#if 1

		else if(ret == OP_SHOW_CONF)
		{
            system("clear");
            bzero(gb_ip_temp,16);
            bzero(gb_netmask_temp,16);
            SendOneLine("以下为设备的基本信息:");
            SendOneLine("\0");
			
            SendOneLine("**************************************************");
            for(i=0;i<manger_ethernet_number;i++)
            {
                bzero(cstrip,200);
                bzero(cstrmask,200);
                bzero(temp,128);	
                if(manger_ethernet_number==1)
                {
                    if(pgb_var->eth_have_ip[i] == TYPE_COM)
                    {
                        ip[0] = 0;	
                        get_ethernet_msg(i,ip,netmask);
                        if (ip[0])
                        {
                            sprintf(cstrip,"*  管理口   ip: %s ",  ip);
                            SendOneLine(cstrip);
                            sprintf(cstrmask,"*  子网掩码: %s ", netmask);
                            SendOneLine(cstrmask);
                        }
                        else if ( !ip[0] )
                        {
                            sprintf(cstrip,"*  管理口   ip: No ");
                            SendOneLine(cstrip);
                            sprintf(cstrmask,"*  子网掩码: No ");
                            SendOneLine(cstrmask);
                        }
                    }
                }
                else
                {
                    if(i==0)
                    {
                        if(pgb_var->eth_have_ip[i] == TYPE_COM)
                        {
                            ip[0] = 0;	
                            get_ethernet_msg(i,ip,netmask);
                            if (ip[0])
                            {
                                sprintf(cstrip,"*  备用口   ip: %s ",  ip);
                                SendOneLine(cstrip);
                                sprintf(cstrmask,"*  子网掩码: %s ", netmask);
                                SendOneLine(cstrmask);
                            }
                            else if ( !ip[0] )
                            {
                                sprintf(cstrip,"*  备用口   ip: No ");
                                SendOneLine(cstrip);
                                sprintf(cstrmask,"*  子网掩码: No ");
                                SendOneLine(cstrmask);
                            }
                        }

                    }
                    if(i==1)
                    {
                        if(pgb_var->eth_have_ip[i] == TYPE_COM)
                        {
                            ip[0] = 0;	
                            get_ethernet_msg(i,ip,netmask);
                            if (ip[0])
                            {
                                sprintf(cstrip,"*  管理口   ip: %s ",  ip);
                                SendOneLine(cstrip);
                                sprintf(cstrmask,"*  子网掩码: %s ", netmask);
                                SendOneLine(cstrmask);
                            }
                            else if ( !ip[0] )
                            {
                                sprintf(cstrip,"*  管理口   ip: No ");
                                SendOneLine(cstrip);
                                sprintf(cstrmask,"*  子网掩码: No ");
                                SendOneLine(cstrmask);
                            }
                        }

                    }
                }
            }
			/***************************************端口号******************************/
#if 1
			typort=get_port();
			sprintf(fileport,"*  文件传输端口号:%d ", typort.file_port);
			SendOneLine(fileport);
			sprintf(comdport,"*  命令传输端口号:%d ", typort.command_port);
			SendOneLine(comdport);
#endif
			/***************************************授权判断****************************/
            ret=Auto_ret(ACCREDIT);
            if(ret==1)
            sprintf(cstraccredit,"*  是否授权：是");
            else
            sprintf(cstraccredit,"*  是否授权：否");
			SendOneLine(cstraccredit);
#if 0
			snprintf(pattern,128,ACCREDIT);
			if(pattern[strlen(pattern)=='/'])
				pattern[strlen(pattern)]='\0';
			strcat(pattern,"/*.web");
			ret=glob(pattern, 0, NULL, &globres);
			if(ret!=0)
			{
				sprintf(cstraccredit,"*  是否授权：否");
			SendOneLine(cstraccredit);
				goto end_flag;
			}
			for(i=0;globres.gl_pathv[0][i]!='.';i++)
			{
				;
			}
			if(globres.gl_pathv[0][i-1]=='0')
				sprintf(cstraccredit,"*  是否授权：是");
			else
				sprintf(cstraccredit,"*  是否授权：否");
			SendOneLine(cstraccredit);
end_flag:
			globfree(&globres);
#endif
			/**************************************获得uuid******************************/
			bzero(temp_file,127);
			get_dev_id(FILE_UUID,temp_file);
			bzero(cstrnumber,1000);
			sprintf(cstrnumber,"*  设备UUID:%s",temp_file);
			//SendString(cstrnumber);
			SendOneLine(cstrnumber);
			/**************************************序列号********************************/
			//bzero(temp_file,127);
			//get_fileinformation(SERIAL_NUMBER,temp_file);
			//bzero(cstrnumber,1000);
			//sprintf(cstrnumber,"*  序列号:%s",temp_file);
			//SendOneLine(cstrnumber);

			SendOneLine("**************************************************");
			SendString("按任意键继续...");
			ReceiveOneLine(tp);
			system("clear");


		}

		else if(ret == OP_SET_INTERFACE) 
		{
interface_flag:
			system("clear");
			SendOneLine("ftp接口设置:");
			SendOneLine("(1) 开启");
			SendOneLine("(2) 关闭");
			SendString("chanct:");
			tret=ReceiveOneLine(tp);
			if (tret <= 0)
				goto main_menu_label;
			tret=trim_str(tp);

			ret1 = 0;
			if (tret==0) 
				goto interface_flag;

			ret1 = atoi(tp);
			if(ret1==1)
            {
ftp_open_flag:
				tret=ftp_open();
                if(tret<0)
                    goto main_menu_label;
                else if(tret==0)
                    goto ftp_open_flag;
                else
                    SendOneLine("ftp开启成功");
                
            }   
			else if(ret1==2)
            {
ftp_close_flag:
				tret=ftp_close();
                if(tret<0)
                    goto main_menu_label;
                else if(tret==0)
                    goto ftp_close_flag;
                else
                    SendOneLine("ftp关闭成功");
                
            }   
			else
				goto interface_flag;
			SendString("按回车键继续...");
			ReceiveOneLine(tp);
			system("clear");
		}
#endif
		else if(ret == OP_CLEAR_PASSWD)
		{
passwd_flag:
			system("clear");
			SendOneLine("确定开启直连网段吗?");
			SendOneLine("(1) 是");
			SendOneLine("(2) 否");
			SendString("chanct:");
			tret=ReceiveOneLine(tp);
			if (tret <= 0)
				goto main_menu_label;
			tret=trim_str(tp);

			ret1 = 0;
			if (tret==0) 
				goto passwd_flag;

			ret1 = atoi(tp);
			if(ret1==1)
			{
                bzero(temp,128);
				sprintf(temp,"python /gms/gapi/modules/est/security.pyc -o c -m 0 -d 1");
				system(temp);
#if 0
                bzero(temp,128);
				sprintf(temp,"rm -f %s",IP_TMP);
				system(temp);
                bzero(temp,128);
				sprintf(temp,"%s",KILL_PYTHON);
				system(temp);
#endif
				SendOneLine("直连网段开启成功");
				SendString("按回车键继续...");
				ReceiveOneLine(tp);

			}
			else if(ret1==2)
			{
				SendOneLine("取消操做");
				SendString("按回车键继续...");
				ReceiveOneLine(tp);
			}
			else
			{
				goto passwd_flag;
			}
			system("clear");
		}
        else if(ret==OP_SET_SSH)
        {
ssh_flag:
			system("clear");
			SendOneLine("设置ssh服务：");
			SendOneLine("(1) 开启");
			SendOneLine("(2) 关闭");
			SendString("chanct:");
			tret=ReceiveOneLine(tp);
			if (tret <= 0)
				goto main_menu_label;
			tret=trim_str(tp);

			ret1 = 0;
			if (tret==0) 
				goto ssh_flag;

			ret1 = atoi(tp);
			if(ret1==1)
			{
				system("/usr/local/sbin/sshd");
				SendString("按回车键继续...");
				ReceiveOneLine(tp);

			}
			else if(ret1==2)
			{
                system("killall sshd");
				SendString("按回车键继续...");
				ReceiveOneLine(tp);
			}
			else
			{
				goto ssh_flag;
			}
			system("clear");
        }

		else if(ret == OP_EXIT_PROG)
		{
			close_tty();
			SendOneLine("Quit");
			exit(0);
		}
#if 1
		else if(ret == OP_RESTORE_PARAM)
		{
			system("clear");
			system(CLEANCMD);
			SendString("按回车键继续...");
			ReceiveOneLine(tp);
			system("clear");

		}	
		else if(ret == OP_INIT_KEY)
		{
			system("clear");
			retkey=userlogin();
			if(retkey == 0)
			{
				SendOneLine("Sucess.");
			}
			else
			{
				SendOneLine("Fail.");
			
			}
			SendString("按回车键继续...");
			ReceiveOneLine(tp);
			system("clear");
		}
#endif
		else
			system("clear");
	}
}
