/* kdcreq.c	Key distribution (AS/TGS) request functions
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

#define SHISHI_KDCREQ_DEFAULT_PVNO      "5"
#define SHISHI_KDCREQ_DEFAULT_PVNO_LEN  0
#define SHISHI_AS_REQ_DEFAULT_MSG_TYPE      "10"
#define SHISHI_AS_REQ_DEFAULT_MSG_TYPE_LEN  0
#define SHISHI_TGS_REQ_DEFAULT_MSG_TYPE      "12"
#define SHISHI_TGS_REQ_DEFAULT_MSG_TYPE_LEN  0
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_KDC_OPTIONS      "\x00\x00\x00\x00"
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_KDC_OPTIONS_LEN  32
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_SNAME_NAME_TYPE "1"	/* SHISHI_NT_PRINCIPAL */
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_SNAME_NAME_TYPE_LEN 0
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_TILL ""
#define SHISHI_KDCREQ_DEFAULT_REQ_BODY_TILL_LEN 1

static Shishi_asn1
_shishi_kdcreq (Shishi * handle, int as)
{
  int res;
  Shishi_asn1 node;
  const char *servicebuf[3];
  char noncebuf[4];

  if (as)
    node = shishi_asn1_asreq (handle);
  else
    node = shishi_asn1_tgsreq (handle);
  if (!node)
    return NULL;

  res = shishi_asn1_write (handle, node, "pvno",
			   SHISHI_KDCREQ_DEFAULT_PVNO,
			   SHISHI_KDCREQ_DEFAULT_PVNO_LEN);
  if (res != SHISHI_OK)
    goto error;

  if (as)
    res = shishi_asn1_write (handle, node, "msg-type",
			     SHISHI_AS_REQ_DEFAULT_MSG_TYPE,
			     SHISHI_AS_REQ_DEFAULT_MSG_TYPE_LEN);
  else
    res = shishi_asn1_write (handle, node, "msg-type",
			     SHISHI_TGS_REQ_DEFAULT_MSG_TYPE,
			     SHISHI_TGS_REQ_DEFAULT_MSG_TYPE_LEN);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_asn1_write (handle, node, "req-body.kdc-options",
			   SHISHI_KDCREQ_DEFAULT_REQ_BODY_KDC_OPTIONS,
			   SHISHI_KDCREQ_DEFAULT_REQ_BODY_KDC_OPTIONS_LEN);
  if (res != SHISHI_OK)
    goto error;

  if (as)
    res = shishi_kdcreq_set_cname (handle, node, SHISHI_NT_PRINCIPAL,
				   shishi_principal_default (handle));
  else
    res = shishi_asn1_write (handle, node, "req-body.cname", NULL, 0);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_kdcreq_set_realm (handle, node, shishi_realm_default (handle));
  if (res != SHISHI_OK)
    goto error;

  servicebuf[0] = "krbtgt";
  servicebuf[1] = shishi_realm_default (handle);
  servicebuf[2] = NULL;
  res = shishi_kdcreq_set_sname (handle, node,
				 SHISHI_NT_PRINCIPAL, servicebuf);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_asn1_write (handle, node, "req-body.sname.name-type",
			   SHISHI_KDCREQ_DEFAULT_REQ_BODY_SNAME_NAME_TYPE,
			   SHISHI_KDCREQ_DEFAULT_REQ_BODY_SNAME_NAME_TYPE_LEN);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_asn1_write (handle, node, "req-body.till",
			   shishi_generalize_time (handle,
						   time (NULL) +
						   handle->ticketlife), 0);
  if (res != SHISHI_OK)
    goto error;

  shishi_randomize (handle, &noncebuf[0], sizeof (noncebuf));
  res = shishi_asn1_write (handle, node, "req-body.nonce", noncebuf,
			   sizeof (noncebuf));
  if (res != SHISHI_OK)
    goto error;

  res = shishi_kdcreq_set_etype (handle, node, handle->clientkdcetypes,
				 handle->nclientkdcetypes);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_asn1_write (handle, node, "req-body.addresses", NULL, 0);
  if (res != SHISHI_OK)
    goto error;

  res = shishi_asn1_write (handle, node,
			   "req-body.enc-authorization-data", NULL, 0);
  if (res != SHISHI_OK)
    goto error;

  res =
    shishi_asn1_write (handle, node, "req-body.additional-tickets", NULL, 0);
  if (res != SHISHI_OK)
    goto error;

  return node;

error:
  shishi_asn1_done (handle, node);
  return NULL;
}

/**
 * shishi_asreq:
 * @handle: shishi handle as allocated by shishi_init().
 *
 * This function creates a new AS-REQ, populated with some default
 * values.
 *
 * Return value: Returns the AS-REQ or NULL on failure.
 **/
Shishi_asn1
shishi_asreq (Shishi * handle)
{
  return _shishi_kdcreq (handle, 1);
}

/**
 * shishi_tgsreq:
 * @handle: shishi handle as allocated by shishi_init().
 *
 * This function creates a new TGS-REQ, populated with some default
 * values.
 *
 * Return value: Returns the TGS-REQ or NULL on failure.
 **/
Shishi_asn1
shishi_tgsreq (Shishi * handle)
{
  return _shishi_kdcreq (handle, 0);
}

/**
 * shishi_kdcreq_print:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for writing.
 * @kdcreq: KDC-REQ to print.
 *
 * Print ASCII armored DER encoding of KDC-REQ to file.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_print (Shishi * handle, FILE * fh, Shishi_asn1 kdcreq)
{
  return _shishi_print_armored_data (handle, fh, kdcreq, "KDC-REQ", NULL);
}

/**
 * shishi_kdcreq_save:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for writing.
 * @kdcreq: KDC-REQ to save.
 *
 * Print DER encoding of KDC-REQ to file.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_save (Shishi * handle, FILE * fh, Shishi_asn1 kdcreq)
{
  return _shishi_save_data (handle, fh, kdcreq, "KDC-REQ");
}

/**
 * shishi_kdcreq_to_file:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to save.
 * @filetype: input variable specifying type of file to be written,
 *            see Shishi_filetype.
 * @filename: input variable with filename to write to.
 *
 * Write KDC-REQ to file in specified TYPE.  The file will be truncated
 * if it exists.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_to_file (Shishi * handle, Shishi_asn1 kdcreq,
		       int filetype, char *filename)
{
  FILE *fh;
  int res;

  if (VERBOSE (handle))
    printf (_("Writing KDC-REQ to %s...\n"), filename);

  fh = fopen (filename, "w");
  if (fh == NULL)
    return SHISHI_FOPEN_ERROR;

  if (VERBOSE (handle))
    printf (_("Writing KDC-REQ in %s format...\n"),
	    filetype == SHISHI_FILETYPE_TEXT ? "TEXT" : "DER");

  if (filetype == SHISHI_FILETYPE_TEXT)
    res = shishi_kdcreq_print (handle, fh, kdcreq);
  else
    res = shishi_kdcreq_save (handle, fh, kdcreq);
  if (res != SHISHI_OK)
    return res;

  res = fclose (fh);
  if (res != 0)
    return SHISHI_FCLOSE_ERROR;

  if (VERBOSE (handle))
    printf (_("Writing KDC-REQ to %s...done\n"), filename);

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_parse:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for reading.
 * @kdcreq: output variable with newly allocated KDC-REQ.
 *
 * Read ASCII armored DER encoded KDC-REQ from file and populate given
 * variable.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_parse (Shishi * handle, FILE * fh, Shishi_asn1 * kdcreq)
{
  return _shishi_kdcreq_input (handle, fh, kdcreq, 0);
}

/**
 * shishi_kdcreq_read:
 * @handle: shishi handle as allocated by shishi_init().
 * @fh: file handle open for reading.
 * @kdcreq: output variable with newly allocated KDC-REQ.
 *
 * Read DER encoded KDC-REQ from file and populate given variable.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_read (Shishi * handle, FILE * fh, Shishi_asn1 * kdcreq)
{
  return _shishi_kdcreq_input (handle, fh, kdcreq, 1);
}

/**
 * shishi_kdcreq_from_file:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: output variable with newly allocated KDC-REQ.
 * @filetype: input variable specifying type of file to be read,
 *            see Shishi_filetype.
 * @filename: input variable with filename to read from.
 *
 * Read KDC-REQ from file in specified TYPE.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_from_file (Shishi * handle, Shishi_asn1 * kdcreq,
			 int filetype, char *filename)
{
  int res;
  FILE *fh;

  if (VERBOSE (handle))
    printf (_("Reading KDC-REQ from %s...\n"), filename);

  fh = fopen (filename, "r");
  if (fh == NULL)
    return SHISHI_FOPEN_ERROR;

  if (VERBOSE (handle))
    printf (_("Reading KDC-REQ in %s format...\n"),
	    filetype == SHISHI_FILETYPE_TEXT ? "TEXT" : "DER");

  if (filetype == SHISHI_FILETYPE_TEXT)
    res = shishi_kdcreq_parse (handle, fh, kdcreq);
  else
    res = shishi_kdcreq_read (handle, fh, kdcreq);
  if (res != SHISHI_OK)
    return res;

  res = fclose (fh);
  if (res != 0)
    return SHISHI_FCLOSE_ERROR;

  if (VERBOSE (handle))
    printf (_("Reading KDC-REQ from %s...done\n"), filename);

  return SHISHI_OK;
}

int
shishi_kdcreq_nonce (Shishi * handle, Shishi_asn1 kdcreq, uint32_t * nonce)
{
  int res;

  res = shishi_asn1_read_uint32 (handle, kdcreq, "req-body.nonce", nonce);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_set_cname:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set client name field in.
 * @name_type: type of principial, see Shishi_name_type, usually
 *             SHISHI_NT_UNKNOWN.
 * @principal: input array with principal name.
 *
 * Set the client name field in the KDC-REQ.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_set_cname (Shishi * handle,
			 Shishi_asn1 kdcreq,
			 Shishi_name_type name_type, const char *principal)
{
  int res;

  res = shishi_principal_set (handle, kdcreq, "req-body.cname", principal);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

int
shishi_kdcreq_cname_get (Shishi * handle,
			 Shishi_asn1 kdcreq, char *cname, size_t * cnamelen)
{
  return shishi_principal_name_get (handle, kdcreq,
				    "req-body.cname", cname, cnamelen);
}

int
shishi_asreq_cnamerealm_get (Shishi * handle,
			     Shishi_asn1 asreq,
			     char *cnamerealm, size_t * cnamerealmlen)
{
  return shishi_principal_name_realm_get (handle, asreq,
					  "req-body.cname", asreq,
					  "req-body.realm",
					  cnamerealm, cnamerealmlen);
}

int
shishi_kdcreq_realm_get (Shishi * handle, Shishi_asn1 kdcreq,
			 char *realm, size_t * realmlen)
{
  return shishi_asn1_read_optional (handle, kdcreq, "req-body.realm",
				    realm, realmlen);
}

/**
 * shishi_kdcreq_set_realm:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set realm field in.
 * @realm: input array with name of realm.
 *
 * Set the realm field in the KDC-REQ.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_set_realm (Shishi * handle, Shishi_asn1 kdcreq,
			 const char *realm)
{
  int res;

  res = shishi_asn1_write (handle, kdcreq, "req-body.realm", realm, 0);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

int
shishi_kdcreq_sname_get (Shishi * handle,
			 Shishi_asn1 kdcreq, char *sname, size_t * snamelen)
{
  return shishi_principal_name_get (handle, kdcreq,
				    "req-body.sname", sname, snamelen);
}

int
shishi_kdcreq_snamerealm_get (Shishi * handle,
			      Shishi_asn1 kdcreq,
			      char *snamerealm, size_t * snamerealmlen)
{
  return shishi_principal_name_realm_get (handle, kdcreq,
					  "req-body.sname", kdcreq,
					  "req-body.realm",
					  snamerealm, snamerealmlen);
}

/**
 * shishi_kdcreq_set_sname:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set server name field in.
 * @name_type: type of principial, see Shishi_name_type, usually
 *             SHISHI_NT_UNKNOWN.
 * @sname: input array with principal name.
 *
 * Set the server name field in the KDC-REQ.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_set_sname (Shishi * handle,
			 Shishi_asn1 kdcreq,
			 Shishi_name_type name_type, const char *sname[])
{
  int res;

  res = shishi_principal_name_set (handle, kdcreq, "req-body.sname",
				   name_type, sname);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

int
shishi_kdcreq_set_server (Shishi * handle, Shishi_asn1 req,
			  const char *server)
{
  int res;

  res = shishi_principal_set (handle, req, "req-body.sname", server);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

int
shishi_kdcreq_set_realmserver (Shishi * handle,
			       Shishi_asn1 req, char *realm, char *server)
{
  int res;

  res = shishi_kdcreq_set_realm (handle, req, realm);
  if (res != SHISHI_OK)
    return res;

  res = shishi_kdcreq_set_server (handle, req, server);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_etype:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to get etype field from.
 * @etype: output encryption type.
 * @netype: element number to return.
 *
 * Return the netype:th encryption type from KDC-REQ.  The first etype
 * is number 1.
 *
 * Return value: Returns SHISHI_OK iff etype successful set.
 **/
int
shishi_kdcreq_etype (Shishi * handle,
		     Shishi_asn1 kdcreq, int32_t * etype, int netype)
{
  char *buf;
  int res;

  asprintf (&buf, "req-body.etype.?%d", netype);
  res = shishi_asn1_read_int32 (handle, kdcreq, buf, etype);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_set_etype:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set etype field in.
 * @etype: input array with encryption types.
 * @netype: number of elements in input array with encryption types.
 *
 * Set the list of supported or wanted encryption types in the
 * request.  The list should be sorted in priority order.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_set_etype (Shishi * handle,
			 Shishi_asn1 kdcreq, int32_t * etype, int netype)
{
  int res;
  char *buf;
  int i;

  res = shishi_asn1_write (handle, kdcreq, "req-body.etype", NULL, 0);
  if (res != SHISHI_OK)
    return res;

  for (i = 1; i <= netype; i++)
    {
      res = shishi_asn1_write (handle, kdcreq, "req-body.etype", "NEW", 1);
      if (res != SHISHI_OK)
	return res;

      asprintf (&buf, "req-body.etype.?%d", i);
      res = shishi_asn1_write_int32 (handle, kdcreq, buf, etype[i - 1]);
      free (buf);
      if (res != SHISHI_OK)
	return res;
    }

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_options:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set etype field in.
 * @flags: pointer to output integer with flags.
 *
 * Extract KDC-Options from KDC-REQ.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_options (Shishi * handle, Shishi_asn1 kdcreq, uint32_t *flags)
{
  return shishi_asn1_read_bitstring (handle, kdcreq,
				     "req-body.kdc-options", flags);
}

/**
 * shishi_kdcreq_renewable_p:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set etype field in.
 *
 * Determine if KDC-Option renewable flag is set.
 *
 * The RENEWABLE option indicates that the ticket to be issued is to
 * have its RENEWABLE flag set. It may only be set on the initial
 * request, or when the ticket-granting ticket on which the request is
 * based is also renewable. If this option is requested, then the
 * rtime field in the request contains the desired absolute expiration
 * time for the ticket.
 *
 * Return value: Returns non-0 iff renewable flag is set in KDC-REQ.
 **/
int
shishi_kdcreq_renewable_p (Shishi * handle, Shishi_asn1 kdcreq)
{
  int options = 0;

  shishi_kdcreq_options (handle, kdcreq, &options);

  return options & SHISHI_KDCOPTIONS_RENEWABLE;
}

/**
 * shishi_kdcreq_options_set:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set etype field in.
 * @options: integer with flags to store in KDC-REQ.
 *
 * Set options in KDC-REQ.  Note that this reset any already existing
 * flags.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_options_set (Shishi * handle,
			   Shishi_asn1 kdcreq,
			   uint32_t options)
{
  int res;

  res = shishi_asn1_write_bitstring (handle, kdcreq,
				     "req-body.kdc-options", options);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_options_add:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ variable to set etype field in.
 * @option: integer with options to add in KDC-REQ.
 *
 * Add KDC-Option to KDC-REQ.  This preserves all existing options.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_options_add (Shishi * handle,
			   Shishi_asn1 kdcreq,
			   uint32_t option)
{
  int options;
  int res;

  res = shishi_kdcreq_options (handle, kdcreq, &options);
  if (res != SHISHI_OK)
    return res;

  options |= option;

  res = shishi_kdcreq_options_set (handle, kdcreq, options);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_clear_padata:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to remove PA-DATA from.
 *
 * Remove the padata field from KDC-REQ.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_clear_padata (Shishi * handle, Shishi_asn1 kdcreq)
{
  int res;

  res = shishi_asn1_write (handle, kdcreq, "padata", NULL, 0);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_get_padata:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to get PA-DATA from.
 * @padatatype: type of PA-DATA, see Shishi_padata_type.
 * @out: output array with newly allocated PA-DATA value.
 * @outlen: size of output array with PA-DATA value.
 *
 * Get pre authentication data (PA-DATA) from KDC-REQ.  Pre
 * authentication data is used to pass various information to KDC,
 * such as in case of a SHISHI_PA_TGS_REQ padatatype the AP-REQ that
 * authenticates the user to get the ticket.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_get_padata (Shishi * handle,
			  Shishi_asn1 kdcreq,
			  Shishi_padata_type padatatype,
			  char **out, size_t * outlen)
{
  char *format;
  int res;
  size_t i, n;

  res = shishi_asn1_number_of_elements (handle, kdcreq, "padata", &n);
  if (res != SHISHI_OK)
    return res;

  *out = NULL;
  *outlen = 0;

  for (i = 1; i <= n; i++)
    {
      int32_t patype;

      asprintf (&format, "padata.?%d.padata-type", i);
      res = shishi_asn1_read_int32 (handle, kdcreq, format, &patype);
      free (format);
      if (res != SHISHI_OK)
	return res;

      if (patype == (int32_t) padatatype)
	{
	  asprintf (&format, "padata.?%d.padata-value", i);
	  res = shishi_asn1_read2 (handle, kdcreq, format, out, outlen);
	  free (format);
	  if (res != SHISHI_OK)
	    return res;
	  break;
	}
    }

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_get_padata_tgs:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to get PA-TGS-REQ from.
 * @apreq: Output variable with newly allocated AP-REQ.
 *
 * Extract TGS pre-authentication data from KDC-REQ.  The data is an
 * AP-REQ that authenticates the request.  This function call
 * shishi_kdcreq_get_padata() with a SHISHI_PA_TGS_REQ padatatype and
 * DER decode the result (if any).
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_get_padata_tgs (Shishi * handle,
			      Shishi_asn1 kdcreq, Shishi_asn1 * apreq)
{
  char *der;
  size_t derlen;
  int rc;

  if (VERBOSE (handle))
    printf ("Extracting AP-REQ from KDC-REQ...\n");

  rc = shishi_kdcreq_get_padata (handle, kdcreq, SHISHI_PA_TGS_REQ,
				 &der, &derlen);
  if (rc != SHISHI_OK)
    return rc;

  *apreq = shishi_der2asn1_apreq (handle, der, derlen);
  if (!*apreq)
    return SHISHI_ASN1_ERROR;

  if (VERBOSEASN1 (handle))
    shishi_apreq_print (handle, stdout, *apreq);

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_add_padata:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to add PA-DATA to.
 * @padatatype: type of PA-DATA, see Shishi_padata_type.
 * @data: input array with PA-DATA value.
 * @datalen: size of input array with PA-DATA value.
 *
 * Add new pre authentication data (PA-DATA) to KDC-REQ.  This is used
 * to pass various information to KDC, such as in case of a
 * SHISHI_PA_TGS_REQ padatatype the AP-REQ that authenticates the user
 * to get the ticket.  (But also see shishi_kdcreq_add_padata_tgs()
 * which takes an AP-REQ directly.)
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_add_padata (Shishi * handle,
			  Shishi_asn1 kdcreq,
			  int padatatype, const char *data, size_t datalen)
{
  char *format;
  int res;
  int i;

  res = shishi_asn1_write (handle, kdcreq, "padata", "NEW", 1);
  if (res != SHISHI_OK)
    return res;

  res = shishi_asn1_number_of_elements (handle, kdcreq, "padata", &i);
  if (res != SHISHI_OK)
    return res;

  asprintf (&format, "padata.?%d.padata-value", i);
  res = shishi_asn1_write (handle, kdcreq, format, data, datalen);
  free (format);
  if (res != SHISHI_OK)
    return res;

  asprintf (&format, "padata.?%d.padata-type", i);
  res = shishi_asn1_write_uint32 (handle, kdcreq, format, padatatype);
  free (format);
  if (res != SHISHI_OK)
    return res;

  return SHISHI_OK;
}

/**
 * shishi_kdcreq_add_padata_tgs:
 * @handle: shishi handle as allocated by shishi_init().
 * @kdcreq: KDC-REQ to add PA-DATA to.
 * @apreq: AP-REQ to add as PA-DATA.
 *
 * Add TGS pre-authentication data to KDC-REQ.  The data is an AP-REQ
 * that authenticates the request.  This functions simply DER encodes
 * the AP-REQ and calls shishi_kdcreq_add_padata() with a
 * SHISHI_PA_TGS_REQ padatatype.
 *
 * Return value: Returns SHISHI_OK iff successful.
 **/
int
shishi_kdcreq_add_padata_tgs (Shishi * handle,
			      Shishi_asn1 kdcreq, Shishi_asn1 apreq)
{
  int res;
  char *data;
  size_t datalen;

  res = shishi_new_a2d (handle, apreq, &data, &datalen);
  if (res != SHISHI_OK)
    {
      shishi_error_printf (handle, "Could not DER encode AP-REQ: %s\n",
			   shishi_strerror (res));
      return res;
    }

  res = shishi_kdcreq_add_padata (handle, kdcreq,
				  SHISHI_PA_TGS_REQ, data, datalen);
  free (data);
  if (res != SHISHI_OK)
    return res;

  return res;
}

int
shishi_kdcreq_build (Shishi * handle, Shishi_asn1 kdcreq)
{
  int res;

  if (VERBOSE (handle))
    printf ("Building KDC-REQ...\n");

  if (shishi_asn1_empty_p (handle, kdcreq, "req-body.rtime"))
    {
      res = shishi_asn1_write (handle, kdcreq, "req-body.rtime", NULL, 0);
      if (res != SHISHI_OK)
	{
	  shishi_error_printf (handle, "Could not write rtime\n");
	  return res;
	}
    }

  if (shishi_asn1_empty_p (handle, kdcreq, "req-body.from"))
    {
      res = shishi_asn1_write (handle, kdcreq, "req-body.from", NULL, 0);
      if (res != SHISHI_OK)
	{
	  shishi_error_printf (handle, "Could not write from\n");
	  return res;
	}
    }

  return SHISHI_OK;
}
