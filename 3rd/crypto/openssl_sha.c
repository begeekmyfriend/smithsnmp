/* crypto/sha/sha1dgst.c */
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

#undef  SHA_0
#define SHA_1

/* The implementation is in ../md32_common.h */

#include "openssl_sha_local.h"

#include "../../core/snmp.h"

void SHA1_key(const unsigned char *password,  /* IN */
              unsigned int passwordlen,       /* IN */
              const unsigned char *engineID,  /* IN  - pointer to snmpEngineID  */
              unsigned int engineLength,      /* IN  - length of snmpEngineID */
              unsigned char *key)             /* OUT - pointer to caller 20-octet buffer */
{
   SHA_CTX         SH;
   unsigned char  *cp, password_buf[72];
   unsigned long   password_index = 0;
   unsigned long   count = 0, i;

   SHA1_Init (&SH);   /* initialize SHA1_ */

   /**********************************************/
   /* Use while loop until we've done 1 Megabyte */
   /**********************************************/
   while (count < 1048576) {
      cp = password_buf;
      for (i = 0; i < SHA1_HASHKEYLEN; i++) {
          /*************************************************/
          /* Take the next octet of the password, wrapping */
          /* to the beginning of the password as necessary.*/
          /*************************************************/
          *cp++ = password[password_index++ % passwordlen];
      }
      SHA1_Update (&SH, password_buf, SHA1_HASHKEYLEN);
      count += SHA1_HASHKEYLEN;
   }
   SHA1_Final (key, &SH);          /* tell SHA1_ we're done */

   /*****************************************************/
   /* Now localize the key with the engineID and pass   */
   /* through SHA1_ to produce final key                  */
   /* May want to ensure that engineLength <= 32,       */
   /* otherwise need to use a buffer larger than 72     */
   /*****************************************************/
   memcpy(password_buf, key, SHA1_SECRETKEYLEN);
   memcpy(password_buf+SHA1_SECRETKEYLEN, engineID, engineLength);
   memcpy(password_buf+SHA1_SECRETKEYLEN+engineLength, key, SHA1_SECRETKEYLEN);

   SHA1_Init(&SH);
   SHA1_Update(&SH, password_buf, 2*SHA1_SECRETKEYLEN+engineLength);
   SHA1_Final(key, &SH);
   return;
}

int
SHA1_hmac(const unsigned char *data,    /* IN  - pointer to message */
          size_t len,                   /* IN  - length of messege */
          unsigned char *mac,           /* OUT - pointer to caller 20-octet buffer */
          size_t maclen,                /* IN  - length of mac */
          const unsigned char *secret,  /* IN  - pointer to usrAuthKey */
          size_t secretlen)             /* IN  - length of usrAuthKey */
{
  SHA_CTX         SH;
  unsigned char   K1[SHA1_HASHKEYLEN];
  unsigned char   K2[SHA1_HASHKEYLEN];
  unsigned char   extendedAuthKey[SHA1_HASHKEYLEN];
  unsigned char   buf[SHA1_HASHKEYLEN];
  size_t          i;
  const unsigned char   *cp;
  unsigned char         *newdata = NULL;
  int             rc = 0;

  if (secretlen != SHA1_SECRETKEYLEN || secret == NULL ||
      mac == NULL || data == NULL ||
      len <= 0 || maclen <= 0) {
      /*
       * DEBUGMSGTL(("sha1","SHA1 signing not properly initialized")); 
       */
      return -1;
  }

  memset(extendedAuthKey, 0, SHA1_HASHKEYLEN);
  memcpy(extendedAuthKey, secret, secretlen);
  for (i = 0; i < SHA1_HASHKEYLEN; i++) {
      K1[i] = extendedAuthKey[i] ^ 0x36;
      K2[i] = extendedAuthKey[i] ^ 0x5c;
  }

  SHA1_Init(&SH);
  rc = !SHA1_Update(&SH, K1, SHA1_HASHKEYLEN);
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

  while (i >= SHA1_HASHKEYLEN) {
      rc = !SHA1_Update(&SH, cp, SHA1_HASHKEYLEN);
      if (rc)
          goto update_end;
      cp += SHA1_HASHKEYLEN;
      i -= SHA1_HASHKEYLEN;
  }

  rc = !SHA1_Update(&SH, cp, i);
  if (rc)
      goto update_end;

  memset(buf, 0, SHA1_HASHKEYLEN);
  SHA1_Final(buf, &SH);

  SHA1_Init(&SH);
  rc = !SHA1_Update(&SH, K2, SHA1_HASHKEYLEN);
  if (rc)
      goto update_end;
  rc = !SHA1_Update(&SH, buf, SHA1_SECRETKEYLEN);
  if (rc)
      goto update_end;

  /*
   * copy the sign checksum to the outgoing pointer 
   */
  SHA1_Final(buf, &SH);
  memcpy(mac, buf, maclen);

update_end:

  if (newdata)
      free(newdata);

  return rc;
}
