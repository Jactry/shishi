# Generated by gnulib-tool.
#
# gnulib-tool --import --dir=. \
#   --source-base=gl --m4-base=gl/m4  \
#   --libtool \
#   error getopt realloc setenv strcase strchrnul strnlen strdup strndup xstrndup vasprintf vasnprintf xalloc getdate timegm gethostname xgethostname xgetdomainname getline xreadlink progname getsubopt

AC_DEFUN([gl_EARLY],
[
  AC_GNU_SOURCE
  gl_USE_SYSTEM_EXTENSIONS
])

AC_DEFUN([gl_INIT],
[
  gl_FUNC_ALLOCA
  gl_ALLOCSA
  gl_ERROR
  gl_EXITFAIL
  dnl gl_USE_SYSTEM_EXTENSIONS must be added quite early to configure.ac.
  gl_GETDATE
  gl_FUNC_GETDOMAINNAME
  gl_FUNC_GETHOSTNAME
  AM_FUNC_GETLINE
  gl_GETOPT
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
  gl_FUNC_GLIBC_UNLOCKED_IO
  gl_FUNC_VASNPRINTF
  gl_FUNC_VASPRINTF
  gl_XALLOC
  gl_XREADLINK
  gl_XSIZE
  gl_XSTRNDUP
])