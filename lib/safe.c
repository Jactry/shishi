/* safe.c	Application data integrity protection.
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

#include "internal.h"

struct Shishi_safe
{
  Shishi *handle;
  Shishi_key *key;
  Shishi_asn1 safe;
  unsigned long seqnumber;
};

/**
 * shishi_safe:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: pointer to new structure that holds information about SAFE exchange
 *
 * Create a new SAFE exchange.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe (Shishi * handle, Shishi_safe ** safe)
{
  Shishi_safe *lsafe;
  struct timeval tv;
  struct timezone tz;
  char usec[BUFSIZ];
  int rc;

  *safe = malloc (sizeof (**safe));
  if (*safe == NULL)
    return SHISHI_MALLOC_ERROR;
  lsafe = *safe;
  memset (lsafe, 0, sizeof (*lsafe));

  lsafe->handle = handle;
  rc = shishi_key (handle, &lsafe->key);
  if (rc != SHISHI_OK)
    return rc;

  lsafe->safe = shishi_asn1_krbsafe (handle);
  if (lsafe->safe == NULL)
    return SHISHI_ASN1_ERROR;

  rc = shishi_asn1_write (handle, lsafe->safe, "pvno", "5", 0);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe, "msg-type", "20", 0);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe, "safe-body.seq-number",
			  NULL, 0);
  if (rc != SHISHI_OK)
    return rc;

  gettimeofday (&tv, &tz);
  sprintf (usec, "%ld", tv.tv_usec % 1000000);
  rc = shishi_asn1_write (handle, lsafe->safe, "safe-body.usec",
			  usec, 0);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe, "safe-body.timestamp",
			  shishi_generalize_time (handle, time (NULL)), 0);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe,
			  "safe-body.s-address.addr-type",
			  "3", 0); /* directional */
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe,
			  "safe-body.s-address.address",
			  "\x00\x00\x00\x00", 4); /* sender */
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe, "safe-body.r-address",
			  NULL, 0);
  if (rc != SHISHI_OK)
    return rc;

  return SHISHI_OK;
}

/**
 * shishi_safe_key:
 * @ap: structure that holds information about SAFE exchange
 *
 * Return value: Returns the key used in the SAFE exchange, or NULL if
 *               not yet set or an error occured.
 **/
Shishi_key *
shishi_safe_key (Shishi_safe * safe)
{
  return safe->key;
}

/**
 * shishi_safe_key_set:
 * @safe: structure that holds information about SAFE exchange
 * @key: key to store in SAFE.
 *
 * Set the Key in the SAFE exchange.
 **/
void
shishi_safe_key_set (Shishi_safe * safe, Shishi_key * key)
{
  shishi_key_copy (safe->key, key);
}

/**
 * shishi_safe_safe:
 * @ap: structure that holds information about SAFE exchange
 *
 * Return value: Returns the ASN.1 safe in the SAFE exchange, or NULL if
 *               not yet set or an error occured.
 **/
Shishi_asn1
shishi_safe_safe (Shishi_safe * safe)
{
  return safe->safe;
}

/**
 * shishi_safe_safe_set:
 * @safe: structure that holds information about SAFE exchange
 * @safe: KRB-SAFE to store in SAFE exchange.
 *
 * Set the KRB-SAFE in the SAFE exchange.
 **/
void
shishi_safe_safe_set (Shishi_safe * safe, Shishi_asn1 asn1safe)
{
  if (safe->safe)
    shishi_asn1_done (safe->handle, safe->safe);
  safe->safe = asn1safe;
}

/**
 * shishi_safe_safe_der:
 * @safe: safe as allocated by shishi_safe().
 * @out: output array with der encoding of SAFE.
 * @outlen: length of output array with der encoding of SAFE.
 *
 * DER encode SAFE structure.  Typically shishi_safe_build() is used
 * instead to build the SAFE structure first.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_safe_der (Shishi_safe * safe, char *out, int *outlen)
{
  int rc;

  rc = shishi_a2d (safe->handle, safe->safe, out, outlen);
  if (rc != SHISHI_OK)
    return rc;

  return SHISHI_OK;
}

/**
 * shishi_safe_safe_der_set:
 * @safe: safe as allocated by shishi_safe().
 * @der: input array with DER encoded KRB-SAFE.
 * @derlen: length of input array with DER encoded KRB-SAFE.
 *
 * DER decode KRB-SAFE and set it SAFE exchange.  If decoding fails, the
 * KRB-SAFE in the SAFE exchange remains.
 *
 * Return value: Returns SHISHI_OK.
 **/
int
shishi_safe_safe_der_set (Shishi_safe * safe, char *der, int derlen)
{
  Shishi_asn1 asn1safe;

  asn1safe = shishi_der2asn1_krbsafe (safe->handle, der, derlen);

  if (asn1safe == NULL)
    return SHISHI_ASN1_ERROR;

  shishi_safe_safe_set(safe, asn1safe);

  return SHISHI_OK;
}

/**
 * shishi_safe_print:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for writing.
 * @safe: SAFE to print.
 *
 * Print ASCII armored DER encoding of SAFE to file.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_print (Shishi * handle, FILE * fh, Shishi_asn1 safe)
{
  return _shishi_print_armored_data (handle, fh, safe, "KRB-SAFE", NULL);
}

/**
 * shishi_safe_save:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for writing.
 * @safe: SAFE to save.
 *
 * Save DER encoding of SAFE to file.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_save (Shishi * handle, FILE * fh, Shishi_asn1 safe)
{
  return _shishi_save_data (handle, fh, safe, "SAFE");
}

/**
 * shishi_safe_to_file:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: SAFE to save.
 * @filetype: input variable specifying type of file to be written,
 *            see Shishi_filetype.
 * @filename: input variable with filename to write to.
 *
 * Write SAFE to file in specified TYPE.  The file will be
 * truncated if it exists.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_to_file (Shishi * handle, Shishi_asn1 safe,
		     int filetype, char *filename)
{
  FILE *fh;
  int res;

  if (VERBOSE (handle))
    printf (_("Writing SAFE to %s...\n"), filename);

  fh = fopen (filename, "w");
  if (fh == NULL)
    return SHISHI_FOPEN_ERROR;

  if (VERBOSE (handle))
    printf (_("Writing SAFE in %s format...\n"),
	    filetype == SHISHI_FILETYPE_TEXT ? "TEXT" : "DER");

  if (filetype == SHISHI_FILETYPE_TEXT)
    res = shishi_safe_print (handle, fh, safe);
  else
    res = shishi_safe_save (handle, fh, safe);
  if (res != SHISHI_OK)
    return res;

  res = fclose (fh);
  if (res != 0)
    return SHISHI_FCLOSE_ERROR;

  if (VERBOSE (handle))
    printf (_("Writing SAFE to %s...done\n"), filename);

  return SHISHI_OK;
}

/**
 * shishi_safe_parse:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for reading.
 * @safe: output variable with newly allocated SAFE.
 *
 * Read ASCII armored DER encoded SAFE from file and populate given
 * variable.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_parse (Shishi * handle, FILE * fh, Shishi_asn1 * safe)
{
  return _shishi_safe_input (handle, fh, safe, 0);
}

/**
 * shishi_safe_read:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for reading.
 * @safe: output variable with newly allocated SAFE.
 *
 * Read DER encoded SAFE from file and populate given variable.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_read (Shishi * handle, FILE * fh, Shishi_asn1 * safe)
{
  return _shishi_safe_input (handle, fh, safe, 1);
}

/**
 * shishi_safe_from_file:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: output variable with newly allocated SAFE.
 * @filetype: input variable specifying type of file to be read,
 *            see Shishi_filetype.
 * @filename: input variable with filename to read from.
 *
 * Read SAFE from file in specified TYPE.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_from_file (Shishi * handle, Shishi_asn1 * safe,
		       int filetype, char *filename)
{
  int res;
  FILE *fh;

  if (VERBOSE (handle))
    printf (_("Reading SAFE from %s...\n"), filename);

  fh = fopen (filename, "r");
  if (fh == NULL)
    return SHISHI_FOPEN_ERROR;

  if (VERBOSE (handle))
    printf (_("Reading SAFE in %s format...\n"),
	    filetype == SHISHI_FILETYPE_TEXT ? "TEXT" : "DER");

  if (filetype == SHISHI_FILETYPE_TEXT)
    res = shishi_safe_parse (handle, fh, safe);
  else
    res = shishi_safe_read (handle, fh, safe);
  if (res != SHISHI_OK)
    return res;

  res = fclose (fh);
  if (res != 0)
    return SHISHI_FCLOSE_ERROR;

  if (VERBOSE (handle))
    printf (_("Reading SAFE from %s...done\n"), filename);

  return SHISHI_OK;
}

/**
 * shishi_safe_cksum:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: safe as allocated by shishi_safe().
 * @cksumtype: output checksum type.
 * @cksum: output checksum data from SAFE.
 * @cksumlen: on input, maximum size of output checksum data buffer,
 *            on output, actual size of output checksum data buffer.
 *
 * Read checksum value from KRB-SAFE.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_cksum (Shishi * handle,
		   Shishi_asn1 safe,
		   int *cksumtype, char *cksum, size_t * cksumlen)
{
  int res;

  res = shishi_asn1_integer_field (handle, safe, cksumtype,
				   "cksum.cksumtype");
  if (res != SHISHI_OK)
    return res;

  res = shishi_asn1_field (handle, safe, cksum, cksumlen,
			   "cksum.checksum");
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_safe_set_cksum:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: safe as allocated by shishi_safe().
 * @cksumtype: input checksum type to store in SAFE.
 * @cksum: input checksum data to store in SAFE.
 * @cksumlen: size of input checksum data to store in SAFE.
 *
 * Store checksum value in SAFE.  A checksum is usually created by
 * calling shishi_checksum() on some application specific data using
 * the key from the ticket that is being used.  To save time, you may
 * want to use shishi_safe_build() instead, which calculates the
 * checksum and calls this function in one step.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_set_cksum (Shishi * handle,
		       Shishi_asn1 safe,
		       int cksumtype, char *cksum, int cksumlen)
{
  char format[BUFSIZ];
  int res;

  sprintf (format, "%i", cksumtype);
  res = shishi_asn1_write (handle, safe, "cksum.cksumtype",
			   format, 0);
  if (res != SHISHI_OK)
    return res;

  res = shishi_asn1_write (handle, safe, "cksum.checksum",
			   cksum, cksumlen);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_safe_user_data:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: safe as allocated by shishi_safe().
 * @userdata: output user data from KRB-SAFE.
 * @userdatalen: on input, maximum size of output user data buffer,
 *               on output, actual size of output user data buffer.
 *
 * Read user data value from KRB-SAFE.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_user_data (Shishi * handle,
		       Shishi_asn1 safe,
		       char *userdata, size_t * userdatalen)
{
  int res;

  res = shishi_asn1_read (handle, safe, "safe-body.user-data",
			  userdata, userdatalen);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_safe_set_user_data:
 * @handle: shishi handle as allocated by shishi_init().
 * @safe: safe as allocated by shishi_safe().
 * @userdata: input user application to store in SAFE.
 * @userdatalen: size of input user application to store in SAFE.
 *
 * Set the application data in SAFE.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_set_user_data (Shishi * handle,
			   Shishi_asn1 safe,
			   char *userdata, int userdatalen)
{
  int res;

  res = shishi_asn1_write (handle, safe, "safe-body.user-data",
			   userdata, userdatalen);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_safe_build:
 * @safe: safe as allocated by shishi_safe().
 * @key: key for session, used to compute checksum.
 *
 * Build checksum and set it in KRB-SAFE.  Note that this follows RFC
 * 1510bis and is incompatible with RFC 1510, although presumably few
 * implementations use the RFC1510 algorithm.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_safe_build (Shishi_safe * safe, Shishi_key * key)
{
  int rc;
  char buffer[BUFSIZ];
  int buflen;
  char cksum[BUFSIZ];
  int cksumlen;
  int cksumtype = shishi_cipher_defaultcksumtype (shishi_key_type (key));

  rc = shishi_safe_set_cksum(safe->handle, safe->safe, 0, "", 0);
  if (rc != SHISHI_OK)
    return rc;

  buflen = sizeof(buffer);
  rc = shishi_safe_safe_der (safe, buffer, &buflen);
  if (rc != SHISHI_OK)
    return rc;

  /* XXX check if keytype/cksumtype is suitable for SAFE */

  if (VERBOSEASN1(safe->handle))
    shishi_key_print(safe->handle, stdout, key);

  cksumlen = sizeof (cksum);
  rc = shishi_checksum (safe->handle, key, SHISHI_KEYUSAGE_KRB_SAFE, cksumtype,
			buffer, buflen, cksum, &cksumlen);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_safe_set_cksum (safe->handle, safe->safe,
			      cksumtype, cksum, cksumlen);
  if (rc != SHISHI_OK)
    return rc;

  return SHISHI_OK;
}

/**
 * shishi_safe_verify:
 * @safe: safe as allocated by shishi_safe().
 * @key: key for session, used to verify checksum.
 *
 * Verify checksum in KRB-SAFE.  Note that this follows RFC 1510bis
 * and is incompatible with RFC 1510, although presumably few
 * implementations use the RFC1510 algorithm.
 *
 * Return value: Returns SHISHI_OK iff successful,
 *               SHISHI_SAFE_BAD_KEYTYPE if an incompatible key type
 *               is used, or SHISHI_SAFE_VERIFY_FAILED if the actual
 *               verification failed.
 **/
int
shishi_safe_verify (Shishi_safe * safe, Shishi_key * key)
{
  char cksum[MAX_CKSUM_LEN];
  size_t cksumlen;
  int cksumtype;
  char cksum2[MAX_CKSUM_LEN];
  size_t cksum2len;
  int cksumtype2;
  int rc;

  cksumlen = sizeof(cksum);
  rc = shishi_safe_cksum (safe->handle, safe->safe,
			  &cksumtype, cksum, &cksumlen);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_safe_build (safe, key);
  if (rc != SHISHI_OK)
    return rc;

  cksum2len = sizeof (cksum2);
  rc = shishi_safe_cksum (safe->handle, safe->safe,
			  &cksumtype2, cksum2, &cksum2len);
  if (rc != SHISHI_OK)
    return rc;

  if (cksumtype != cksumtype2)
    return SHISHI_SAFE_BAD_KEYTYPE;

  if (cksum2len != cksumlen ||
      memcmp(cksum, cksum2, cksumlen) != 0)
    return SHISHI_SAFE_VERIFY_FAILED;

  return SHISHI_OK;
}
