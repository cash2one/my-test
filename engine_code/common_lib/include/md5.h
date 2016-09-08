#ifndef MD5_H
#define MD5_H

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *, unsigned char *, unsigned int);
void MD5Final (unsigned char [16], MD5_CTX *);



/*-------------------------------------------------------------*/
/*  º¯ÊýÉùÃ÷                                                   */
/*-------------------------------------------------------------*/
void MessageDigest(unsigned char *szInput, unsigned int inputLen,
                        unsigned char szOutput[16]);


#endif/*MD5_H*/
