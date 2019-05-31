dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.65])
AC_INIT([pam_aad], [0.0.2], [info@cyberninjas.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.11 foreign subdir-objects -Wall -Werror])
AM_SILENT_RULES([yes])
AC_USE_SYSTEM_EXTENSIONS

dnl Checks for programs.
AC_PROG_CC_C_O 
AM_PROG_AR
AC_LIBTOOL_WIN32_DLL
AC_DISABLE_STATIC

LT_INIT([disable-static])
AC_ENABLE_STATIC([no])
AC_ENABLE_SHARED([yes])

AC_ARG_ENABLE([man], 
	      [AS_HELP_STRING([--disable-man], [Disable man page generation])],
	      [:],
	      [enable_man=yes]
	      )

AC_SUBST(PAMDIR, "\$(exec_prefix)/lib/security")
AC_ARG_WITH(pam-dir,
	    AC_HELP_STRING([--with-pam-dir=DIR],
			   [Where to install PAM module [[PREFIX/lib/security]]]),
			   [case "${withval}" in
			    /*) PAMDIR="${withval}";;
			    ./*|../*) AC_MSG_ERROR(Bad value for --with-pam-dir);;
			    *)  PAMDIR="\$(exec_prefix)/lib/${withval}";;
		    esac])
AC_MSG_NOTICE([PAM installation path $PAMDIR])

dnl Checks for libraries.
AC_CHECK_LIB([jwt], [jwt_set_alg])
AC_CHECK_LIB([pam], [pam_get_user])
AC_CHECK_LIB([ssl], [SSL_library_init])

dnl Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h syslog.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_CHECK_FUNCS([floor memset pow strchr strdup strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([Summary of build options:

	       Version:            ${VERSION}
	       Host type:          ${host}
	       Install prefix:     ${prefix}
	       Compiler:           ${CC}
	       Library types:      Shared=${enable_shared}, Static=${enable_static}
	       PAM:                ${PAMDIR}
	       ])
