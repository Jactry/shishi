/* key.c --- Shishi KEY self tests.
 * Copyright (C) 2002, 2003, 2004  Simon Josefsson
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

#include "utils.c"

void
test (Shishi * handle)
{
  Shishi_key *key;
  char *p;
  int res;

  res = shishi_key_from_base64 (handle,
				SHISHI_AES128_CTS_HMAC_SHA1_96,
				"Zm5vcmRmbm9yZGZub3JkZg==", &key);
  if (debug)
    printf ("shishi_key_from_base64 => `%p'.\n", key);
  if (res == SHISHI_OK)
    success ("shishi_key_from_base64 OK\n");
  else
    fail ("shishi_key_from_base64 failed\n");

  p = shishi_key_value (key);
  if (!p)
    fail ("shishi_key_value failed\n");
  else
    {
      if (memcmp (p, "fnordfnordfnordf", shishi_key_length (key)) == 0)
	success ("shishi_key_value OK\n");
      else
	fail ("shishi_key_value mismatch\n");
    }

}