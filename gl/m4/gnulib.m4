# Copyright (C) 2004 Free Software Foundation, Inc.
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# Invoked as: gnulib-tool --import
# Reproduce by: gnulib-tool --import --dir=. --lib=libgnu --source-base=gl --m4-base=gl/m4 --libtool  alloca alloca-opt allocsa error extensions getaddrinfo getdate getdomainname gethostname getline getopt getpass getsubopt gettext gettime gettimeofday mktime progname readlink realloc restrict setenv stdbool strcase strchrnul strdup strndup strnlen timegm time_r timespec vasnprintf vasprintf xalloc xgetdomainname xgethostname xreadlink xsize xstrndup

AC_DEFUN([gl_EARLY],
[
  AC_GNU_SOURCE
  gl_USE_SYSTEM_EXTENSIONS
])

AC_DEFUN([gl_INIT],
[
  gl_FUNC_ALLOCA
changequote(,)dnl
LTALLOCA=`echo "$ALLOCA" | sed 's/\.[^.]* /.lo /g;s/\.[^.]*$/.lo/'`
changequote([, ])dnl
AC_SUBST(LTALLOCA)
  gl_FUNC_ALLOCA
  gl_ALLOCSA
  gl_ERROR
  dnl gl_USE_SYSTEM_EXTENSIONS must be added quite early to configure.ac.
  gl_GETADDRINFO
  gl_GETDATE
  gl_FUNC_GETDOMAINNAME
  gl_FUNC_GETHOSTNAME
  AM_FUNC_GETLINE
  gl_GETOPT
  gl_FUNC_GETPASS
  gl_FUNC_GETSUBOPT
  dnl you must add AM_GNU_GETTEXT([external]) or similar to configure.ac.
  gl_GETTIME
  AC_FUNC_GETTIMEOFDAY_CLOBBER
  gl_FUNC_MKTIME
  gl_FUNC_READLINK
  AC_FUNC_REALLOC
  gl_C_RESTRICT
  gt_FUNC_SETENV
  AM_STDBOOL_H
  gl_STRCASE
  gl_FUNC_STRCHRNUL
  gl_FUNC_STRDUP
  gl_FUNC_STRNDUP
  gl_FUNC_STRNLEN
  gl_FUNC_TIMEGM
  gl_TIME_R
  gl_TIMESPEC
  gl_FUNC_VASNPRINTF
  gl_FUNC_VASPRINTF
  gl_XALLOC
  gl_XREADLINK
  gl_XSIZE
  gl_XSTRNDUP
])

dnl Usage: gl_MODULES(module1 module2 ...)
AC_DEFUN([gl_MODULES], [])

dnl Usage: gl_SOURCE_BASE(DIR)
AC_DEFUN([gl_SOURCE_BASE], [])

dnl Usage: gl_M4_BASE(DIR)
AC_DEFUN([gl_M4_BASE], [])

dnl Usage: gl_LIB(LIBNAME)
AC_DEFUN([gl_LIB], [])

dnl Usage: gl_LGPL
AC_DEFUN([gl_LGPL], [])

# gnulib.m4 ends here
