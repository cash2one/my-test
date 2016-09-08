#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"decrypt.h"
//#include "openssl/des.h"

#if 0 
int DES_Decrypt(char *pszSource_file, char *szkey, char *pszObject_file);

int main(void)
{
    
    char szkey[32];
    char szSource_file[100];
    char szObject_file[1024];
    
    strcpy(szSource_file,"/home/xiedongling/svn/GMS/trunk/engine_code/auth/conf/auth-32-tsp.gau");
    strcpy(szObject_file,"/home/xiedongling/svn/GMS/trunk/engine_code/auth/conf/auth-32-timestamp.gau");
    strcpy(szkey,"chanct-gms");
   
    printf("Original Key: %s\n", szkey); 
    printf("Source file: %s\n",szSource_file);
    printf("Object file: %s\n",szObject_file);
    
    DES_Decrypt(szSource_file, szkey, szObject_file);

    return 0;
}
#endif

int DES_Decrypt(char *pszSource_file, char *szkey, char *pszObject_file)
{
    FILE *pfPlain, *pfCipher;
    int icount = 0, times = 0;
    long fileLen;
    unsigned char szPlainBlock[8], szCipherBlock[8];
    
    if((pfPlain = fopen(pszObject_file, "wb")) == NULL)
    {   
        printf ("==============\nopen file [ %s ] error!\n==============\n", pszObject_file);
        return -1;
    }
    if((pfCipher = fopen(pszSource_file, "rb")) == NULL)
    {   
        printf ("==============\nopen file [ %s ] error!\n==============\n", pszSource_file);
        return -1;
    }
    
    DES_key_schedule key_schedule;
    DES_cblock ivec;
    const_DES_cblock key[1];
    DES_string_to_key(szkey, key);
    DES_set_key_checked(key, &key_schedule);

   
    fseek(pfCipher, 0, SEEK_END);   
    fileLen = ftell(pfCipher);      
    rewind(pfCipher);               
    while(1)
    {
       
        fread(szCipherBlock, sizeof(char), 8, pfCipher);
        memset((char*)&ivec, 0, sizeof(ivec));
        DES_ncbc_encrypt(szCipherBlock, szPlainBlock, 8, &key_schedule, &ivec, DES_DECRYPT);                      
        times += 8;
        
        if(times < fileLen)
        {
            fwrite(szPlainBlock, sizeof(char), 8, pfPlain);
        }
        else
        {
            break;
        }
    }

   

    if(szPlainBlock[7] < 8)
    {
        for(icount = 8 - szPlainBlock[7]; icount < 7; icount++)
        {
            if(szPlainBlock[icount] != '\0')
            {
                break;
            }
        }
    }   
    if(icount == 7)        
    {
        fwrite(szPlainBlock,sizeof(char), 8 - szPlainBlock[7], pfPlain);
    }
    else                   
    {
        fwrite(szPlainBlock, sizeof(char), 8, pfPlain);
    }
    fclose(pfPlain);
    fclose(pfCipher);

    return 0;
}
