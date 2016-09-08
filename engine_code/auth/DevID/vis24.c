/*
 * vis24 encoding & decoding
 * 
 * by lenx, 2003.1
 *
 * vis24 encoding:
 *    4 byte binary data to 7 byte base24 data
 *    last 3 byte           6
 *         2                4
 *         1                2
 *
 *    use network byte order
 *
 *    base24 coding using: ABCFGHJKMQRTVWXY23456789
 *              to number:           11111111111222
 *                         012345678901234567890123
 *                         
 */

#include <string.h>
#include <stdlib.h>
#include "endian.h"

//#define DEBUG

/*
 * BASE24_init, prepare some data for BASE24 computing
 */
#define BASE 24
static char BASE24_codelookup[256]; /* 256 bytes */
static unsigned char BASE24_codebook[BASE+1]="ABCFGHJKMQRTVWXY23456789";
static int BASE24_codelookup_init=0;
void BASE24_init()
{
  int i;
  if(BASE24_codelookup_init) return;
  /* compute lookup table, warning! races between threads! */
  if(strlen(BASE24_codebook)!=BASE)
    return; /* failure! */
  for(i=0;i<256;i++)
    BASE24_codelookup[i]=-1;
  for(i=0;i<24;i++)
    BASE24_codelookup[BASE24_codebook[i]]=i;
  BASE24_codelookup_init=1;
}

/*
 * CKS24_compute, compute content's chsum24
 * 
 * input:
 *   code  : encoded content
 *   len   : length of code
 *
 * return value:
 *   cksum24 char : success
 *   0            : failure
 */
static unsigned char CKS24_compute(const unsigned char* code, const int len)
{
  unsigned int cks24=0;
  int i;

  if(len<=0) return 0;

  #ifdef DEBUG
  printf ("(len is %d)",len);
  #endif

  BASE24_init();

  for(i=0;i<len;i++){
    char c=BASE24_codelookup[code[i]];
    if(c<0)return 0;
    cks24+=c;
  }
  
  return BASE24_codebook[cks24%24];
}

/*
 * CKS24_verify, verify encoded content's cksum24
 * 
 * input:
 *   code  : encoded content
 *
 * return value:
 *   0     : success
 *   <0    : failure
 *   -1    : bad char, not in BASE24 codebook
 *   -2    : bad cksum
 */
int CKS24_verify(const char* code)
{
  int len=strlen(code);
  unsigned char cks24=CKS24_compute(code,len-1);
  if(!cks24)return -1; 
  if(cks24!=code[len-1])return -2;
  return 0;
}

/* warning: ldiv_t & ldiv in std libc is signed !! */
typedef struct{unsigned long quot; unsigned long rem;} udiv_t;
static inline udiv_t udiv(unsigned long numer, unsigned long denom)
{
	udiv_t t;
	t.quot=numer / denom;
	t.rem=numer % denom;
	return t;
}

/*
 * vis24_encode
 *   encode data to vis24 string
 *
 * input:
 *    data    : which to be encoded
 *    len     : length of data
 *    buf_len : length of result buffer
 *              should be vis24_len(len)+1
 *
 * output:
 *    buf     : with ending '\0'
 *
 * return value:
 *    0       : success
 *    -1      : erro, usually buffer not enough
 */
int vis24_encode(char* buf, const int  buf_len, const unsigned char *data, const int len)
{
	int i,j,len1;
	unsigned char *p=(unsigned char*)buf, *q=(unsigned char*)data;

	udiv_t k=udiv(len,4);

	len1=k.quot*7+k.rem*2+1;
	if(buf_len < len1+1) return -1;

#ifdef DEBUG
	printf ("(k is (%d,%d))",k.quot,k.rem);
#endif 

	for(i=0;i<k.quot;i++){
		unsigned long w=(*(unsigned long*)q);
		for(j=0;j<7;j++){
			udiv_t t=udiv(w,BASE);
			*(p+6-j)=BASE24_codebook[t.rem];
			#ifdef DEBUG
			printf ("(%c,%ul,%ul)",*(p+6-j),t.rem,w);
			#endif 
			w=t.quot;
		}//j
		q+=4;
		p+=7;
	}//i
	
	if(k.rem!=0){
		unsigned long w=0;
		int r=2*k.rem;
		for (i=0;i<k.rem;i++)
			w=(w<<8)+*(q+i);
		#ifdef DEBUG
		printf("(rem is %d,%d)",k.rem,w);
		#endif
		for (j=r-1;j>=0;j--){
			udiv_t t=udiv(w,BASE);
			*(p+j)=BASE24_codebook[t.rem];
			#ifdef DEBUG
			printf ("%c,",*(p+j));
			#endif 
			w=t.quot;
		}
		p+=r;
	}
	*p=CKS24_compute(buf,len1-1);
	p++;
	*p=0;

	return 0;
}

/*
 * vis24_decode
 *    decode vis24 string to data
 *  
 * input:
 *    vis24str : string in vis24 encoding
 *    buf_len  : length of buffer, 
 *                  which should be:
 *                      len=strlen(vis24str);
 *                      (quot,rem)=udiv(len-1,7);
 *                      buf_len=quot*4+rem/2;
 *
 * output:
 *    buf  : decoded data
 *    len  : length of decoded data
 *
 * return value:
 *    0    : success
 *    -1   : wrong char in vis24 string
 *    -2   : wrong cksum24
 *    -3   : not enough buffer
 *    -4   : wrong length of vis24 string
 *    -100 : other errors
 */
int vis24_decode(unsigned char* buf, int *len, const int buf_len, const char* vis24str)
{
	int r,i,j;
	int l=strlen(vis24str);
	unsigned char *p=(unsigned char*)vis24str, *q=buf;

	udiv_t k=udiv(l-1,7);

	*len=k.quot*4+k.rem/2;
	if(buf_len<*len) return -3;
	if((k.rem/2)*2!=k.rem)	return -4;

	r=CKS24_verify(vis24str);
	if(r)return r;

	for(i=0;i<k.quot;i++){
		unsigned int w=0;
		for(j=0;j<7;j++){
			char c=BASE24_codelookup[*(p+j)];
			if((c<0)||(c>=BASE)) return -1;
			w=w*BASE+c;
		}
		*((unsigned long*)q)=(w);
		q+=4;
		p+=7;
	}
	
	if(k.rem!=0){
		unsigned int w=0;
		for(j=0;j<k.rem;j++){
			char c=BASE24_codelookup[*(p+j)];
			if((c<0)||(c>=BASE)) return -1;
			w=w*BASE+c;
		}
		#ifdef DEBUG
		printf ("(w is %d)",w);
		#endif
		for(j=k.rem/2-1;j>=0;j--){
			*(q+j)=w&0xff;
			w>>=8;
		}
	}
	return 0;
}

/*
 * dash4, insert "-" between per 4 chars
 * 
 * input:
 *    buf     : string buffer
 *    buf_len : length of buffer
 *
 * output:
 *    buf     : string with inserted "-"s, with ending '\0'
 *
 * return value:
 *    0       : success
 *    -1      : not enough space
 */
int dash4(char * buf, const int buf_len)
{
	int i,j, len;
	udiv_t k=udiv(strlen(buf),4);
	char *s, *d;
	len=k.quot*5+k.rem+1-(k.rem==0?1:0);
	if(k.quot==0) return 0; /* no need */
	if(buf_len < len) return -1;

	/* ending null */
	d=buf+len-1;
	*d=0; d--;

	/* last k.rem bytes */
	s=buf+strlen(buf)-1; 
	for(j=0; j<k.rem;j++){
		*d=*s;
		d--;
		s--;
	}

	/* k.quot of 4 bytes */
	for(i=0; i<k.quot; i++){
		if(i!=0 || k.rem!=0){
			*d='-'; d--;
		}
		*((long*)(d-3))=*((long*)(s-3));
		d-=4;
		s-=4;
	}

	return 0;
}

/*
 * undash, remove "-" from string
 *
 * input: 
 *   buf  : string which need to remove "-"
 *
 * output:
 *   buf  : string without "-"
 */
void undash(char* buf)
{
	int i, len=strlen(buf);
	char *s, *d;
	
	s=d=buf;
	for(i=0;i<len;i++){
		if(*s=='-') s++;
		else{
			*d=*s;
			d++;
			s++;
		}
	}
	*d=0;
}

#ifdef VIS24_UNIT_TEST

#include "bin2hex.h"

main()
{
	{
		char data1[7]="\x00\x23\x32\x43\xef\xe8";
		char d1[7];
		char data2[7]="\x00\x23\x32\x43\xef\xe9";
		char d2[7];
		char str[15];
		int r,len,r1;
		
		BASE24_init();
		
		bin2hex(str,15,data1,6);
		str[12]=0;
		printf("data1 is %s\n",str);
		
		r=vis24_encode(str,15,data1,6);
		r1=dash4(str,15);
		printf("vis 1 is %s, ret is %d,%d\n",str,r,r1);
		
		undash(str);
		r=vis24_decode(d1,&len, 7, str);
		bin2hex(str,15,d1,len);
		str[2*len]=0;
		printf("decode is %s, ret is %d\n", str,r);
		
		bin2hex(str,15,data2,6);
		str[12]=0;
		printf("data2 is %s\n",str);
		
		r=vis24_encode(str,15,data2,6);
		printf("vis 2 is %s, ret is %d\n",str,r);
		
		r=vis24_decode(d2,&len, 7, str);
		bin2hex(str,15,d2,len);
		str[2*len]=0;
		printf("decode is %s, ret is %d\n", str,r);
	}

	{
		unsigned char p[15]="\x23\x43\xe3\xf3\xe5\xb3\xa5\xc9\xd0\xf2\x61\x98\x05\x51";
		unsigned char p1[15];
		char str[33];
		int r,r1,len;
		bin2hex(str,33,p,14);
		str[2*strlen(p)]=0;
		printf ("data 14 is %s\n", str);

		r=vis24_encode(str,33,p,14);
		printf("vis 14 is %s, ret is %d\n",str,r);
		r1=dash4(str,33);
		printf("dash4 is %s, ret is %d\n",str,r1);

		undash(str);
		r=vis24_decode(p1,&len,15,str);
		bin2hex(str,32,p1,len);
		str[2*len]=0;
		printf("decode is %s, ret is %d\n",str,r);
	}
	{
		unsigned char p[15]="\x23\x43\xe3\xf4\xe5\xb3\xa5\xc9\xd0\xf2\x61\x98\x05\x51";
		unsigned char p1[15];
		char str[33];
		int r,r1,len;
		bin2hex(str,33,p,14);
		str[2*strlen(p)]=0;
		printf ("data 14 is %s\n", str);

		r=vis24_encode(str,33,p,14);
		printf("vis 14 is %s, ret is %d\n",str,r);
		r1=dash4(str,33);
		printf("dash4 is %s, ret is %d\n",str,r1);

		undash(str);
		r=vis24_decode(p1,&len,15,str);
		bin2hex(str,32,p1,len);
		str[2*len]=0;
		printf("decode is %s, ret is %d\n",str,r);
	}
}
#endif //VIS24_UNIT_TEST
