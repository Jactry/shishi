# Copyright (C) 2004 Free Software Foundation, Inc.
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the specification of how gnulib-tool is used.
# It acts as a cache: It is written and read by gnulib-tool.
# In projects using CVS, this file is meant to be stored in CVS,
# like the configure.ac and various Makefile.am files.


# Specification in the form of a command-line invocation:
#   gnulib-tool --import --dir=. --lib=libgnu --source-base=gl --m4-base=gl/m4 --aux-dir=. --avoid=xalloc-die --libtool --macro-prefix=gl base64 crc error gc-arcfour gc-des gc-hmac-md5 gc-hmac-sha1 gc-md4 gc-md5 gc-pbkdf2-sha1 gc-random getaddrinfo getdate gethostname getline getopt getpass getsubopt maintainer-makefile progname realloc setenv socklen strcase strchrnul strdup strndup strtok_r timegm vasnprintf vasprintf xalloc xgetdomainname xgethostname xreadlink xstrndup xvasprintf

# Specification in the form of a few gnulib-tool.m4 macro invocations:
gl_MODULES([base64 crc error gc-arcfour gc-des gc-hmac-md5 gc-hmac-sha1 gc-md4 gc-md5 gc-pbkdf2-sha1 gc-random getaddrinfo getdate gethostname getline getopt getpass getsubopt maintainer-makefile progname realloc setenv socklen strcase strchrnul strdup strndup strtok_r timegm vasnprintf vasprintf xalloc xgetdomainname xgethostname xreadlink xstrndup xvasprintf])
gl_AVOID([xalloc-die])
gl_SOURCE_BASE([gl])
gl_M4_BASE([gl/m4])
gl_TESTS_BASE([gl/tests])
gl_LIB([libgnu])
gl_LIBTOOL
gl_MACRO_PREFIX([gl])
