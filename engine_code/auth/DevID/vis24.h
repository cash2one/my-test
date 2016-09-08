#ifndef _vis24_h
#define _vis24_h

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
 *    plus one byte CKS24 char
 */

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
int vis24_encode(char* buf, const int  buf_len, const unsigned char *data, const int len);

/*
 * vis24_decode
 *    decode vis24 string to data
 *  
 * input:
 *    vis24str : string in vis24 encoding
 *    buf_len  : length of buffer, 
 *                  which should be:
 *                      len=strlen(vis24str);
 *                      (quot,rem)=ldiv(len-1,7);
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
int vis24_decode(unsigned char* buf, int *len, const int buf_len, const char* vis24str);

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
     int CKS24_verify(const char* code);

/*
 * BASE24_init, prepare some data for BASE24 computing
 *
 * code book :"ABCFGHJKMQRTVWXY23456789";
 *
 * no need to call in one thread enviroment, it would be called
 * when those data are first used.
 */
void BASE24_init();

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
int dash4(char * buf, const int buf_len);

/*
 * undash, remove "-" from string
 *
 * input: 
 *   buf  : string which need to remove "-"
 *
 * output:
 *   buf  : string without "-"
 */
void undash(char* buf);
int Dev_id(char *DevID);
#endif //_vis24_h
