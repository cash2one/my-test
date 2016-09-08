#include "term_main.h"
int ftp_open()
{
    FILE*fd;
    char ftp[50]={0};
    int ret;
    char username[40]={0};
    char *pswd=NULL;
    //char flag;
    SendString("请输入ftp的ip:");
    ret=ReceiveOneLine(ftp);
    if(ret<=0)
    {
        return ret;
    }
    ret=trim_str(ftp);
    if(ret<=0)
    {
        return ret;
    }
    if(!IsValidIpaddr(ftp))
    {
        SendOneLine("输入的ip无效请重新输入");
        return 0;
    }
    SendString("user name:");
    ret=ReceiveOneLine(username);
    if(ret<=0)
    {
        return ret;
    }
    ret=trim_str(username);
    if(ret<=0)
    {
        return ret;
    }
    pswd=getpass("passwd:");

        fd=fopen(FTPFILE,"w");
        if(fd==NULL)
        {
            perror("open()");
            return -1;
        }
#if 0
ftp_flag:
    SendString("请选择是否开启ftp服务(y/n):");
    ret=RecevieOneLine(&flag);
    if(ret<0)
    return ret;
    else if(ret==1)
    {
        fd=fopen("FTPFILE","w");
        if(fd==NULL)
        {
            perror("open()");
            return -1;
        }
        if(flag=='y')
        {
#endif
            fprintf(fd,"[ftp_server]\n");
            fprintf(fd,"ip = %s\n",ftp);
            fprintf(fd,"port = 21\n");
            fprintf(fd,"user = %s\n",username);
            fprintf(fd,"pwd = %s\n",pswd);
            fprintf(fd,"enable = yes\n");
            fclose(fd);
#if 0
        }
        else if(flag=='n')
        {
            fprintf(fd,"[ftp_server]\n");
            fprintf(fd,"ip = %s\n",ftp);
            fprintf(fd,"port = 21\n");
            fprintf(fd,"user = %s\n",username);
            fprintf(fd,"pwd = %s\n",pswd);
            fprintf(fd,"enable = no\n");
            fclose(fd);
        }
        else
        {
            fclose(fd);
            goto ftp_flag;
        }
    }
#endif

    return 1;
}



int ftp_close()
{
        FILE*fd;
        fd=fopen(FTPFILE,"w");
        if(fd==NULL)
        {
            perror("open()");
            return -1;
        }
    
            fprintf(fd,"[ftp_server]\n");
            fprintf(fd,"ip = no\n");
            fprintf(fd,"port = 21\n");
            fprintf(fd,"user = no\n");
            fprintf(fd,"pwd = no\n");
            fprintf(fd,"enable = no\n");
            fclose(fd);
        return 1;
}

