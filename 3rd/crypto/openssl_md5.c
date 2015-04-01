/* crypto/md5/md5_dgst.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <stdio.h>
#include "openssl_md5_local.h"

/* Implemented from RFC1321 The MD5 Message-Digest Algorithm
 */

#define INIT_DATA_A (unsigned long)0x67452301L
#define INIT_DATA_B (unsigned long)0xefcdab89L
#define INIT_DATA_C (unsigned long)0x98badcfeL
#define INIT_DATA_D (unsigned long)0x10325476L

int MD5_Init(MD5_CTX *c)
	{
	memset (c,0,sizeof(*c));
	c->A=INIT_DATA_A;
	c->B=INIT_DATA_B;
	c->C=INIT_DATA_C;
	c->D=INIT_DATA_D;
	return 1;
	}

#ifndef md5_block_data_order
#ifdef X
#undef X
#endif
void md5_block_data_order (MD5_CTX *c, const void *data_, size_t num)
	{
	const unsigned char *data=data_;
	register unsigned MD32_REG_T A,B,C,D,l;
#ifndef MD32_XARRAY
	/* See comment in crypto/sha/sha_locl.h for details. */
	unsigned MD32_REG_T	XX0, XX1, XX2, XX3, XX4, XX5, XX6, XX7,
				XX8, XX9,XX10,XX11,XX12,XX13,XX14,XX15;
# define X(i)	XX##i
#else
	MD5_LONG XX[MD5_LBLOCK];
# define X(i)	XX[i]
#endif

	A=c->A;
	B=c->B;
	C=c->C;
	D=c->D;

	for (;num--;)
		{
	HOST_c2l(data,l); X( 0)=l;		HOST_c2l(data,l); X( 1)=l;
	/* Round 0 */
	R0(A,B,C,D,X( 0), 7,0xd76aa478L);	HOST_c2l(data,l); X( 2)=l;
	R0(D,A,B,C,X( 1),12,0xe8c7b756L);	HOST_c2l(data,l); X( 3)=l;
	R0(C,D,A,B,X( 2),17,0x242070dbL);	HOST_c2l(data,l); X( 4)=l;
	R0(B,C,D,A,X( 3),22,0xc1bdceeeL);	HOST_c2l(data,l); X( 5)=l;
	R0(A,B,C,D,X( 4), 7,0xf57c0fafL);	HOST_c2l(data,l); X( 6)=l;
	R0(D,A,B,C,X( 5),12,0x4787c62aL);	HOST_c2l(data,l); X( 7)=l;
	R0(C,D,A,B,X( 6),17,0xa8304613L);	HOST_c2l(data,l); X( 8)=l;
	R0(B,C,D,A,X( 7),22,0xfd469501L);	HOST_c2l(data,l); X( 9)=l;
	R0(A,B,C,D,X( 8), 7,0x698098d8L);	HOST_c2l(data,l); X(10)=l;
	R0(D,A,B,C,X( 9),12,0x8b44f7afL);	HOST_c2l(data,l); X(11)=l;
	R0(C,D,A,B,X(10),17,0xffff5bb1L);	HOST_c2l(data,l); X(12)=l;
	R0(B,C,D,A,X(11),22,0x895cd7beL);	HOST_c2l(data,l); X(13)=l;
	R0(A,B,C,D,X(12), 7,0x6b901122L);	HOST_c2l(data,l); X(14)=l;
	R0(D,A,B,C,X(13),12,0xfd987193L);	HOST_c2l(data,l); X(15)=l;
	R0(C,D,A,B,X(14),17,0xa679438eL);
	R0(B,C,D,A,X(15),22,0x49b40821L);
	/* Round 1 */
	R1(A,B,C,D,X( 1), 5,0xf61e2562L);
	R1(D,A,B,C,X( 6), 9,0xc040b340L);
	R1(C,D,A,B,X(11),14,0x265e5a51L);
	R1(B,C,D,A,X( 0),20,0xe9b6c7aaL);
	R1(A,B,C,D,X( 5), 5,0xd62f105dL);
	R1(D,A,B,C,X(10), 9,0x02441453L);
	R1(C,D,A,B,X(15),14,0xd8a1e681L);
	R1(B,C,D,A,X( 4),20,0xe7d3fbc8L);
	R1(A,B,C,D,X( 9), 5,0x21e1cde6L);
	R1(D,A,B,C,X(14), 9,0xc33707d6L);
	R1(C,D,A,B,X( 3),14,0xf4d50d87L);
	R1(B,C,D,A,X( 8),20,0x455a14edL);
	R1(A,B,C,D,X(13), 5,0xa9e3e905L);
	R1(D,A,B,C,X( 2), 9,0xfcefa3f8L);
	R1(C,D,A,B,X( 7),14,0x676f02d9L);
	R1(B,C,D,A,X(12),20,0x8d2a4c8aL);
	/* Round 2 */
	R2(A,B,C,D,X( 5), 4,0xfffa3942L);
	R2(D,A,B,C,X( 8),11,0x8771f681L);
	R2(C,D,A,B,X(11),16,0x6d9d6122L);
	R2(B,C,D,A,X(14),23,0xfde5380cL);
	R2(A,B,C,D,X( 1), 4,0xa4beea44L);
	R2(D,A,B,C,X( 4),11,0x4bdecfa9L);
	R2(C,D,A,B,X( 7),16,0xf6bb4b60L);
	R2(B,C,D,A,X(10),23,0xbebfbc70L);
	R2(A,B,C,D,X(13), 4,0x289b7ec6L);
	R2(D,A,B,C,X( 0),11,0xeaa127faL);
	R2(C,D,A,B,X( 3),16,0xd4ef3085L);
	R2(B,C,D,A,X( 6),23,0x04881d05L);
	R2(A,B,C,D,X( 9), 4,0xd9d4d039L);
	R2(D,A,B,C,X(12),11,0xe6db99e5L);
	R2(C,D,A,B,X(15),16,0x1fa27cf8L);
	R2(B,C,D,A,X( 2),23,0xc4ac5665L);
	/* Round 3 */
	R3(A,B,C,D,X( 0), 6,0xf4292244L);
	R3(D,A,B,C,X( 7),10,0x432aff97L);
	R3(C,D,A,B,X(14),15,0xab9423a7L);
	R3(B,C,D,A,X( 5),21,0xfc93a039L);
	R3(A,B,C,D,X(12), 6,0x655b59c3L);
	R3(D,A,B,C,X( 3),10,0x8f0ccc92L);
	R3(C,D,A,B,X(10),15,0xffeff47dL);
	R3(B,C,D,A,X( 1),21,0x85845dd1L);
	R3(A,B,C,D,X( 8), 6,0x6fa87e4fL);
	R3(D,A,B,C,X(15),10,0xfe2ce6e0L);
	R3(C,D,A,B,X( 6),15,0xa3014314L);
	R3(B,C,D,A,X(13),21,0x4e0811a1L);
	R3(A,B,C,D,X( 4), 6,0xf7537e82L);
	R3(D,A,B,C,X(11),10,0xbd3af235L);
	R3(C,D,A,B,X( 2),15,0x2ad7d2bbL);
	R3(B,C,D,A,X( 9),21,0xeb86d391L);

	A = c->A += A;
	B = c->B += B;
	C = c->C += C;
	D = c->D += D;
		}
	}
#endif

#include "../../core/snmp.h"

void MD5_key(const unsigned char *password,  /* IN */
             unsigned int passwordlen,       /* IN */
             const unsigned char *engineID,  /* IN  - pointer to snmpEngineID  */
             unsigned int engineLength,      /* IN  - length of snmpEngineID */
             unsigned char *key)             /* OUT - pointer to caller 16-octet buffer */
{
  MD5_CTX            MD;
  unsigned char     *cp, password_buf[MD5_HASHKEYLEN];
  unsigned long      password_index = 0;
  unsigned long      count = 0, i;

  MD5_Init (&MD);   /* initialize MD5_ */

  /**********************************************/
  /* Use while loop until we've done 1 Megabyte */
  /**********************************************/
  while (count < 1048576) {
    cp = password_buf;
    for (i = 0; i < MD5_HASHKEYLEN; i++) {
        /*************************************************/
        /* Take the next octet of the password, wrapping */
        /* to the beginning of the password as necessary.*/
        /*************************************************/
        *cp++ = password[password_index++ % passwordlen];
    }
    MD5_Update (&MD, password_buf, MD5_HASHKEYLEN);
    count += MD5_HASHKEYLEN;
  }
  MD5_Final (key, &MD);          /* tell MD5_ we're done */

  /*****************************************************/
  /* Now localize the key with the engineID and pass   */
  /* through MD5 to produce final key                  */
  /* May want to ensure that engineLength <= 32,       */
  /* otherwise need to use a buffer larger than 64     */
  /*****************************************************/
  memcpy(password_buf, key, MD5_SECRETKEYLEN);
  memcpy(password_buf+MD5_SECRETKEYLEN, engineID, engineLength);
  memcpy(password_buf+MD5_SECRETKEYLEN+engineLength, key, MD5_SECRETKEYLEN);

  MD5_Init(&MD);
  MD5_Update(&MD, password_buf, 2*MD5_SECRETKEYLEN+engineLength);
  MD5_Final(key, &MD);
  return;
}

/* These functions are basically copies of the MDSign() routine in
   md5.c modified to be used with the OpenSSL hashing functions.  The
   copyright below is from the md5.c file that these functions were
   taken from: */

/*
 * ** **************************************************************************
 * ** md5.c -- Implementation of MD5 Message Digest Algorithm                 **
 * ** Updated: 2/16/90 by Ronald L. Rivest                                    **
 * ** (C) 1990 RSA Data Security, Inc.                                        **
 * ** **************************************************************************
 */

/*
 * MD5_hmac(data, len, MD5): do a checksum on an arbirtrary amount
 * of data, and prepended with a secret in the standard fashion 
 */
int
MD5_hmac(const unsigned char *data,    /* IN  - pointer to message */
         size_t len,                   /* IN  - length of messege */
         unsigned char *mac,           /* OUT - pointer to caller 20-octet buffer */
         size_t maclen,                /* IN  - length of mac */
         const unsigned char *secret,  /* IN  - pointer to usrAuthKey */
         size_t secretlen)             /* IN  - length of usrAuthKey */
{
  MD5_CTX                MD;
  unsigned char          K1[MD5_HASHKEYLEN];
  unsigned char          K2[MD5_HASHKEYLEN];
  unsigned char          extendedAuthKey[MD5_HASHKEYLEN];
  unsigned char          buf[MD5_HASHKEYLEN];
  size_t                 i;
  const unsigned char   *cp;
  unsigned char         *newdata = NULL;
  int                    rc = 0;

  if (secretlen != MD5_SECRETKEYLEN || secret == NULL ||
      mac == NULL || data == NULL ||
      len <= 0 || maclen <= 0) {
      /* MD5 signing not properly initialized */
      return -1;
  }

  memset(extendedAuthKey, 0, MD5_HASHKEYLEN);
  memcpy(extendedAuthKey, secret, secretlen);
  for (i = 0; i < MD5_HASHKEYLEN; i++) {
      K1[i] = extendedAuthKey[i] ^ 0x36;
      K2[i] = extendedAuthKey[i] ^ 0x5c;
  }

  MD5_Init(&MD);
  rc = !MD5_Update(&MD, K1, MD5_HASHKEYLEN);
  if (rc)
      goto update_end;

  i = len;
  if (((uintptr_t) data) % sizeof(long) != 0) {
      /*
       * this relies on the ability to use integer math and thus we
       * must rely on data that aligns on 32-bit-word-boundries 
       */
      newdata = malloc(len);
      if (newdata)
          memcpy(newdata, data, len);
      cp = newdata;
  } else {
      cp = data;
  }

  while (i >= MD5_HASHKEYLEN) {
      rc = !MD5_Update(&MD, cp, MD5_HASHKEYLEN);
      if (rc)
          goto update_end;
      cp += MD5_HASHKEYLEN;
      i -= MD5_HASHKEYLEN;
  }

  rc = !MD5_Update(&MD, cp, i);
  if (rc)
      goto update_end;

  memset(buf, 0, MD5_HASHKEYLEN);
  MD5_Final(buf, &MD);

  MD5_Init(&MD);
  rc = !MD5_Update(&MD, K2, MD5_HASHKEYLEN);
  if (rc)
      goto update_end;
  rc = !MD5_Update(&MD, buf, MD5_SECRETKEYLEN);
  if (rc)
      goto update_end;

  /*
   * copy the sign checksum to the outgoing pointer 
   */
  MD5_Final(buf, &MD);
  memcpy(mac, buf, maclen);

update_end:

  if (newdata)
      free(newdata);

  return rc;
}
