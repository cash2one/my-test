#include "login.h"
#define _XOPEN_SOURCE
#include "term_ttyrd.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <unistd.h>
#include <stdlib.h>

int userlogin()
{
	char keycmd[100]={0};
	char  *passwd=NULL,*passwdbak=NULL;
	char pword[100]={0};
//	struct spwd *psp=NULL;
//	struct passwd *pw=NULL;
//	char *pen;
	int ret=0;
	setechoflag();
//	SendString("input name:");
//	ReceiveOneLine(name);
//	setpwd();
	//SendString("passwd:");
key_flag:
	SendOneLine("Changing password for user admin.");
	passwd=getpass("new passwd:");
	if(strlen(passwd) == 0)
	{
		SendOneLine("BAD PASSWORD: it is WAY too short");
		SendOneLine("BAD PASSWORD: is a palindrome");
	}
	strcpy(pword,passwd);
	passwdbak=getpass("Retype new passwd:");
	if(strlen(passwd) == 0)
	{
		SendOneLine("No password supplied");
		ret=1;

	}
	printf("%s  %s\n",pword,passwdbak);
	if(ret == 1)
	{
		ret =0;
		SendOneLine("passwd: Authentication token manipulation error");
		return -1;
	}
	if(strcmp(pword,passwdbak) == 0)
	{
		sprintf(keycmd,"%s %s",INIT_KEY,passwdbak);
		if(system(keycmd) == 0)
			return 0;
		else
			return -1;
	}else{
		SendOneLine("Sorry, passwords do not match.");
		goto key_flag;
	}




	
	//ReceiveOneLine(passwd);
#if 0
	psp = getspnam(name);
	if ( psp == NULL ) {
		SendOneLine("usrname no exist.\n");
		return -1;
	}
	
	//将passwd按照name这个用户的密码的加密方法进行加密
	pen = crypt(passwd, (const char*)psp->sp_pwdp);

	if ( strcmp(pen, psp->sp_pwdp) == 0 ) {
		SendOneLine("login successfully...");
		pw=getpwnam(name);
		setuid(pw->pw_uid);
		return 1;
	}
	else {
		SendOneLine("login failed...");
		return -1;
	}
#endif
	return 1;

}


void print_welcome()
{
	int ret;
	char fp[200];
	while(1)
	{
		system("clear");
	SendOneLine("********************************欢迎进入************************************");
	SendOneLine("*              *                                                           *");
	SendOneLine("*        * *   *                                         * *       *       *");
	SendOneLine("*      *       ********       *****     ********       *       **********  *");
	SendOneLine("*    *         *       *    *      *    *       *    *             *       *");
	SendOneLine("*   *          *       *   *       *    *       *   *              *       *");
	SendOneLine("*    *         *       *   *       *    *       *    *             *       *");
	SendOneLine("*      *       *       *    *      *    *       *      *           *       *");
	SendOneLine("*        * *   *       *     ****** *   *       *        * *        ***    *");
	SendOneLine("*                                                                          *");
	SendOneLine("**************************http://mail.chanct.com****************************");
	SendOneLine("****************************************************************************");
	SendOneLine("*                              串口设置选项                                *");
	SendOneLine("****************************************************************************");
	SendString("按回车键进入设置...");
	ret=ReceiveOneLine(fp);
	if(ret<=0)
		return; 
	}
}
