/*
 * Copyright (c) 2002, 2003 Bob Deblier
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

/*!\file aes.h
 * \brief AES block cipher, as specified by NIST FIPS 197.
 * \author Bob Deblier <bob.deblier@telenet.be>
 * \ingroup BC_m BC_aes_m
 */

#ifndef _AES_H
#define _AES_H

#include <sys/types.h>

typedef u_int8_t byte;

struct _aesParam
{
	/*!\var k
	 * \brief Holds the key expansion.
	 */
	u_int32_t k[64];
	/*!\var nr
	 * \brief Number of rounds to be used in encryption/decryption.
	 */
	u_int32_t nr;
	/*!\var fdback
	 * \brief Buffer to be used by block chaining or feedback modes.
	 */
	u_int32_t fdback[4];
};

typedef struct _aesParam aesParam;

/*!\var aes
 * \brief Holds the full API description of the AES algorithm.
 */
//extern const  blockCipher aes;

/*!\fn int aesSetup(aesParam* ap, const byte* key, size_t keybits, cipherOperation op)
 * \brief This function performs the cipher's key expansion.
 * \param ap The cipher's parameter block.
 * \param key The key value.
 * \param keybits The number of bits in the key; legal values are:
 *  128, 192 and 256.
 * \param op ENCRYPT or DECRYPT.
 * \retval 0 on success.
 * \retval -1 on failure.
 */

typedef enum
{
	NOCRYPT,
	ENCRYPT,
	DECRYPT
} cipherOperation;

int key_setup(aesParam *param, cipherOperation op);
int aes_encrypt(aesParam *key_param, char *dst, int max_dstlen, char *src);
int aes_decrypt(aesParam *key_param, char *dst, int dst_maxlen, char *src);

#endif
