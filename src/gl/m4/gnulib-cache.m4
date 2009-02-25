# Copyright (C) 2002-2009 Free Software Foundation, Inc.
#
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
#   gnulib-tool --import --dir=. --lib=libgnu --source-base=src/gl --m4-base=src/gl/m4 --doc-base=doc --tests-base=tests --aux-dir=build-aux --avoid=extensions --avoid=gettext --avoid=gettext-h --avoid=include_next --avoid=intprops --avoid=stdarg --avoid=string --avoid=unistd --makefile-name=gnulib.mk --libtool --macro-prefix=gl2 --no-vc-files error getopt locale progname version-etc

# Specification in the form of a few gnulib-tool.m4 macro invocations:
gl_LOCAL_DIR([])
gl_MODULES([
  error
  getopt
  locale
  progname
  version-etc
])
gl_AVOID([extensions gettext gettext-h include_next intprops stdarg string unistd])
gl_SOURCE_BASE([src/gl])
gl_M4_BASE([src/gl/m4])
gl_PO_BASE([])
gl_DOC_BASE([doc])
gl_TESTS_BASE([tests])
gl_LIB([libgnu])
gl_MAKEFILE_NAME([gnulib.mk])
gl_LIBTOOL
gl_MACRO_PREFIX([gl2])
gl_PO_DOMAIN([])
gl_VC_FILES([false])
