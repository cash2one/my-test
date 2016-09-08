#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include "common.h"

inline int set_thread_title(const char* fmt, ...)
{
    char title [64] ={0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf (title, sizeof (title) , fmt, ap);
    va_end (ap);

   return prctl(PR_SET_NAME,title) ;

}

#define KEYVALLEN 127

/*   É¾³ý×ó±ßµÄ¿Õ¸ñ   */
char * tcad_conf_l_trim(char * szOutput, const char *szInput)
{
    for (; *szInput != '\0' && isspace(*szInput); ++szInput) {
        ;
    }
    return strncpy(szOutput, szInput, KEYVALLEN);
}

/*   É¾³ýÓÒ±ßµÄ¿Õ¸ñ   */
char *tcad_conf_r_trim(char *szOutput, const char *szInput)
{
    char *p = NULL;
    strncpy(szOutput, szInput, KEYVALLEN);
    for (p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p){
        ;
    }
    *(++p) = '\0';
    return szOutput;
}

/*   É¾³ýÁ½±ßµÄ¿Õ¸ñ   */
char * tcad_conf_a_trim(char * szOutput, const char * szInput)
{
    char *p = NULL;
    tcad_conf_l_trim(szOutput, szInput);
    for (p = szOutput + strlen(szOutput) - 1; p >= szOutput && isspace(*p); --p) {
        ;
    }
    *(++p) = '\0';
    return szOutput;
}

int tcad_conf_get_profile_string(char *profile, char *AppName, char *KeyName, char *KeyVal )
{
    char appname[32],keyname[32], KeyVal_o[64];
    char *buf = NULL, *c = NULL;
    char buf_i[KEYVALLEN+1] = {0}, buf_o[KEYVALLEN+1] = {0};
    FILE *fp;
    int found=0; /* 1 AppName 2 KeyName */

    if ( (fp=fopen( profile,"r" ))==NULL ){
        printf( "openfile [%s] error [%s]\n",profile,strerror(errno) );
        return(-1);
    }
    fseek( fp, 0, SEEK_SET );
    memset( appname, 0, sizeof(appname) );
    sprintf( appname,"[%s]", AppName );

    while( !feof(fp) && fgets( buf_i, KEYVALLEN, fp )!=NULL ){
        tcad_conf_l_trim(buf_o, buf_i);
        if( strlen(buf_o) <= 0 )
            continue;
        buf = buf_o;

        if( found == 0 ){
            if( buf[0] != '[' ) {
                continue;
            } else if ( strncmp(buf, appname, strlen(appname))==0 ){
                found = 1;
                continue;
            }
        } else if( found == 1 ){
            if( buf[0] == '#' ){
                continue;
            } else if ( buf[0] == '[' ) {
                break;
            } else {
                if( (c = (char*)strchr(buf, '=')) == NULL )
                    continue;
                
                memset( keyname, 0, sizeof(keyname) );

                sscanf( buf, "%[^=|^ |^\t]", keyname );

                if (0 == strcmp(keyname, KeyName)){
                    sscanf( ++c, "%[^\n]", KeyVal );

                    memset(KeyVal_o, 0, sizeof(KeyVal_o));
                    tcad_conf_a_trim(KeyVal_o, KeyVal);
                    if ('\0' != KeyVal_o[0])
                        strcpy(KeyVal, KeyVal_o);
                
                    found = 2;
                    break;
                } else {
                    continue;
                }
            }
        }
    }

    fclose( fp );
    
    if( found == 2 )
        return(0);

    return(-1);
}


