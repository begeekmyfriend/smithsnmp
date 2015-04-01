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

#include "openssl_aes.h"
#include "openssl_aes_local.h"

#include "../../core/snmp.h"

/*******************************************************************
 * AES_Encrypt
 *
 * Parameters:
 *	*key		    Key bits for crypting.
 *	 keylen		  Length of key (buffer) in bytes.
 *	*iv		      IV bits for crypting.
 *	 ivlen		  Length of iv (buffer) in bytes.
 *	*plaintext	Plaintext to crypt.
 *	 ptlen		  Length of plaintext.
 *	*ciphertext	Ciphertext to crypt.
 *	*ctlen		  Length of ciphertext.
 *
 * Encrypt plaintext into ciphertext using key and iv.
 *
 * ctlen contains actual number of crypted bytes in ciphertext upon
 * successful return.
 */
void
AES_Encrypt(const unsigned char *key, unsigned int keylen,
            const unsigned char *iv, unsigned int ivlen,
            const unsigned char *plaintext, unsigned int ptlen,
            unsigned char *ciphertext, unsigned int *ctlen)
{
  unsigned char   my_iv[AES_SECRETKEYLEN * 8];
  int             new_ivlen = 0;
  AES_KEY         aes_key;

  if (!key || !iv || !plaintext || !ciphertext || !ctlen ||
      ptlen <= 0 || *ctlen <= 0 || ptlen > *ctlen ||
      keylen < AES_SECRETKEYLEN || ivlen < AES_SECRETKEYLEN) {
    return;
  }

  memset(my_iv, 0, sizeof(my_iv));
  AES_set_encrypt_key(key, AES_SECRETKEYLEN * 8, &aes_key);
  memcpy(my_iv, iv, ivlen);
  /*
   * encrypt the data 
   */
  AES_cfb128_encrypt(plaintext, ciphertext, ptlen,
                     &aes_key, my_iv, &new_ivlen, AES_ENCRYPT);
  *ctlen = ptlen;
}

/*******************************************************************
 * AES_Decrypt
 *
 * Parameters:
 *	*key		    Key bits for crypting.
 *	 keylen		  Length of key (buffer) in bytes.
 *	*iv		      IV bits for crypting.
 *	 ivlen		  Length of iv (buffer) in bytes.
 *	*ciphertext	Ciphertext to crypt.
 *	*ctlen		  Length of ciphertext.
 *	*plaintext	Plaintext to crypt.
 *	 ptlen		  Length of plaintext.
 *
 * Decrypt ciphertext into plaintext using key and iv.
 *
 * ptlen contains actual number of plaintext bytes in plaintext upon
 * successful return.
 */
void
AES_Decrypt(const unsigned char *key, unsigned int keylen,
            const unsigned char *iv, unsigned int ivlen,
            const unsigned char *ciphertext, unsigned int ctlen,
            unsigned char *plaintext, unsigned int *ptlen)
{
  unsigned char   my_iv[AES_SECRETKEYLEN * 8];
  int new_ivlen = 0;
  AES_KEY aes_key;

  if (!key || !iv || !plaintext || !ciphertext || !ptlen ||
      ctlen <= 0 || *ptlen <= 0 || ctlen > *ptlen ||
      keylen < AES_SECRETKEYLEN || ivlen < AES_SECRETKEYLEN) {
    return;
  }

  memset(my_iv, 0, sizeof(my_iv));
  memcpy(my_iv, iv, ivlen);

  /* set key */
  AES_set_encrypt_key(key, AES_SECRETKEYLEN * 8, &aes_key);
  /* encrypt the data */
  AES_cfb128_encrypt(ciphertext, plaintext, ctlen,
                     &aes_key, my_iv, &new_ivlen, AES_DECRYPT);
  *ptlen = ctlen;
}
