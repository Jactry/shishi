/* nettle.c   shishi crypto wrappers around nettle.
 * Copyright (C) 2002, 2003  Simon Josefsson
 *
 * This file is part of Shishi.
 *
 * Shishi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Shishi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shishi; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* Note: This file is only built if Shishi uses Nettle. */

#include "internal.h"

#include "hmac.h"
#include "des.h"
#include "aes.h"
#include "cbc.h"
#include "cbc-cts.h"
#include "cbc-mac.h"

int
shishi_crypto_init (void)
{
  return SHISHI_OK;
}

/**
 * shishi_randomize:
 * @handle: shishi handle as allocated by shishi_init().
 * @data: output array to be filled with random data.
 * @datalen: size of output array.
 *
 * Store cryptographically strong random data of given size in the
 * provided buffer.
 *
 * Return value: Returns %SHISHI_OK iff successful.
 **/
int
shishi_randomize (Shishi * handle, char *data, size_t datalen)
{
  int fd;
  char *device;
  size_t len = 0;
  int rc;

  device = "/dev/random";

  fd = open (device, O_RDONLY);
  if (fd < 0)
    {
      shishi_error_printf (handle, "Could not open random device: %s",
			   strerror (errno));
      return SHISHI_FILE_ERROR;
    }

  do
    {
      ssize_t tmp;

      tmp = read (fd, data, datalen);

      if (tmp < 0)
	{
	  shishi_error_printf (handle, "Could not read from random device: %s",
			       strerror (errno));
	  return SHISHI_FILE_ERROR;
	}

      len += tmp;

      if (len < datalen)
	shishi_error_printf (handle, "Short read from random device: %d < %d",
			     len, datalen);
    }
  while (len < datalen);

  rc = close (fd);
  if (rc < 0)
    shishi_warn (handle, "Could not close random device: %s",
		 strerror (errno));

  return SHISHI_OK;
}

int
shishi_md4 (Shishi * handle,
	    const char *in, size_t inlen,
	    char **out, size_t *outlen)
{
  struct md4_ctx md4;

  md4_init (&md4);
  md4_update (&md4, inlen, in);
  *outlen = MD4_DIGEST_SIZE;
  *out = xmalloc (*outlen);
  md4_digest (&md4, *outlen, *out);

  return SHISHI_OK;
}

int
shishi_md5 (Shishi * handle,
	    const char *in, size_t inlen,
	    char **out, size_t *outlen)
{
  struct md5_ctx md5;

  md5_init (&md5);
  md5_update (&md5, inlen, in);
  *outlen = MD5_DIGEST_SIZE;
  *out = xmalloc (*outlen);
  md5_digest (&md5, *outlen, *out);

  return SHISHI_OK;
}


int
shishi_hmac_sha1 (Shishi * handle,
		  const char *key, size_t keylen,
		  const char *in, size_t inlen,
		  char **outhash, size_t * outhashlen)
{
  struct hmac_sha1_ctx ctx;

  hmac_sha1_set_key (&ctx, keylen, key);
  hmac_sha1_update (&ctx, inlen, in);
  *outhashlen = SHA1_DIGEST_SIZE;
  *outhash = xmalloc (*outhashlen);
  hmac_sha1_digest (&ctx, *outhashlen, *outhash);

  return SHISHI_OK;
}

int
shishi_des (Shishi * handle, int decryptp,
	    const char *key, size_t keylen,
	    const char *iv, size_t ivlen,
	    char **ivout, size_t * ivoutlen,
	    const char *in, size_t inlen,
	    char **out, size_t * outlen)
{
  struct CBC_CTX (struct des_ctx, DES_BLOCK_SIZE) des;
  int rc;

  *outlen = inlen;
  *out = xmalloc (*outlen);

  rc = des_set_key (&des.ctx, key);
  if (!rc)
    {
      shishi_error_printf (handle, "Nettle setkey failed");
      return SHISHI_CRYPTO_INTERNAL_ERROR;
    }

  memset (des.iv, 0, sizeof (des.iv));
  /* XXX Use CBC_SET_IV (&des, iv), but how with ivlen? */
  memcpy (des.iv, iv, ivlen < sizeof (des.iv) ? ivlen : sizeof (des.iv));

  if (decryptp)
    CBC_DECRYPT (&des, des_decrypt, inlen, *out, in);
  else
    CBC_ENCRYPT (&des, des_encrypt, inlen, *out, in);

  if (ivout && ivoutlen)
    {
      *ivoutlen = sizeof (des.iv);
      *ivout = xmemdup (des.iv, *ivoutlen);
    }

  return SHISHI_OK;
}

int
shishi_des_cbc_mac (Shishi * handle,
		    const char key[8],
		    const char iv[8],
		    const char *in, size_t inlen,
		    char *out[8])
{
  struct CBC_MAC_CTX (struct des_ctx, DES_BLOCK_SIZE) des;
  int rc;

  rc = des_set_key (&des.ctx, key);
  if (!rc)
    {
      shishi_error_printf (handle, "Nettle des_set_key() failed (%d)", rc);
      return SHISHI_CRYPTO_INTERNAL_ERROR;
    }

  if (iv)
    CBC_SET_IV (&des, iv);
  else
    memset (des.iv, 0, DES_BLOCK_SIZE);

  *out = xmalloc (8);

  CBC_MAC (&des, des_encrypt, inlen, *out, in);

  return SHISHI_OK;
}

int
shishi_3des (Shishi * handle, int decryptp,
	     const char *key, size_t keylen,
	     const char *iv, size_t ivlen,
	     char **ivout, size_t * ivoutlen,
	     const char *in, size_t inlen,
	     char **out, size_t * outlen)
{
  struct CBC_CTX (struct des3_ctx, DES3_BLOCK_SIZE) des3;
  int rc;

  *outlen = inlen;
  *out = xmalloc (*outlen);

  rc = des3_set_key (&des3.ctx, key);
  if (!rc)
    {
      shishi_error_printf (handle, "Nettle setkey failed");
      return SHISHI_CRYPTO_INTERNAL_ERROR;
    }

  memset (des3.iv, 0, sizeof (des3.iv));
  /* XXX Use CBC_SET_IV (&des, iv), but how with ivlen? */
  memcpy (des3.iv, iv, ivlen < sizeof (des3.iv) ? ivlen : sizeof (des3.iv));

  if (decryptp)
    CBC_DECRYPT (&des3, des3_decrypt, inlen, *out, in);
  else
    CBC_ENCRYPT (&des3, des3_encrypt, inlen, *out, in);

  if (ivout && ivoutlen)
    {
      *ivoutlen = sizeof (des3.iv);
      *ivout = xmemdup (des3.iv, *ivoutlen);
    }

  return SHISHI_OK;
}

int
shishi_aes (Shishi * handle, int decryptp,
	    const char *key, size_t keylen,
	    const char *iv, size_t ivlen,
	    char **ivout, size_t * ivoutlen,
	    const char *in, size_t inlen,
	    char **out, size_t * outlen)
{
  struct CBC_CTS_CTX (struct aes_ctx, AES_BLOCK_SIZE) aes;

  *outlen = inlen;
  *out = xmalloc (*outlen);

  memset (aes.iv, 0, sizeof (aes.iv));
  /* XXX Use CBC_SET_IV (&des, iv), but how with ivlen? */
  memcpy (aes.iv, iv, ivlen < sizeof (aes.iv) ? ivlen : sizeof (aes.iv));
  if (decryptp)
    {
      aes_set_decrypt_key (&aes.ctx, keylen, key);
      CBC_CTS_DECRYPT (&aes, aes_decrypt, inlen, *out, in);
    }
  else
    {
      aes_set_encrypt_key (&aes.ctx, keylen, key);
      CBC_CTS_ENCRYPT (&aes, aes_encrypt, inlen, *out, in);
    }

  if (ivout && ivoutlen)
    {
      *ivoutlen = sizeof (aes.iv);
      /* XXX what is the output iv for CBC-CTS mode?
	 but is this value useful at all for that mode anyway?
	 Mostly it is DES apps that want the updated iv, so this is ok. */
      *ivout = xmemdup (aes.iv, *ivoutlen);
    }

  return SHISHI_OK;
}
