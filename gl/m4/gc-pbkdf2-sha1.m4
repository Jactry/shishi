# gc-pbkdf2-sha1.m4 serial 1
dnl Copyright (C) 2005 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

AC_DEFUN([gl_GC_PBKDF2_SHA1],
[
  AC_LIBSOURCES([gc-pbkdf2-sha1.c])
  AC_LIBOBJ([gc-pbkdf2-sha1])
])
