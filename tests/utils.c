/* utils.c --- Shishi self tests utilities.
 * Copyright (C) 2002, 2003, 2004, 2006, 2007, 2008  Simon Josefsson
 *
 * This file is part of Shishi.
 *
 * Shishi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shishi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shishi; if not, see http://www.gnu.org/licenses or write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#if defined HAVE_DECL_H_ERRNO && !HAVE_DECL_H_ERRNO
/* extern int h_errno; */
#endif

#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <signal.h>

#include <netinet/in.h>
#ifdef HAVE_NETINET_IN6_H
#include <netinet/in6.h>
#endif

#include "base64.h"

#include <shishi.h>

const char *program_name = PACKAGE;

static int verbose = 0;
static int debug = 0;
static int error_count = 0;
static int break_on_error = 0;

static void
fail (const char *format, ...)
{
  va_list arg_ptr;

  va_start (arg_ptr, format);
  vfprintf (stderr, format, arg_ptr);
  va_end (arg_ptr);
  error_count++;
  if (break_on_error)
    exit (1);
}

static void
success (const char *format, ...)
{
  va_list arg_ptr;

  va_start (arg_ptr, format);
  if (verbose)
    vfprintf (stdout, format, arg_ptr);
  va_end (arg_ptr);
}

static void
escapeprint (const char *str, int len)
{
  int i;

  if (!str || !len)
    return;

  printf ("\t ;; `");
  for (i = 0; i < len; i++)
    if ((str[i] >= 'A' && str[i] <= 'Z') ||
	(str[i] >= 'a' && str[i] <= 'z') ||
	(str[i] >= '0' && str[i] <= '9') || str[i] == '.')
      printf ("%c", str[i]);
    else
      printf ("\\x%02x", str[i]);
  printf ("' (length %d bytes)\n", len);
}

static void
hexprint (const char *str, int len)
{
  int i;

  if (!str || !len)
    return;

  printf ("\t ;; ");
  for (i = 0; i < len; i++)
    {
      printf ("%02x ", str[i]);
      if ((i + 1) % 8 == 0)
	printf (" ");
      if ((i + 1) % 16 == 0 && i + 1 < len)
	printf ("\n\t ;; ");
    }
  puts ("");
}

static void
binprint (const char *str, int len)
{
  int i;

  if (!str || !len)
    return;

  printf ("\t ;; ");
  for (i = 0; i < len; i++)
    {
      printf ("%d%d%d%d%d%d%d%d ",
	      str[i] & 0x80 ? 1 : 0,
	      str[i] & 0x40 ? 1 : 0,
	      str[i] & 0x20 ? 1 : 0,
	      str[i] & 0x10 ? 1 : 0,
	      str[i] & 0x08 ? 1 : 0,
	      str[i] & 0x04 ? 1 : 0,
	      str[i] & 0x02 ? 1 : 0, str[i] & 0x01 ? 1 : 0);
      if ((i + 1) % 3 == 0)
	printf (" ");
      if ((i + 1) % 6 == 0 && i + 1 < len)
	printf ("\n\t ;; ");
    }
  puts ("");
}

void test (Shishi * handle);

int
main (int argc, char *argv[])
{
  Shishi *handle;

  do
    if (strcmp (argv[argc - 1], "-v") == 0 ||
	strcmp (argv[argc - 1], "--verbose") == 0)
      verbose = 1;
    else if (strcmp (argv[argc - 1], "-d") == 0 ||
	     strcmp (argv[argc - 1], "--debug") == 0)
      debug = 1;
    else if (strcmp (argv[argc - 1], "-b") == 0 ||
	     strcmp (argv[argc - 1], "--break-on-error") == 0)
      break_on_error = 1;
    else if (strcmp (argv[argc - 1], "-h") == 0 ||
	     strcmp (argv[argc - 1], "-?") == 0 ||
	     strcmp (argv[argc - 1], "--help") == 0)
      {
	printf ("Usage: %s [-vdbh?] [--verbose] [--debug] "
		"[--break-on-error] [--help]\n", argv[0]);
	return 1;
      }
  while (argc-- > 1);

  handle = shishi ();
  if (handle == NULL)
    {
      fail ("Could not initialize shishi\n");
      return 1;
    }

  if (debug)
    {
      shishi_cfg (handle, strdup ("verbose"));
      shishi_cfg (handle, strdup ("verbose-noise"));
      shishi_cfg (handle, strdup ("verbose-asn1"));
      shishi_cfg (handle, strdup ("verbose-crypto"));
      shishi_cfg (handle, strdup ("verbose-crypto-noise"));
    }

  test (handle);

  shishi_done (handle);

  if (verbose)
    printf ("Self test `%s' done with %d errors\n", argv[0], error_count);

  return error_count ? 1 : 0;
}
