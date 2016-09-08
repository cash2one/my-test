/*********************************************************
 *		     RC6加密程序			 *
 *		    	郑红学				 *
 *		     2000.12.25				 *
 *********************************************************/
#include <string.h>
#include <endian.h>
#include <stdio.h>

#include "misc.h"

/* Rotations */
#define ROTL(x,y) (((x)<<((y)&(RC6_WORD_SIZE-1))) | ((x)>>(RC6_WORD_SIZE-((y)&(RC6_WORD_SIZE-1)))))
#define ROTR(x,y) (((x)>>((y)&(RC6_WORD_SIZE-1))) | ((x)<<(RC6_WORD_SIZE-((y)&(RC6_WORD_SIZE-1)))))

unsigned int S_RC6_S[RC6_R24] = {0};	/* Key schedule */

static int cvttext(unsigned char p)
{
	int ret = 0;

	if ((p >= '0') && (p <= '9'))
		ret = p - '0';
	else if ((p >= 'a') && (p <= 'f'))
		ret = p - 'a' + 10;
	else if ((p >= 'A') && (p <= 'F'))
		ret = p - 'A' + 10;
	else
		ret = p;

	return ret;
}

static int asc2hex(unsigned char *src, int slen, unsigned char *dst)
{
	unsigned char *p = src;
	int cnt = 0;

	while (slen > 0) {
		dst[cnt] = (cvttext(p[0]) * 16 + cvttext(p[1]));
		slen -= 2;
		cnt++;
		p = p + 2;
	}

	return cnt;
}

static int hex2asc(unsigned char *inbuf, unsigned char *outbuf, int len)
{
	int i = 0;
	char *p = (char *)outbuf;

	for (i = 0; i < len; i++) {
		sprintf(p, "%02X", inbuf[i]);
		p += 2;
	}
	return (len << 1);
}

static int change_byte(unsigned char *output_char, unsigned char *input_char)
{
#if __BYTE_ORDER == __BIG_ENDIAN
	output_char[0] = input_char[3];
	output_char[1] = input_char[2];
	output_char[2] = input_char[1];
	output_char[3] = input_char[0];
	output_char[4] = input_char[7];
	output_char[5] = input_char[6];
	output_char[6] = input_char[5];
	output_char[7] = input_char[4];
	output_char[8] = input_char[11];
	output_char[9] = input_char[10];
	output_char[10] = input_char[9];
	output_char[11] = input_char[8];
	output_char[12] = input_char[15];
	output_char[13] = input_char[14];
	output_char[14] = input_char[13];
	output_char[15] = input_char[12];
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	memcpy(output_char, input_char, 16);
#endif
	return 1;
}

/************************************************
 *		密钥的初始化			*
 *	K：输入参数，数据加密和解密的密钥。	*
 *	b：输入参数，密钥长度。			*
 *************************************************/
void rc6_key_setup_3(unsigned char *K, int b, unsigned int *rc6_s)
{
	int i, j, s, v;
	unsigned int L[(32 + RC6_BYTES - 1) / RC6_BYTES];	/* Big enough for max b */
	unsigned int A, B;

	memset(rc6_s, 0, RC6_R24 * sizeof(unsigned int));

	L[RC6_C - 1] = 0;
	for (i = b - 1; i >= 0; i--)
		L[i / RC6_BYTES] = (L[i / RC6_BYTES] << 8) + K[i];

	rc6_s[0] = RC6_P32;
	for (i = 1; i <= 2 * RC6_R + 3; i++)
		rc6_s[i] = rc6_s[i - 1] + RC6_Q32;

	A = B = i = j = 0;
	v = RC6_R24;
	if (RC6_C > v)
		v = RC6_C;
	v *= 3;
	for (s = 1; s <= v; s++) {
		A = rc6_s[i] = ROTL(rc6_s[i] + A + B, 3);
		B = L[j] = ROTL(L[j] + A + B, A + B);
		i = (i + 1) % RC6_R24;
		j = (j + 1) % RC6_C;
	}
}

/************************************************
 *		加密函数，一次加密16个字节	*
 *	pt：输入参数，明文。			*
 *	ct：输出参数，密文。			*
 *************************************************/
static void rc6_block_encrypt(unsigned int *pt, unsigned int *ct, unsigned int *rc6_s)
{
	unsigned int A, B, C, D, t, u, x;
	int i;

	A = pt[0];
	B = pt[1];
	C = pt[2];
	D = pt[3];
	B += rc6_s[0];
	D += rc6_s[1];
	for (i = 2; i <= 2 * RC6_R; i += 2) {
		t = ROTL(B * (2 * B + 1), RC6_LGW);
		u = ROTL(D * (2 * D + 1), RC6_LGW);
		A = ROTL(A ^ t, u) + rc6_s[i];
		C = ROTL(C ^ u, t) + rc6_s[i + 1];
		x = A;
		A = B;
		B = C;
		C = D;
		D = x;
	}
	A += rc6_s[2 * RC6_R + 2];
	C += rc6_s[2 * RC6_R + 3];
	ct[0] = A;
	ct[1] = B;
	ct[2] = C;
	ct[3] = D;
}

/************************************************
 *		解密函数，一次解密16个字节      *
 *       pt：输入参数，密文。                    *
 *       ct：输出参数，明文。                    *
 *************************************************/
static void rc6_block_decrypt(unsigned int *ct, unsigned int *pt, unsigned int *rc6_s)
{
	unsigned int A, B, C, D, t, u, x;
	int i;

	A = ct[0];
	B = ct[1];
	C = ct[2];
	D = ct[3];
	C -= rc6_s[2 * RC6_R + 3];
	A -= rc6_s[2 * RC6_R + 2];
	for (i = 2 * RC6_R; i >= 2; i -= 2) {
		x = D;
		D = C;
		C = B;
		B = A;
		A = x;
		u = ROTL(D * (2 * D + 1), RC6_LGW);
		t = ROTL(B * (2 * B + 1), RC6_LGW);
		C = ROTR(C - rc6_s[i + 1], t) ^ u;
		A = ROTR(A - rc6_s[i], u) ^ t;
	}
	D -= rc6_s[1];
	B -= rc6_s[0];
	pt[0] = A;
	pt[1] = B;
	pt[2] = C;
	pt[3] = D;
}

/************************************************
 *		加密函数		    	*
 *	pbuf：输入参数，明文。			*
 *	cbuf：输出参数，密文。			*
 *	length：加密块数据长度(16字节倍数)。	*
 *************************************************/
int rc6_encrypt_4(void *pbuf, void *cbuf, int length, unsigned int *rc6_s)
{
	int j = 0;
	unsigned int *pp, *pc, ct[4], pt[4];

	pp = (unsigned int *) pbuf;
	pc = (unsigned int *) cbuf;

	for (j = 0; j < length; j = j + 16) {
		change_byte((unsigned char *) pt, (unsigned char *) pp);
		rc6_block_encrypt(pt, ct, rc6_s);
		change_byte((unsigned char *) pc, (unsigned char *) ct);
		pp += 4;
		pc += 4;
	}

	return ((unsigned char *)pc - (unsigned char *)cbuf);
}

/************************************************
 *               解密函数                        *
 *	cbuf：输入参数，密文。			*
 *	pbuf：输出参数，明文。			*
 *	length：解密块数据长度。		*
 *************************************************/
int rc6_decrypt_4(void *cbuf, void *pbuf, int length, unsigned int *rc6_s)
{
	int j = 0;
	unsigned int *pp, *pc, ct[4], pt[4], pt1[4];

	pp = (unsigned int *) pbuf;
	pc = (unsigned int *) cbuf;
	for (j = 0; j < length; j = j + 16) {
		change_byte((unsigned char *) ct, (unsigned char *) pc);
		rc6_block_decrypt(ct, pt, rc6_s);
		change_byte((unsigned char *) pt1, (unsigned char *) pt);
		memcpy(pp, pt1, length - j > 16 ? 16 : length - j);
		pp += 4;
		pc += 4;
	}

	return ((unsigned char *)pp - (unsigned char *)pbuf);
}

int rule_decrypttion(unsigned char *mat)
{
	int ii = 0;
	unsigned char hex_match[4096] = {0};
	int dlen = 0;
	int mat_len = 0;

	mat_len = (int)strlen((char *)mat);
	dlen = asc2hex(mat, mat_len, hex_match);
	bzero(mat, mat_len);
	rc6_decrypt(hex_match, mat, dlen);
	mat_len = (int)strlen((char *)mat);

	for (ii = mat_len; ii > 0; ii--) {
		if ((mat[ii - 1] != ' ') && (mat[ii - 1] != 0x09)) {
			break;
		}
	}
	mat[ii] = '\0';

	return 0;
}

int rule_encryption(unsigned char *in, unsigned char *out, int out_maxlen)
{
#define MAX_BUF_ENCRYLEN 4096
	int len = 0;
	int ret = 0;
	int size = 0;
	int buf_len = 0;
	unsigned char buf[MAX_BUF_ENCRYLEN] = { 0 };
	unsigned char buf_in[MAX_BUF_ENCRYLEN] = { 0 };

	if (unlikely(NULL == in || NULL == out)) {
		return 0;
	}

	strncpy((char*)buf_in, (char *)in,  MAX_BUF_ENCRYLEN);
	len = strlen((char *)buf_in);

	size = (len & 0x0F);
	if (size != 0) {
		memset(buf_in + len, 0, 16 - size);
		len += (16 - size);

		if (len >= MAX_BUF_ENCRYLEN) {
			len -= 16;
		}
	}

	if (len >= (out_maxlen >> 1)) {
		return 0;
	}

	buf_len = rc6_encrypt(buf_in, buf, len);
	ret = hex2asc(buf, out, buf_len);

	return ret;
}

