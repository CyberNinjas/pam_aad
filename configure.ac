#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pam_aad], [0.8], [info@cyberninjas.com])
AC_CONFIG_SRCDIR([src/aad.c])
AC_CONFIG_HEADERS([config.h])
AC_DISABLE_STATIC
AC_ENABLE_SHARED
LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([crypto], [CRYPTO_new_ex_data])
AC_CHECK_LIB([jwt], [jwt_set_alg])
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([pam], [pam_get_user])
AC_CHECK_LIB([ssl], [SSL_library_init])

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor memset pow strchr strdup strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
  $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debug
  C Compiler.....: $CC $CFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
"

