/*
 * Copyright (C) 2002, 2005, 2006, 2008, 2009, 2010 Free Software
 * Foundation, Inc.
 *
 * This file is part of LIBTASN1.
 *
 * The LIBTASN1 library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include <int.h>
#ifdef STDC_HEADERS
# include <stdarg.h>
#endif

#define LIBTASN1_ERROR_ENTRY(name) { #name, name }

struct libtasn1_error_entry
{
  const char *name;
  int number;
};
typedef struct libtasn1_error_entry libtasn1_error_entry;

static const libtasn1_error_entry error_algorithms[] = {
  LIBTASN1_ERROR_ENTRY (ASN1_SUCCESS),
  LIBTASN1_ERROR_ENTRY (ASN1_FILE_NOT_FOUND),
  LIBTASN1_ERROR_ENTRY (ASN1_ELEMENT_NOT_FOUND),
  LIBTASN1_ERROR_ENTRY (ASN1_IDENTIFIER_NOT_FOUND),
  LIBTASN1_ERROR_ENTRY (ASN1_DER_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_VALUE_NOT_FOUND),
  LIBTASN1_ERROR_ENTRY (ASN1_GENERIC_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_VALUE_NOT_VALID),
  LIBTASN1_ERROR_ENTRY (ASN1_TAG_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_TAG_IMPLICIT),
  LIBTASN1_ERROR_ENTRY (ASN1_ERROR_TYPE_ANY),
  LIBTASN1_ERROR_ENTRY (ASN1_SYNTAX_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_MEM_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_MEM_ALLOC_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_DER_OVERFLOW),
  LIBTASN1_ERROR_ENTRY (ASN1_NAME_TOO_LONG),
  LIBTASN1_ERROR_ENTRY (ASN1_ARRAY_ERROR),
  LIBTASN1_ERROR_ENTRY (ASN1_ELEMENT_NOT_EMPTY),
  {0, 0}
};

/**
 * asn1_perror - prints a string to stderr with a description of an error
 * @error: is an error returned by a libtasn1 function.
 *
 * This function is like perror().  The only difference is that it
 * accepts an error returned by a libtasn1 function.
 *
 * This function replaces libtasn1_perror() in older libtasn1.
 *
 * Since: 1.6
 **/
void
asn1_perror (asn1_retCode error)
{
  const char *str = asn1_strerror (error);
  fprintf (stderr, "LIBTASN1 ERROR: %s\n", str ? str : "(null)");
}

/**
 * asn1_strerror - Returns a string with a description of an error
 * @error: is an error returned by a libtasn1 function.
 *
 * This function is similar to strerror.  The only difference is that
 * it accepts an error (number) returned by a libtasn1 function.
 *
 * This function replaces libtasn1_strerror() in older libtasn1.
 *
 * Returns: Pointer to static zero-terminated string describing error
 *   code.
 *
 * Since: 1.6
 **/
const char *
asn1_strerror (asn1_retCode error)
{
  const libtasn1_error_entry *p;

  for (p = error_algorithms; p->name != NULL; p++)
    if (p->number == error)
      return p->name + sizeof ("ASN1_") - 1;

  return NULL;
}

#ifndef ASN1_DISABLE_DEPRECATED

/* Compatibility mappings to preserve ABI. */

/**
 * libtasn1_perror - prints a string to stderr with a description of an error
 * @error: is an error returned by a libtasn1 function.
 *
 * This function is like perror(). The only difference is that it
 * accepts an error returned by a libtasn1 function.
 *
 * Deprecated: Use asn1_perror() instead.
 **/
void
libtasn1_perror (asn1_retCode error)
{
  asn1_perror (error);
}

/**
 * libtasn1_strerror - Returns a string with a description of an error
 * @error: is an error returned by a libtasn1 function.
 *
 * This function is similar to strerror.  The only difference is that
 * it accepts an error (number) returned by a libtasn1 function.
 *
 * Returns: Pointer to static zero-terminated string describing error
 *   code.
 *
 * Deprecated: Use asn1_strerror() instead.
 **/
const char *
libtasn1_strerror (asn1_retCode error)
{
  return asn1_strerror (error);
}

#endif
