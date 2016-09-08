#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BOWSER_MAX_NUM 50

/*********************************************************
 * Mozilla 为识别浏览类型的二维数组                      *
 * ******************************************************/
char Mozilla[BOWSER_MAX_NUM][15]={"MSIE","Netscape","Opera","Safari","Firefox","Galeon","Epiphany","Konqueror","Debian","Avant Browser","Googlebot","Wget","Msnbot","ELinks","Links","msnbot","Msnbot","w3m","HTTP","Download Demon","EmailWolf","Email Wolf","OWF","grub-client","Gulper Web Bot","MultiZilla",""};
int mystrcmp(char *src,char*match,char*vernum,int len,int flag)
{
    int i,j,ret;
    char *ptr=src;
    char *pch=match;
    for(i=0;i<strlen(ptr);i++)
    {
        if(ptr[i]==pch[0])
        {
            if(!strncmp(&ptr[i],pch,strlen(pch)))
            {
                if(vernum)
                {
                    ret=0;
                    for(j=i+strlen(pch)+flag;(ptr[j]!=' ')&&(ptr[j]!=';')&&((j-(i+strlen(pch))+flag)<len)&&(ptr[j]!='\0')&&(ptr[j]!='\n')&&(ptr[j]!='\r');j++)
                    {
                        if((ptr[j]=='.')&&(ret==1))
                            return 0;
                        vernum[j-(i+strlen(pch))-flag]=ptr[j];
                        if(ptr[j]=='.')
                            ret=1;
                    }
                }
                return 0;
            }

        }
    }
    return -1;
}    

/*************************************************
***@user_Agent 为uer_Agent的字符串   *************
***@id         为浏览器类型的id号    *************
***@version    为浏览器的版本	     *************
***@如果没有匹配的浏览器类型，id=0，version=""****
*************************************************/
int match_str(char *user_Agent,int *id,char *version, int vmaxlen)
{
    char *text = NULL;
    //char *pattern = NULL;
    //int *next = NULL;
    int i, ret = 0;
    char ver[10]={0};
    
    text = user_Agent;
    if (!mystrcmp(text, "Mozilla/", NULL, 0, 0))
    {
        for (i=0; i<sizeof(Mozilla)/sizeof(Mozilla[0])&&(strlen(Mozilla[i])>0); i++)
        {
            if(!mystrcmp(text,Mozilla[i],ver,10,1))
            {
                ret=1;
                break;
            }
        }
        if(ret==1)
        {
            *id=i+1;
            strncpy(version, ver, vmaxlen);
        }
        else
        {
            *id=0;
            strncpy(version, "", vmaxlen);
        }
            
    }
    else
    {
        //char buf[15]={0};
        for(i=0;i<sizeof(Mozilla)/sizeof(Mozilla[0])&&(strlen(Mozilla[i])>0);i++)
        {
            ret=0;
            if(!mystrcmp(text,Mozilla[i],ver,10,1))
            {
                ret=1;
                *id=i+1;
                strncpy(version, ver, vmaxlen);
                break;

            }
        }
        if(ret==0)
        {
            *id=0;
            strncpy(version, "", vmaxlen);
        
        }


    }
    return 0;
}

int browser_get(char *useragent, int *browser_id, char *version, int vmaxlen)
{
    //int id = 0;
    match_str(useragent, browser_id, version, vmaxlen);

    //if (id > 0 && id <= BOWSER_MAX_NUM) {
    //    strncpy(browser, Mozilla[id-1], bmaxlen);
    //}
    //printf("id=%d browser=%s  ver=%s\n",id,Mozilla[id-1],ver);

    return 0;
}
