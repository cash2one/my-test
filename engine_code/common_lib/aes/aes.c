/*
 * Copyright (c) 2002, 2003, 2009 Bob Deblier
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*!\file aes.c
 * \brief AES block cipher, as specified by NIST FIPS 197.
 *
 * The table lookup method was inspired by Brian Gladman's AES implementation,
 * which is much more readable than the standard code.
 *
 * \author Bob Deblier <bob.deblier@telenet.be>
 * \ingroup BC_aes_m BC_m
 */

#define BEECRYPT_DLL_EXPORT

#ifdef OPTIMIZE_MMX
# include <mmintrin.h>
#endif

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "aes.h"

#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
# if (BYTE_ORDER != BIG_ENDIAN) && (BYTE_ORDER != LITTLE_ENDIAN)
#  error unsupported endian-ness.
# endif
#endif

#if WORDS_BIGENDIAN
# include "aes_be.h"
#else
#  include "aes_le.h"
#endif

//#define AES_DEBUG

#ifdef AES_DEBUG
	#define AES_DUMP(...) \
		printf(__VA_ARGS__);
	#define AES_ERROR(...) \
		fprintf(stderr, __VA_ARGS__);

#if 0
#define REVERSE_CHAR(ch) \
		(!isascii(ch) || isprint (ch) ? (ch) : '.')
#else
#define REVERSE_CHAR(ch) \
		(isprint (ch) ? (ch) : '.')
#endif
	void print_buf2(char *buf, int nbuf, int column)
	{
		int i = 0, j = 0;
		int col = 0;
		char ch = 0;
		int first_line = 1;
		char *begin = NULL;

		if (buf == NULL || nbuf <= 0) {
			return;
		}

		if (column == 0) {
			col = 16;
		} else {
			col = column;
		}

		for (i = 0; i < nbuf; i++) {
			if (first_line) {
				printf("%08xh: ", i);
				first_line = 0;
			}

			printf("%x%x ", (buf[i] >> 4) & 0xf, buf[i] & 0xf);
			if ((i + 1) % col == 0) {
				printf("; ");
				begin = buf + i - (col - 1);
				for (j = 0; j < col; j++) {
					ch = *(begin + j);
					printf("%c", REVERSE_CHAR(ch));
				}
				printf("\n");
				first_line = 1;
			}
		}

		if (first_line == 1) {
			return;
		}

		for (j = 0; j < col - i % col; j++)
			printf("   ");

		printf("; ");

		for (j = 0; j < i % col; j++) {
			ch = *(buf + i + j - i % col);
			printf("%c", REVERSE_CHAR(ch));
		}

		printf("\n");

		return;
	}
#else
	#define AES_DUMP(...)
	#define AES_ERROR(...)
	#define print_buf2(buf, nbuf, column)
#endif

static int aesSetup(aesParam* ap, const byte* key, size_t keybits, cipherOperation op)
{
	if ((op != ENCRYPT) && (op != DECRYPT))
		return -1;

	if (((keybits & 63) == 0) && (keybits >= 128) && (keybits <= 256))
	{
		register u_int32_t* rk, t, i, j;

		/* clear fdback/iv */
		ap->fdback[0] = 0;
		ap->fdback[1] = 0;
		ap->fdback[2] = 0;
		ap->fdback[3] = 0;

		ap->nr = 6 + (keybits >> 5);

		rk = ap->k;

		memcpy(rk, key, keybits >> 3);

		i = 0;

		if (keybits == 128)
		{
			while (1)
			{
				t = rk[3];
				#if WORDS_BIGENDIAN
				t = (_ae4[(t >> 16) & 0xff] & 0xff000000) ^
					(_ae4[(t >>  8) & 0xff] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x000000ff) ^
					 _arc[i];
				#else
				t = (_ae4[(t >>  8) & 0xff] & 0x000000ff) ^
					(_ae4[(t >> 16) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0xff000000) ^
					 _arc[i];
				#endif
				rk[4] = (t ^= rk[0]);
				rk[5] = (t ^= rk[1]);
				rk[6] = (t ^= rk[2]);
				rk[7] = (t ^= rk[3]);
				if (++i == 10)
					break;
				rk += 4;
			}
		}
		else if (keybits == 192)
		{
			while (1)
			{
				t = rk[5];
				#if WORDS_BIGENDIAN
				t = (_ae4[(t >> 16) & 0xff] & 0xff000000) ^
					(_ae4[(t >>  8) & 0xff] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x000000ff) ^
					 _arc[i];
				#else
				t = (_ae4[(t >>  8) & 0xff] & 0x000000ff) ^
					(_ae4[(t >> 16) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0xff000000) ^
					 _arc[i];
				#endif
				rk[6] = (t ^= rk[0]);
				rk[7] = (t ^= rk[1]);
				rk[8] = (t ^= rk[2]);
				rk[9] = (t ^= rk[3]);
				if (++i == 8)
					break;
				rk[10] = (t ^= rk[4]);
				rk[11] = (t ^= rk[5]);
				rk += 6;
			}
		}
		else if (keybits == 256)
		{
			while (1)
			{
				t = rk[7];
				#if WORDS_BIGENDIAN
				t = (_ae4[(t >> 16) & 0xff] & 0xff000000) ^
					(_ae4[(t >>  8) & 0xff] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x000000ff) ^
					 _arc[i];
				#else
				t = (_ae4[(t >>  8) & 0xff] & 0x000000ff) ^
					(_ae4[(t >> 16) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 24)       ] & 0x00ff0000) ^
					(_ae4[(t      ) & 0xff] & 0xff000000) ^
					 _arc[i];
				#endif
				rk[8] = (t ^= rk[0]);
				rk[9] = (t ^= rk[1]);
				rk[10] = (t ^= rk[2]);
				rk[11] = (t ^= rk[3]);
				if (++i == 7)
					break;
				#if WORDS_BIGENDIAN
				t = (_ae4[(t >> 24)       ] & 0xff000000) ^
					(_ae4[(t >> 16) & 0xff] & 0x00ff0000) ^
					(_ae4[(t >>  8) & 0xff] & 0x0000ff00) ^
					(_ae4[(t      ) & 0xff] & 0x000000ff);
				#else
				t = (_ae4[(t      ) & 0xff] & 0x000000ff) ^
					(_ae4[(t >>  8) & 0xff] & 0x0000ff00) ^
					(_ae4[(t >> 16) & 0xff] & 0x00ff0000) ^
					(_ae4[(t >> 24)       ] & 0xff000000);
				#endif
				rk[12] = (t ^= rk[4]);
				rk[13] = (t ^= rk[5]);
				rk[14] = (t ^= rk[6]);
				rk[15] = (t ^= rk[7]);
				rk += 8;
			}
		}

		if (op == DECRYPT)
		{
			rk = ap->k;

			for (i = 0, j = (ap->nr << 2); i < j; i += 4, j -= 4)
			{
				t = rk[i  ]; rk[i  ] = rk[j  ]; rk[j  ] = t;
				t = rk[i+1]; rk[i+1] = rk[j+1]; rk[j+1] = t;
				t = rk[i+2]; rk[i+2] = rk[j+2]; rk[j+2] = t;
				t = rk[i+3]; rk[i+3] = rk[j+3]; rk[j+3] = t;
			}
			for (i = 1; i < ap->nr; i++)
			{
				rk += 4;
				#if WORDS_BIGENDIAN
				rk[0] =
					_ad0[_ae4[(rk[0] >> 24)       ] & 0xff] ^
					_ad1[_ae4[(rk[0] >> 16) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[0] >>  8) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[0]      ) & 0xff] & 0xff];
				rk[1] =
					_ad0[_ae4[(rk[1] >> 24)       ] & 0xff] ^
					_ad1[_ae4[(rk[1] >> 16) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[1] >>  8) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[1]      ) & 0xff] & 0xff];
				rk[2] =
					_ad0[_ae4[(rk[2] >> 24)       ] & 0xff] ^
					_ad1[_ae4[(rk[2] >> 16) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[2] >>  8) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[2]      ) & 0xff] & 0xff];
				rk[3] =
					_ad0[_ae4[(rk[3] >> 24)       ] & 0xff] ^
					_ad1[_ae4[(rk[3] >> 16) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[3] >>  8) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[3]      ) & 0xff] & 0xff];
				#else
				rk[0] =
					_ad0[_ae4[(rk[0]      ) & 0xff] & 0xff] ^
					_ad1[_ae4[(rk[0] >>  8) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[0] >> 16) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[0] >> 24)       ] & 0xff];
				rk[1] =
					_ad0[_ae4[(rk[1]      ) & 0xff] & 0xff] ^
					_ad1[_ae4[(rk[1] >>  8) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[1] >> 16) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[1] >> 24)       ] & 0xff];
				rk[2] =
					_ad0[_ae4[(rk[2]      ) & 0xff] & 0xff] ^
					_ad1[_ae4[(rk[2] >>  8) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[2] >> 16) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[2] >> 24)       ] & 0xff];
				rk[3] =
					_ad0[_ae4[(rk[3]      ) & 0xff] & 0xff] ^
					_ad1[_ae4[(rk[3] >>  8) & 0xff] & 0xff] ^
					_ad2[_ae4[(rk[3] >> 16) & 0xff] & 0xff] ^
					_ad3[_ae4[(rk[3] >> 24)       ] & 0xff];
				#endif
			}
		}
		return 0;
	}
	return -1;
}

#ifndef ASM_AESENCRYPT
static int aesEncrypt(aesParam* ap, u_int32_t* dst, const u_int32_t* src)
{
	#if defined (OPTIMIZE_MMX) && (defined(OPTIMIZE_I586) || defined(OPTIMIZE_I686))
	register __m64 s0, s1, s2, s3;
	register __m64 t0, t1, t2, t3;
	register u_int32_t i0, i1, i2, i3;
	#else
	register u_int32_t s0, s1, s2, s3;
	register u_int32_t t0, t1, t2, t3;
	#endif
	register u_int32_t* rk = ap->k;

	#if defined (OPTIMIZE_MMX) && (defined(OPTIMIZE_I586) || defined(OPTIMIZE_I686))
	s0 = _mm_cvtsi32_si64(src[0] ^ rk[0]);
	s1 = _mm_cvtsi32_si64(src[1] ^ rk[1]);
	s2 = _mm_cvtsi32_si64(src[2] ^ rk[2]);
	s3 = _mm_cvtsi32_si64(src[3] ^ rk[3]);
	#else
	s0 = src[0] ^ rk[0];
	s1 = src[1] ^ rk[1];
	s2 = src[2] ^ rk[2];
	s3 = src[3] ^ rk[3];
	#endif

	etfs(4);		/* round 1 */
	esft(8);		/* round 2 */
	etfs(12);		/* round 3 */
	esft(16);		/* round 4 */
	etfs(20);		/* round 5 */
	esft(24);		/* round 6 */
	etfs(28);		/* round 7 */
	esft(32);		/* round 8 */
	etfs(36);		/* round 9 */

	if (ap->nr > 10)
	{
		esft(40);	/* round 10 */
		etfs(44);	/* round 11 */
		if (ap->nr > 12)
		{
			esft(48);	/* round 12 */
			etfs(52);	/* round 13 */
		}
	}

	rk += (ap->nr << 2);

	elr(); /* last round */

	#if defined(OPTIMIZE_MMX) && (defined(OPTIMIZE_I586) || defined(OPTIMIZE_I686))
	dst[0] = _mm_cvtsi64_si32(s0);
	dst[1] = _mm_cvtsi64_si32(s1);
	dst[2] = _mm_cvtsi64_si32(s2);
	dst[3] = _mm_cvtsi64_si32(s3);
	#else
	dst[0] = s0;
	dst[1] = s1;
	dst[2] = s2;
	dst[3] = s3;
	#endif

	return 0;
}
#endif

#ifndef ASM_AESDECRYPT
static int aesDecrypt(aesParam* ap, u_int32_t* dst, const u_int32_t* src)
{
	register u_int32_t s0, s1, s2, s3;
	register u_int32_t t0, t1, t2, t3;
	register u_int32_t* rk = ap->k;

	s0 = src[0] ^ rk[0];
	s1 = src[1] ^ rk[1];
	s2 = src[2] ^ rk[2];
	s3 = src[3] ^ rk[3];

	dtfs(4);		/* round 1 */
	dsft(8);		/* round 2 */
	dtfs(12);		/* round 3 */
	dsft(16);		/* round 4 */
	dtfs(20);		/* round 5 */
	dsft(24);		/* round 6 */
	dtfs(28);		/* round 7 */
	dsft(32);		/* round 8 */
	dtfs(36);		/* round 9 */

	if (ap->nr > 10)
	{
		dsft(40);	/* round 10 */
		dtfs(44);	/* round 11 */
		if (ap->nr > 12)
		{
			dsft(48);	/* round 12 */
			dtfs(52);	/* round 13 */
		}
	}

	rk += (ap->nr << 2);

	dlr(); /* last round */

	dst[0] = s0;
	dst[1] = s1;
	dst[2] = s2;
	dst[3] = s3;

	return 0;
}
#endif

int key_setup(aesParam *param, cipherOperation op)
{
	//char *str_key = "1234567890123456";
	char *str_key = "1@3$qWeR1@3$qWeR";
	//char *str_key = "venus_sign_event_307_encrypt_key";
	printf("key=%s\n", str_key);

#if 0
	aesParam *param = calloc(1, sizeof(aesParam));
	if (param == NULL) {
		goto err;
	}
#endif

	int keybits = strlen(str_key) << 3;
	if (aesSetup(param, (byte *)str_key, keybits, op) < 0) {
		goto err;
	}

	AES_DUMP("%s\n", str_key);
	AES_DUMP("keybits: %d\n", keybits);

	return 0;

err:
#if 0
	if (param) {
		free(param);
		param = NULL;
	}
#endif

	return -1;
}

#define hextochar(c) \
        (((c) < 10) ? (c) + '0' : (c) + 'a' - 10)

static inline int aes_bin2hex(const char *begin, size_t len, char *hex_str, size_t max_hexlen)
{
	unsigned char *data = (unsigned char *)begin;

	if (max_hexlen < (len * 2)) {
		AES_ERROR("error: len(%ld) is bigger than max_hexlen(%ld)\n", len, max_hexlen);
		return 0;
	}

	size_t i = 0;
	unsigned char high_char = 0;
	unsigned char lower_char = 0;
	int j = 0;
	for (i = 0; i < len; i++) {
		high_char = (data[i] >> 4) & 0xf;
		lower_char = data[i] & 0xf;

		hex_str[j++] = hextochar(high_char);
		hex_str[j++] = hextochar(lower_char);
	}

	data[j] = '\0';

	return j;
}

static int fromhex(unsigned char *data, const char *hexdata)
{
	int length = strlen(hexdata);
	int count = 0, index = 0;
	byte b = 0;
	char ch;

	if (length & 1)
		count = 1;

	while (index++ < length)
	{
		ch = *(hexdata++);

		b <<= 4;
		if (ch >= '0' && ch <= '9')
			b += (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			b += (ch - 'A') + 10;
		else if (ch >= 'a' && ch <= 'f')
			b += (ch - 'a') + 10;

		count++;
		if (count == 2)
		{
			*(data++) = b;
			b = 0;
			count = 0;
		}
	}
	return (length+1) >> 1;
}

int aes_encrypt(aesParam *key_param, char *dst, int max_dstlen, char *src)
{
	int src_len = strlen(src);
	int src_block_len = (src_len >> 4) + (src_len & 15 ? 1 : 0);

	int dst_bin_len = (src_block_len << 4);
	char *dst_bin = calloc(dst_bin_len, 1);
	if (dst_bin == NULL) {
		return 0;
	}

	int dst_len = dst_bin_len * 2;
	if (dst_len >= max_dstlen) {
		goto err;
	}

	int idx = 0;
	for (idx = 0; idx < src_block_len; idx++) {
		aesEncrypt(key_param, (u_int32_t*) (dst_bin + (idx << 4)), 
		    	  	    (u_int32_t*) (src + (idx << 4)));
	}

	AES_DUMP("enc dst_bin:%d\n", dst_bin_len);
//printf("===========================\n");
//	print_buf2(dst_bin, dst_bin_len, 0);

	aes_bin2hex(dst_bin, dst_bin_len, dst, max_dstlen);
	AES_DUMP("enc dst:(%d:%ld) %s\n", dst_len, strlen(dst), dst);

	free(dst_bin);
	return (dst_len);

err:
	if (dst_bin) {
		free(dst_bin);
		dst_bin = NULL;
	}

	return 0;
}

int aes_decrypt(aesParam *key_param, char *dst, int dst_maxlen, char *src)
{
	int src_bin_len = (strlen(src) >> 1);

	int src_block_len = (src_bin_len >> 4) + (src_bin_len & 15 ? 1 : 0);
	src_bin_len = (src_block_len << 4);

	if (src_bin_len > dst_maxlen) {
		return 0;
	}

	char *src_bin = calloc(src_bin_len, 1);
	if (src_bin == NULL) {
		return 0;
	}

	AES_DUMP("dec src: %s\n", src);
	fromhex((unsigned char *)src_bin, src);

	int idx = 0;
	for (idx = 0; idx < src_block_len; idx++) {
		aesDecrypt(key_param, (u_int32_t*) (dst + (idx << 4)), 
			  	      (u_int32_t*) (src_bin + (idx << 4)));
	}

	free(src_bin);

	return 0;
}

