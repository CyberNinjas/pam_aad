#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([pam_aad], [0.8], [info@cyberninjas.com])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.11 foreign])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE 

AC_USE_SYSTEM_EXTENSIONS
# Checks for programs.
AC_PROG_CC_C_O 
AM_PROG_AR
AC_LIBTOOL_WIN32_DLL
AC_DISABLE_DSTATIC
AC_PROG_LIBTOOL

AC_ARG_ENABLE([man], 
[AS_HELP_STRING([--disable-man], [Disable man page generation])],
[:],
[enable_man=yes]
)


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

