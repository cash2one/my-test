#include"port.h"
#include"term_ttyrd.h"
#include"term_ip.h"
#include<stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<ctype.h>
Nport get_port()
{
	FILE*port_fd;
	Nport tmpport;
	size_t s;
    char comm[6];
    char file[6];
	char cstrport[1000];
	char num;
    char i=0,j=0;
	port_fd=fopen(NETPORT,"r");
	if(port_fd==NULL)
	{
		sprintf(cstrport, "fopen(%s):%s", NETPORT,strerror(errno));
		SendOneLine(cstrport);
		exit(-1);
	}
	while(feof(port_fd)==0)
	{
	    s=fread(&num,1,1,port_fd);
        if(s<=0)
    {
        break;
        
    }
        else if(num=='r')
    {
	    s=fread(&num,1,1,port_fd);
        if(num=='t')
    {
	    s=fread(&num,1,1,port_fd);
        if(num=='>')
    {       i=0;
        do{
	        s=fread(&num,1,1,port_fd);
            if(s<=0||num=='<')
            break;
            if(j==0)
            {
            comm[i]=num;
            i++;
            }
            else if(j==2)
            {
                file[i]=num;
                i++;
            }
//                printf("%d\n",j);
                
            }while(s>0);
            j++;
        if(j==3)
        break;
    }
        
    }
        
    }
            
}
			tmpport.file_port=atoi(file);
			tmpport.command_port=atoi(comm);	

	fclose(port_fd);
		return tmpport;

}
int set_file_port(FILE *fd,FILE *tmp_fd,char *fileport,size_t a)
{
	size_t s=0;
	char   ch;
	size_t i=0,j=0;
	while(feof(fd)==0)
	{
		s=fread(&ch,1,1,fd);
		if(s<=0)
		{
			break;
		}
			fwrite(&ch,1,1,tmp_fd);
		if(ch=='r')
		{
			s=fread(&ch,1,1,fd);
			fwrite(&ch,1,1,tmp_fd);
			if(ch=='t')
			{
			s=fread(&ch,1,1,fd);
			fwrite(&ch,1,1,tmp_fd);
			if(ch=='>')
			{
				if(j==a)
				{
				for(i=0;ch!='<';i++)
				{
					s=fread(&ch,1,1,fd);
				}
				
				fprintf(tmp_fd,fileport);
				fwrite(&ch,1,1,tmp_fd);
				}
				j++;
			}
				
			}
		}
	}
}

int set_port()
{
	FILE*port_r;
	FILE*port_w;
	int set,tret;
	int ret,ret1;
	char tp[500];
	char *cmdgv[]={"cp",TMP_NETPORT,NETPORT,NULL};
	char cstrport[1000];
	Nport typort;
	char fileport[1000];
	char comdport[1000];
	pid_t pid;
	SendOneLine("当前端口号为:");
	typort=get_port();
	sprintf(fileport,"(1)  文件传输端口号:%d ", typort.file_port);
	SendOneLine(fileport);
	sprintf(comdport,"(2)  命令传输端口号:%d ", typort.command_port);
	SendOneLine(comdport);
port_flag:
	SendOneLine("请选择要修改的端口代号1或2:");

	port_r=fopen(NETPORT,"r");
	if(port_r==NULL)
	{
		sprintf(cstrport, "fopen(%s):%s", NETPORT,strerror(errno));
		SendOneLine(cstrport);
		return -1;
	}
	port_w=fopen(TMP_NETPORT,"w");
	if(port_w==NULL)
	{
		sprintf(cstrport, "fopen(%s):%s", TMP_NETPORT,strerror(errno));
		SendOneLine(cstrport);
		fclose(port_r);
		return -1;
	}



	bzero(tp,500);
	set=ReceiveOneLine(tp);
	if(set<0)
	{
		fclose(port_r);
		fclose(port_w);
		return -1;
	}
	else if(set>0)
	{
		tret=trim_str(tp);
		if(tret>0)
		{
			ret=atoi(tp);
			if(ret==1)
			{
file_port_flag:
				while(1)
				{
					SendOneLine("请输入文件传输端口号:");
					bzero(tp,500);
					set=ReceiveOneLine(tp);
					if(set<0)
					{
						fclose(port_r);
						fclose(port_w);
						return -1;
					}
					else if(set>0)
					{
						tret=trim_str(tp);
						if(tret>0)
						{
							ret1=atoi(tp);
							if(ret1<1024||ret1>65535)
							{
								SendOneLine("输入的端口号无效，请重新输入");
								goto file_port_flag;
							}
							set_file_port(port_r,port_w,tp,2);
							fclose(port_r);
							fclose(port_w);
                            system("rm -rf NETPORT");
							pid=fork();
                            
							if(pid==0)
							{
								execvp(cmdgv[0],cmdgv);
								exit(1);
							}
							wait(NULL);
							SendOneLine("文件端口设置成功");

							return 0;
						}
					}

				}
			}
			else if(ret==2)
			{
				while(1)
				{
					SendOneLine("请输入命令传输端口号:");
					bzero(tp,500);
					set=ReceiveOneLine(tp);
					if(set<0)
					{
						fclose(port_r);
						fclose(port_w);
						return -1;
					}
					else if(set>0)
					{
						tret=trim_str(tp);
						if(tret>0)
						{
							ret1=atoi(tp);
							if(ret1<=0)
							{
								SendOneLine("输入的端口号无效，请重新输入");
								goto file_port_flag;
							}
							set_file_port(port_r,port_w,tp,0);

							fclose(port_r);
							fclose(port_w);
                            system("rm -rf NETPORT");
							pid=fork();
							if(pid==0)
							{
								execvp(cmdgv[0],cmdgv);
								exit(1);
							}
							wait(NULL);
							SendOneLine("命令端口设置成功");

							return 0;
						}
					}

				}
			}
			else
				goto port_flag;
		}
		else
			goto port_flag;
	}
	else
		goto port_flag;

}
