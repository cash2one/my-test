#include "term_ttyrd.h"
int ini_tty(char * aName,int aBaud)
{
	int s;
	struct termio tfd;
	if ((_chanct_fd=open(aName,O_RDWR))<0)
	{
	  perror("Open com-port error\n");
	  _chanct_fd=-1;
	  goto ret;
	} 
	
	s=ioctl(_chanct_fd,TCGETA,&tfd);//tcgetattr:得到终端属性
	if (s==-1)
	{
  	  perror("ioctl  com-port error\n");
	  _chanct_fd=-1;
	  goto ret;
	}
	tfd.c_lflag&=~(ICANON|ISIG|ECHO);

	tfd.c_iflag&=~(INLCR|ICRNL|IUCLC|ISTRIP|IXON|BRKINT);
	tfd.c_oflag&=~OPOST;
	tfd.c_cc[VMIN]=1;
	tfd.c_cc[VTIME]=1;
	tfd.c_cflag&=~(CBAUD|CSTOPB|PARENB|CSIZE);
	if (aBaud==9600)
	    tfd.c_cflag|=B9600;
	if (aBaud==4800)
	    tfd.c_cflag|=B4800;
	if (aBaud==1200)
	    tfd.c_cflag|=B1200;
	if (aBaud==600)
	    tfd.c_cflag|=B600;
	tfd.c_cflag|=CS8;
	
	s=ioctl(_chanct_fd,TCSETAW,&tfd);//TCSANOW
	if (s==-1)
        {
	  perror("ioctl  com-port error\n");
	  _chanct_fd=-1;
	  goto ret;
	}
	ioctl(_chanct_fd,TCFLSH,2);//2表示刷新输入输出队列
       // s=fcntl(_chanct_fd,F_SETFL,O_NONBLOCK);//对打开的终端文件设置非阻塞方式
        s=fcntl(_chanct_fd,F_SETFL,0);//对打开的终端文件设置非阻塞方式
	if (s<0) 
	{
	  perror("fcntl  com-port error\n");
	  _chanct_fd=-1;
	  goto ret;	
	}
	return _chanct_fd;
ret:
	close(_chanct_fd);
	return _chanct_fd;
}
int  read_232(char * aBuf,int aNum)
/****************************************************************
*         read data from com port                               *
*****************************************************************/
{
	int s;
	char tmp;
	tmp = '*';
	//s=read(_chanct_fd,aBuf,aNum);
	s=read(STDIN_FILENO,aBuf,aNum);
	if(s>0)
	{
		if((*aBuf)==8)
	    		return s;
	    	if((*aBuf)==9)
	    	{
			*aBuf=' ';
	    	}
	}
#if 0
        if ( s > 0 && _eFlag==1) //允许回显
    //         write(_chanct_fd,aBuf,s);
             write(STDIN_FILENO,aBuf,s);
	else
	  if ( s > 0 && _eFlag==2){//输入为口令
	    if ((*aBuf==10)||(*aBuf==13))
	//	write(_chanct_fd,aBuf,s);
		write(STDIN_FILENO,aBuf,s);
	    else	 	
	//        write(_chanct_fd,&tmp,s);
	        write(STDIN_FILENO,&tmp,s);
	  }
#endif
	return s;

}
int write_232(char * aBuf,int aNum)
/*****************************************************************
*          write data into com port                              *
******************************************************************/
{
	int s;
	//s=write(_chanct_fd,aBuf,aNum);
	s=write(STDOUT_FILENO,aBuf,aNum);
	return s;
}
void close_tty()
/*****************************************************************
*          close com port                                        *
******************************************************************/
{
	close(_chanct_fd);
}
void setechoflag()
{
	_eFlag=1;
}
void clearechoflag()
{
	_eFlag=0;
}
void setpwd()
{
	_eFlag=2;
}
int ReceiveOneLine(char * p){
     char ch;
     int s;
     long tm;
     int curlen;	
     int iFlag;
     iFlag=0;
     tm=0;
     curlen=0;
     while(1){
       s=read_232(&ch,1);

//    printf("ch=%c\n",ch);
	 if(s>0)
	 {
      if (ch==13||ch==10) 
		{
		  break;
		}
              else
	      {
             	if (ch==(char)8)
	  	{
#if 1
			if(curlen>0)	
               	 	{
				ch = 0x8;
                    		write_232(&ch,1);
                    		ch = ' ';
                    		write_232(&ch,1);
                    		ch = 0x8;
                    		write_232(&ch,1);
                    	}
#endif
                       	curlen--;
			if (curlen<0)
				curlen=0;
		}
	    	else
		{
               		if(! (ch==(char)27))
			{ 
				p[curlen]=ch;
				if (curlen<500) 	
				curlen++;
			}	
			iFlag=0;	
               	}
             }   
	      	tm=0;	
	}
    	else
    	{
        	tm++;
	        if( tm >max_wait_time)
	 	{
			return -1;	 
		}	
	}
	 usleep(sleep_time);	
    }
	if(curlen >= 0) p[curlen]='\0';
	if ((curlen == 1) && (p[0] == 'q')) 
	{
       	write_232("\n",1);
		return -1;	
	}
	else
	{
       	write_232("\n",1);
    	return curlen;
	}

} 



int SendOneLine(char * p)
{
      int s;
      int len; 
      char p1[502];
      len=strlen(p);
      if (len>500) return -2;
      strcpy(p1,p);
      p1[len]=(char)13;
      p1[len+1]=(char)10;
      s = write_232(p1,len+2);
      if (s<len+2) return -1;
      return s;
}
int SendString(char * p){
     unsigned int s;
     s=write_232(p,strlen(p));
     if (s <strlen(p)) return -1;
     return s;
}
