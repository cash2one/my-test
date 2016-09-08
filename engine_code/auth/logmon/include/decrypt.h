#ifndef _DECRYPT_H
#define _DECRYPT_H
#include "openssl/des.h"

#define SOURCE_FILE "/gms/auth/conf/auth-32-tsp.gau"
#define OBJECT_FILE "/gms/auth/conf/auth-32-timestamp.gau"
#define KEY "chanct-gms"

int DES_Decrypt(char *pszSource_file, char *szkey, char *pszObject_file);

#endif
