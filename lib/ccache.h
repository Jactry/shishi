/* ccache.h --- Read MIT style Kerberos Credential Cache file.
 * Copyright (C) 2006  Simon Josefsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */

#ifndef CCACHE_H
#define CCACHE_H 1

#include <stdint.h>
#include <string.h>

#define CCACHE_MAX_COMPONENTS 5

struct ccache_header
{
  uint16_t tag;
  uint16_t taglen;
  char *tagdata;
};

struct ccache_buffer
{
  uint32_t length;
  char *data;
};

struct ccache_principal
{
  uint32_t name_type;
  uint32_t num_components;
  struct ccache_buffer realm;
  struct ccache_buffer components[CCACHE_MAX_COMPONENTS];
};

struct ccache_keyblock
{
  uint16_t keytype;
  uint16_t etype;
  uint16_t keylen;
  void *keyvalue;
};

struct ccache_credential
{
  struct ccache_principal client;
  struct ccache_principal server;
  struct ccache_keyblock key;
  uint32_t authtime;
  uint32_t starttime;
  uint32_t endtime;
  uint32_t renew_till;
  uint8_t is_skey;
  uint32_t tktflags;
  struct ccache_buffer ticket;
  struct ccache_buffer second_ticket;
};

struct ccache
{
  uint16_t file_format_version;
  uint16_t headerlen;
  void *header;
  struct ccache_principal default_principal;
  size_t credentialslen;
  void *credentials;
};

extern int ccache_parse (const void *data, size_t length, struct ccache *out);

extern int ccache_parse_credential (const void *data, size_t len,
				    struct ccache_credential *out,
				    size_t * n);

extern void ccache_print (struct ccache *ccache);
extern void ccache_print_principal (struct ccache_principal *princ);
extern void ccache_print_credential (struct ccache_credential *cred);

#endif /* CCACHE_H */