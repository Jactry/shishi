/* crypto-des.c	DES crypto functions
 * Copyright (C) 2002  Simon Josefsson
 *
 * This file is part of Shishi.
 *
 * Shishi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Shishi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Shishi; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Note: This file is #include'd by crypto.c.
 *
 */

/* Table of CRCs of all 8-bit messages.
   Generated by running code from RFC 1952, modified to print out the table.*/
static uint32_t crc_table[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 
  0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 
  0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 
  0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 
  0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 
  0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 
  0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 
  0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 
  0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 
  0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 
  0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 
  0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 
  0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 
  0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 
  0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 
  0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 
  0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 
  0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242, 
  0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 
  0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 
  0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 
  0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

/* 
   The source code was extracted from RFC 1952 by Simon Josefsson, for
   the Shishi project, and modified to be compatible with the modified
   CRC-32 used by Kerberos.  Original copyright and notice follows.

   Copyright (c) 1996 L. Peter Deutsch

   Permission is granted to copy and distribute this document for any
   purpose and without charge, including translations into other
   languages and incorporation into compilations, provided that the
   copyright notice and this notice are preserved, and that any
   substantive changes or deletions from the original are clearly
   marked.

   The following sample code represents a practical implementation of
   the CRC (Cyclic Redundancy Check). (See also ISO 3309 and ITU-T V.42
   for a formal specification.)

   Return the CRC of the bytes buf[0..len-1]. */
uint32_t
shishi_mod_crc32 (char *buf, int len)
{
  uint32_t c = 0L
#if 0
  ^ 0xffffffffL
#endif
;
  int n;
  
  for (n = 0; n < len; n++)
    c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);

  return c
#if 0
  ^ 0xffffffffL
#endif
;
}

static int
des_encrypt (Shishi * handle,
	     char *out,
	     int *outlen, char *in, int inlen, char key[8])
{
  return simplified_dencrypt (handle, SHISHI_DES_CBC_MD5, out, outlen, in, inlen, key, 8, 0);
}

static int
des_decrypt (Shishi * handle,
	     char *out,
	     int *outlen, char *in, int inlen, char key[8])
{
  return simplified_dencrypt (handle, SHISHI_DES_CBC_MD5, out, outlen, in, inlen, key, 8, 1);
}

static int
des_crc_verify (Shishi * handle, char *out, int *outlen)
{
  uint32_t crc, crc2;
  int i;

  printf("crc %d: ", *outlen);
  for (i = 8; i < 8+4; i++)
    printf ("%02X ", out[i] & 0xFF);
  printf ("\n");

  memcpy((char*)&crc, &out[8], 4);

  printf("in %08x\n", crc);
  crc = htonl(crc);
  printf("in %08x\n", crc);

  memset((char*)&out[8], 0, 4);

  printf("crc %d: ", *outlen);
  for (i = 0; i < *outlen; i++)
    printf ("%02X ", out[i] & 0xFF);
  printf ("\n");

  crc2 = shishi_mod_crc32(out, *outlen);
  printf("calc %08x\n", crc2);
  crc2 = htonl(crc2);
  printf("calc %08x\n", crc2);

  printf("str %d: ", *outlen);
  for (i = 0; i < *outlen; i++)
    printf ("%02X ", out[i] & 0xFF);
  printf ("\n");

  memmove (out, out + 8 + 4, *outlen - 8 - 4);
  *outlen -= 8 + 4;

  return SHISHI_OK;
}

static int
des_crc_checksum (Shishi * handle,
		  char *out,
		  int *outlen, char *in, int inlen)
{
  uint32_t crc;
  int res;

  if (inlen + 8 + 4 > BUFSIZ)
    {
      shishi_error_printf (handle, "checksum inbuffer too large");
      return !SHISHI_OK;
    }

  memmove (in + 8 + 4, in, inlen);
  memset (in + 8, 0, 4);

  res = shishi_randomize (handle, in, 8);
  if (res != SHISHI_OK)
    return res;

  crc = shishi_mod_crc32(in, 8 + 4 + inlen);
  crc = htonl(crc);
  memcpy(&in[8], &crc, 4);

  *outlen = 8 + 4;

  return SHISHI_OK;
}

static int
des_md4_verify (Shishi * handle, char *out, int *outlen)
{
  int res;
  char md[16];
  GCRY_MD_HD hd;
  char *p;

  memcpy (md, out + 8, 16);
  memset (out + 8, 0, 16);

  hd = gcry_md_open (GCRY_MD_MD4, 0);

  if (!hd)
    {
      puts("MD4 not available");
      return !SHISHI_OK;
    }

  gcry_md_write (hd, out, *outlen);
  p = gcry_md_read (hd, GCRY_MD_MD4);
  if (DEBUGCRYPTO(handle))
    {
      int i;

      for (i = 0; i < 16; i++)
	printf ("%02X ", md[i]);
      printf ("\n");
      for (i = 0; i < 16; i++)
	printf ("%02X ", p[i]);
      printf ("\n");
    }

  if (memcmp (p, md, 16) == 0)
    {
      memmove (out, out + 8 + 16, *outlen - 8 - 16);
      *outlen -= 8 + 16;
      res = SHISHI_OK;
    }
  else
    {
      if (DEBUG(handle))
	printf ("des-cbc-md4 verify fail\n");
      res = !SHISHI_OK;
    }

  gcry_md_close (hd);

  return res;
}

static int
des_md4_checksum (Shishi * handle,
		  char *out,
		  int *outlen, char *in, int inlen)
{
  int res;
  char buffer[BUFSIZ];
  char confounder[8];
  char md[16];
  char *p;
  GCRY_MD_HD hd;
  int i;

  if (inlen + 8 + 16 > BUFSIZ)
    {
      shishi_error_printf (handle, "checksum inbuffer too large");
      return !SHISHI_OK;
    }

  memcpy (buffer + 8 + 16, in, inlen);
  memset (buffer + 8, 0, 16);

  res = shishi_randomize (handle, buffer, 8);
  if (res != SHISHI_OK)
    return res;

  hd = gcry_md_open (GCRY_MD_MD4, 0);
  if (hd == NULL)
    return SHISHI_GCRYPT_ERROR;

  gcry_md_write (hd, buffer, inlen + 8 + 16);
  p = gcry_md_read (hd, GCRY_MD_MD4);

  memcpy (buffer + 8, p, 16);
  gcry_md_close (hd);

  memcpy (out, buffer, 8 + 16);

  *outlen = 8 + 16;

  return SHISHI_OK;
}

static int
des_md5_verify (Shishi * handle, char *out, int *outlen)
{
  int res;
  char md[16];
  GCRY_MD_HD hd;
  char *p;

  memcpy (md, out + 8, 16);
  memset (out + 8, 0, 16);

  hd = gcry_md_open (GCRY_MD_MD5, 0);

  if (!hd)
    {
      shishi_error_set (handle, "MD5 not available");
      return !SHISHI_OK;
    }

  gcry_md_write (hd, out, *outlen);
  p = gcry_md_read (hd, GCRY_MD_MD5);
  if (DEBUGCRYPTO(handle))
    {
      int i;

      for (i = 0; i < 16; i++)
	printf ("%02X ", md[i]);
      printf ("\n");
      for (i = 0; i < 16; i++)
	printf ("%02X ", p[i]);
      printf ("\n");
    }

  if (memcmp (p, md, 16) == 0)
    {
      memmove (out, out + 8 + 16, *outlen - 8 - 16);
      *outlen -= 8 + 16;
      res = SHISHI_OK;
    }
  else
    {
      if (DEBUG(handle))
	printf ("des-cbc-md5 verify fail\n");
      res = !SHISHI_OK;
    }

  gcry_md_close (hd);

  return res;
}

static int
des_md5_checksum (Shishi * handle,
		  char *out,
		  int *outlen, char *in, int inlen)
{
  int res;
  char buffer[BUFSIZ];
  char confounder[8];
  char md[16];
  char *p;
  GCRY_MD_HD hd;
  int i;

  if (inlen + 8 + 16 > BUFSIZ)
    {
      shishi_error_printf (handle, "checksum inbuffer too large");
      return !SHISHI_OK;
    }

  memcpy (buffer + 8 + 16, in, inlen);
  memset (buffer + 8, 0, 16);

  res = shishi_randomize (handle, buffer, 8);
  if (res != SHISHI_OK)
    return res;

  hd = gcry_md_open (GCRY_MD_MD5, 0);
  if (hd == NULL)
    return SHISHI_GCRYPT_ERROR;

  gcry_md_write (hd, buffer, inlen + 8 + 16);
  p = gcry_md_read (hd, GCRY_MD_MD5);

  memcpy (buffer + 8, p, 16);
  gcry_md_close (hd);

  memcpy (out, buffer, 8 + 16);

  *outlen = 8 + 16;

  return SHISHI_OK;
}

static int
des_crc_encrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  char buffer[BUFSIZ];
  int buflen;
  int res;

  while ((inlen % 8) != 0)
    {
      in[inlen] = '\0';
      inlen++;
    }

  buflen = sizeof (buffer);
  res = des_crc_checksum (handle, buffer, &buflen, in, inlen);
  memcpy (buffer + buflen, in, inlen);
  buflen += inlen;
  res = des_encrypt (handle, out, outlen, buffer, buflen, key);

  return res;
}

static int
des_crc_decrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  int res;

  printf("in %d\n", inlen);
  res = des_decrypt (handle, out, outlen, in, inlen, key);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "decrypt failed");
      return res;
    }
#if 0
  memcpy(out, "\x56\xcc\xa9\xd6\x67\x0a\xca\x0e\xbc\x58\xdc\x9b\x79\x81\xd3\x30\x81\xd0\xa0\x13\x30\x11\xa0\x03\x02\x01\x01\xa1\x0a\x04\x08\x8f\x75\x58\x45\x9d\x31\x6b\x1f\xa1\x1c\x30\x1a\x30\x18\xa0\x03\x02\x01\x00\xa1\x11\x18\x0f\x31\x39\x37\x30\x30\x31\x30\x31\x30\x30\x30\x30\x30\x30\x5a\xa2\x06\x02\x04\x3d\xdd\x3a\x46\xa4\x07\x03\x05\x00\x50\x40\x00\x00\xa5\x11\x18\x0f\x32\x30\x30\x32\x31\x31\x32\x31\x31\x39\x35\x35\x35\x30\x5a\xa7\x11\x18\x0f\x32\x30\x30\x32\x31\x31\x32\x32\x30\x35\x35\x35\x35\x30\x5a\xa9\x0f\x1b\x0d\x4a\x4f\x53\x45\x46\x53\x53\x4f\x4e\x2e\x4f\x52\x47\xaa\x22\x30\x20\xa0\x03\x02\x01\x00\xa1\x19\x30\x17\x1b\x06\x6b\x72\x62\x74\x67\x74\x1b\x0d\x4a\x4f\x53\x45\x46\x53\x53\x4f\x4e\x2e\x4f\x52\x47\xab\x2f\x30\x2d\x30\x0d\xa0\x03\x02\x01\x02\xa1\x06\x04\x04\xc0\xa8\x01\x01\x30\x0d\xa0\x03\x02\x01\x02\xa1\x06\x04\x04\xc0\xa8\x02\x01\x30\x0d\xa0\x03\x02\x01\x02\xa1\x06\x04\x04\xd9\xd0\xac\x49\x00\x00\x00\x00\x00\x00", 232);
   *outlen = 232;
#endif
    {
      int i;
      printf("decrypt %d\n", *outlen);
      for(i=0; i < *outlen; i++)
	printf("%02x ", ((char*)out)[i] & 0xFF);
      printf("\n");
    }
  res = des_crc_verify (handle, out, outlen);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "verify failed");
      return res;
    }

  return res;
}

static int
des_md4_encrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  char buffer[BUFSIZ];
  int buflen;
  int res;

  while ((inlen % 8) != 0)
    {
      in[inlen] = '\0';
      inlen++;
    }

  buflen = sizeof (buffer);
  res = des_md4_checksum (handle, buffer, &buflen, in, inlen);
  memcpy (buffer + buflen, in, inlen);
  buflen += inlen;
  res = des_encrypt (handle, out, outlen, buffer, buflen, key);

  return res;
}

static int
des_md4_decrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  int res;

  res = des_decrypt (handle, out, outlen, in, inlen, key);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "decrypt failed");
      return res;
    }
  res = des_md4_verify (handle, out, outlen);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "verify failed");
      return res;
    }

  return res;
}

static int
des_md5_encrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  char buffer[BUFSIZ];
  int buflen;
  int res;

  while ((inlen % 8) != 0)
    {
      in[inlen] = '\0';
      inlen++;
    }

  buflen = sizeof (buffer);
  res = des_md5_checksum (handle, buffer, &buflen, in, inlen);
  memcpy (buffer + buflen, in, inlen);
  buflen += inlen;
  res = des_encrypt (handle, out, outlen, buffer, buflen, key);

  return res;
}

static int
des_md5_decrypt (Shishi * handle,
	      int keyusage,
	      char *key,
	      int keylen,
	      char *in,
	      int inlen,
	      char *out,
	      int *outlen)
{
  int res;

  res = des_decrypt (handle, out, outlen, in, inlen, key);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "decrypt failed");
      return res;
    }
  res = des_md5_verify (handle, out, outlen);
  if (res != SHISHI_OK)
    {
      shishi_error_set (handle, "verify failed");
      return res;
    }

  return res;
}

static int
des_set_odd_key_parity (char key[8])
{
  int i, j;

  for (i = 0; i < 8; i++)
    {
      int n_set_bits = 0;

      for (j = 1; j < 8; j++)
	if (key[i] & (1 << j))
	  n_set_bits++;

      key[i] &= ~1;
      if ((n_set_bits % 2) == 0)
	key[i] |= 1;
    }

  return SHISHI_OK;
}

static int
des_key_correction (Shishi * handle, char *key)
{
  int res;
  GCRY_CIPHER_HD ch;
  int j;

  /* fixparity(key); */
  des_set_odd_key_parity (key);

  ch = gcry_cipher_open (GCRY_CIPHER_DES, GCRY_CIPHER_MODE_CBC, 0);
  if (ch == NULL)
    return !SHISHI_OK;

  /* XXX libgcrypt tests for pseudo-weak keys, rfc 1510 doesn't */

  res = gcry_cipher_setkey (ch, key, 8);
  if (res != GCRYERR_SUCCESS)
    {
      if (res == GCRYERR_WEAK_KEY)
	{
	  if (DEBUGCRYPTO(handle))
	    printf ("\t ;; WEAK KEY (corrected)\n");
	  key[7] ^= 0xF0;
	}
      else
	return !SHISHI_OK;
    }

  gcry_cipher_close (ch);

  return SHISHI_OK;
}

static int
des_cbc_check (char key[8], char *data, int n_data)
{
  GCRY_CIPHER_HD ch;
  int j;
  int res;
  char ct[1024];
  char final[2 * 8];
  char iv[8];

  ch = gcry_cipher_open (GCRY_CIPHER_DES, GCRY_CIPHER_MODE_CBC, 0);
  if (ch == NULL)
    return !SHISHI_OK;

  res = gcry_cipher_setkey (ch, key, 8);
  if (res != GCRYERR_SUCCESS)
    return !SHISHI_OK;

  memset (iv, 0, 8);
  res = gcry_cipher_setiv (ch, key, 8);
  if (res != 0)
    return !SHISHI_OK;

  res = gcry_cipher_encrypt (ch, ct, sizeof (ct), data, n_data);
  if (res != 0)
    return !SHISHI_OK;

  memcpy (key, ct + n_data - 8, 8);

  gcry_cipher_close (ch);

  return SHISHI_OK;
}

static int
des_random_to_key (Shishi * handle,
		   char *random,
		   int randomlen,
		   char *outkey)
{
  int keylen = shishi_cipher_keylen (SHISHI_DES_CBC_MD5);

  if (randomlen < keylen)
    return !SHISHI_OK;

  memcpy(outkey, random, keylen);
  des_set_odd_key_parity (outkey);

  return SHISHI_OK;
}

static int
des_string_to_key (Shishi * handle,
		   char *string,
		   int stringlen,
		   char *salt,
		   int saltlen,
		   char *parameter,
		   char *outkey)
{
  char *s;
  int n_s;
  int odd;
  char tempkey[8];
  int i, j;
  char temp, temp2;
  int res;

  if (DEBUGCRYPTO(handle))
    {
      printf ("des_string_to_key (string, salt)\n");

      printf ("\t ;; String:\n");
      escapeprint (string, stringlen);
      hexprint (string, stringlen);
      puts ("");
      puts ("");

      printf ("\t ;; Salt:\n");
      escapeprint (salt, saltlen);
      hexprint (salt, saltlen);
      puts ("");

      printf ("odd = 1;\n");
      printf ("s = string | salt;\n");
      printf ("tempstring = 0; /* 56-bit string */\n");
      printf ("pad(s); /* with nulls to 8 byte boundary */\n");

    }

  if (saltlen < 0)
    saltlen = 0;

  odd = 1;
  n_s = stringlen + saltlen;
  if ((n_s % 8) != 0)
    n_s += 8 - n_s % 8;
  s = (char *) malloc (n_s);
  memcpy (s, string, stringlen);
  if (saltlen > 0)
    memcpy (s + stringlen, salt, saltlen);
  memset (s + stringlen + saltlen, 0, n_s - stringlen - saltlen);
  memset (tempkey, 0, sizeof (tempkey));	/* tempkey = NULL; */

  if (DEBUGCRYPTO(handle))
    {
      printf ("\t ;; s = pad(string|salt):\n");
      escapeprint (s, n_s);
      hexprint (s, n_s);
      puts ("");
    }

  for (i = 0; i < n_s / 8; i++)
    {
      if (DEBUGCRYPTO(handle))
	{
	  printf ("for (8byteblock in s) {\n");
	  printf ("\t ;; loop iteration %d\n", i);
	  printf ("\t ;; 8byteblock:\n");
	  escapeprint (&s[i * 8], 8);
	  hexprint (&s[i * 8], 8);
	  puts ("");
	  binprint (&s[i * 8], 8);
	  puts ("");
	  printf ("56bitstring = removeMSBits(8byteblock);\n");
	}

      for (j = 0; j < 8; j++)
	s[i * 8 + j] = s[i * 8 + j] & ~0x80;

      if (DEBUGCRYPTO(handle))
	{
	  printf ("\t ;; 56bitstring:\n");
	  bin7print (&s[i * 8], 8);
	  puts ("");
	  printf ("if (odd == 0) reverse(56bitstring);\t ;; odd=%d\n", odd);
	}

      if (odd == 0)
	{
	  for (j = 0; j < 4; j++)
	    {
	      temp = s[i * 8 + j];
	      temp =
		((temp >> 6) & 0x01) |
		((temp >> 4) & 0x02) |
		((temp >> 2) & 0x04) |
		((temp) & 0x08) |
		((temp << 2) & 0x10) |
		((temp << 4) & 0x20) | ((temp << 6) & 0x40);
	      temp2 = s[i * 8 + 7 - j];
	      temp2 =
		((temp2 >> 6) & 0x01) |
		((temp2 >> 4) & 0x02) |
		((temp2 >> 2) & 0x04) |
		((temp2) & 0x08) |
		((temp2 << 2) & 0x10) |
		((temp2 << 4) & 0x20) | ((temp2 << 6) & 0x40);
	      s[i * 8 + j] = temp2;
	      s[i * 8 + 7 - j] = temp;
	    }
	  if (DEBUGCRYPTO(handle))
	    {
	      printf ("reverse(56bitstring)\n");
	      printf ("\t ;; 56bitstring after reverse\n");
	      bin7print (&s[i * 8], 8);
	      puts ("");
	    }
	}

      odd = !odd;

      if (DEBUGCRYPTO(handle))
	{
	  printf ("odd = ! odd\n");
	  printf ("tempstring = tempstring XOR 56bitstring;\n");
	}

      /* tempkey = tempkey XOR 8byteblock; */
      for (j = 0; j < 8; j++)
	tempkey[j] ^= s[i * 8 + j];

      if (DEBUGCRYPTO(handle))
	{
	  printf ("\t ;; tempstring\n");
	  bin7print (tempkey, 8);
	  puts ("");
	  puts ("");
	}
    }

  for (j = 0; j < 8; j++)
    tempkey[j] = tempkey[j] << 1;

  if (DEBUGCRYPTO(handle))
    {
      printf ("for (8byteblock in s) {\n");
      printf ("}\n");
      printf ("\t ;; for loop terminated\n");
      printf ("\t ;; tempstring as 64bitblock\n");
      hexprint (tempkey, 8);
      puts ("");
      binprint (tempkey, 8);
      puts ("");
      printf ("/* add parity as low bit of each byte */\n");
      printf ("tempkey = key_correction(add_parity_bits(tempstring));\n");
    }

  res = des_key_correction (handle, tempkey);
  if (res != SHISHI_OK)
    return res;

  if (DEBUGCRYPTO(handle))
    {
      printf ("\t ;; tempkey\n");
      escapeprint (tempkey, 8);
      hexprint (tempkey, 8);
      puts ("");
      binprint (tempkey, 8);
      puts ("");
      puts ("");
      printf ("key = key_correction(DES-CBC-check(s,tempkey));\n");
    }

  memcpy (s, string, stringlen);
  if (saltlen > 0)
    memcpy (s + stringlen, salt, saltlen);
  memset (s + stringlen + saltlen, 0, n_s - stringlen - saltlen);

  res = des_cbc_check (tempkey, s, n_s);
  if (res != SHISHI_OK)
    return res;

  res = des_key_correction (handle, tempkey);
  if (res != SHISHI_OK)
    return res;

  if (DEBUGCRYPTO(handle))
    {
      printf ("\t ;; key\n");
      escapeprint (tempkey, 8);
      hexprint (tempkey, 8);
      puts ("");
      binprint (tempkey, 8);
      puts ("");
      puts ("");
    }

  memcpy (outkey, tempkey, 8);

  return SHISHI_OK;
}

static int
checksum_md4 (Shishi * handle,
	      char *out, int *outlen, char *in, int inlen)
{
  int res;
  char buffer[BUFSIZ];
  char confounder[8];
  char md[16];
  GCRY_MD_HD hd;
  int i;

  if (inlen + 8 > BUFSIZ)
    {
      shishi_error_printf (handle, "checksum inbuffer too large");
      return !SHISHI_OK;
    }


  memcpy (buffer + 8, in, inlen);

#if 0
  printf ("cksum in len=%d:", inlen);
  for (i = 0; i < inlen; i++)
    printf ("%02x ", in[i]);
  printf ("\n");
#endif

  res = shishi_randomize (handle, buffer, 8);
  if (res != SHISHI_OK)
    return res;

#if 0
  printf ("cksum random: ");
  for (i = 0; i < 8; i++)
    printf ("%02X ", buffer[i]);
  printf ("\n");
#endif

  hd = gcry_md_open (GCRY_MD_MD4, 0);
  if (hd)
    {
      char *p;

      gcry_md_write (hd, buffer, inlen + 8);
      p = gcry_md_read (hd, GCRY_MD_MD4);

#if 0
      printf ("cksum md4: ");
      for (i = 0; i < 16; i++)
	printf ("%02X ", p[i]);
      printf ("\n");
#endif

      memcpy (buffer + 8, p, 16);
      gcry_md_close (hd);
    }
  else
    {
      puts ("bajs");
      exit (1);
    }

  memcpy (out, buffer, 8 + 16);

  *outlen = 8 + 16;

#if 0
  printf ("cksum out: ");
  for (i = 0; i < *outlen; i++)
    printf ("%02X ", out[i]);
  printf ("\n");
#endif

  return SHISHI_OK;
}

static int
checksum_md5 (Shishi * handle,
	      char *out, int *outlen, char *in, int inlen)
{
  int res;
  char buffer[BUFSIZ];
  char confounder[8];
  char md[16];
  GCRY_MD_HD hd;
  int i;

  if (inlen + 8 > BUFSIZ)
    {
      shishi_error_printf (handle, "checksum inbuffer too large");
      return !SHISHI_OK;
    }


  memcpy (buffer + 8, in, inlen);

#if 0
  printf ("cksum in len=%d:", inlen);
  for (i = 0; i < inlen; i++)
    printf ("%02x ", in[i]);
  printf ("\n");
#endif

  res = shishi_randomize (handle, buffer, 8);
  if (res != SHISHI_OK)
    return res;

#if 0
  printf ("cksum random: ");
  for (i = 0; i < 8; i++)
    printf ("%02X ", buffer[i]);
  printf ("\n");
#endif

  hd = gcry_md_open (GCRY_MD_MD5, 0);
  if (hd)
    {
      char *p;

      gcry_md_write (hd, buffer, inlen + 8);
      p = gcry_md_read (hd, GCRY_MD_MD5);

#if 0
      printf ("cksum md5: ");
      for (i = 0; i < 16; i++)
	printf ("%02X ", p[i]);
      printf ("\n");
#endif

      memcpy (buffer + 8, p, 16);
      gcry_md_close (hd);
    }
  else
    {
      puts ("bajs");
      exit (1);
    }

  memcpy (out, buffer, 8 + 16);

  *outlen = 8 + 16;

#if 0
  printf ("cksum out: ");
  for (i = 0; i < *outlen; i++)
    printf ("%02X ", out[i]);
  printf ("\n");
#endif

  return SHISHI_OK;
}
