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

  rc = shishi_asn1_write (handle, lsafe->safe, "KRB-SAFE.pvno", "5", 0);
  if (rc != SHISHI_OK)
    return rc;

  rc = shishi_asn1_write (handle, lsafe->safe, "KRB-SAFE.msg-type", "20", 0);
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
  return _shishi_print_armored_data (handle, fh, safe, "SAFE", NULL);
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
