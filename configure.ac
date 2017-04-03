AC_INIT(pam-azure-active-directory, 0.8, info@cyberninjas.com)
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC

AC_CHECK_HEADERS_ONCE([security/pam_appl.h])
#On solaris, <security/pam_modules.h> requres <security/pam_appl.h> to be included first.
AC_CHECK_HEADER([security/pam_modules.h], [], [], \
		[#ifdef HAVE_SECURITY_PAM_APPLY_H
		# include <security/pam_apply.h>
		#endif
		])

AC_CHECK_LIB([pam], [pam_get_user], [:])

AS_IF([test "x$ac_cv_header_security_pam_modules_h" = "xno" \
	-o "x$ac_cv_lib_pam_pam_get_user" = "xno"], [
	AC_MSG_ERROR([Unable to find the PAM library or the PAM header files])
])


AC_MSG_CHECKING([whether certain PAM functions require const arguments]) 
AC_LANG_PUSH(C)
AC_LANG_WERROR
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
    #include <security/pam_appl.h>
    #include <security/pam_modules.h>
]],[[
    const void **item = 0;
    int dummy = 0;
    const pam_handle_t *ph = (const pam_handle_t *)&dummy;
    (void) pam_get_item(ph, 0, item);
]])],[AC_DEFINE([PAM_CONST], [const], \
        [Prevent certain PAM functions from using const arguments])
             AC_MSG_RESULT([yes])],
        [AC_DEFINE([PAM_CONST], [], \
        [Prevent certain PAM functions from using const argument])
        AC_MSG_RESULT([no])])
AC_LANG_POP(C)

#Check for OpenSSL
AC_MSG_CHECKING([Checking whether OpenSSL development package is installed...])
AC_MSG_CHECKING([for --with-openssl])
AC_ARG_WITH(
	[openssl],
	[AS_HELP_STRING[--with-openssl],[/absolute/path/to/openssl-base])],
	[ LDFLAGS="$LDFLAGS -L$with_openssl/lib";
	  CFLAGS="$CFLAGS -I$with_openssl/include/";
	  LIBS="-lssl";
	  AC_MSG_RESULT([$with_openssl])
	],
	[AC_MSG_RESULT([no])]
)

AC_CHECK_LIB([crypto],[CRYPTO_new_ex_data], [], [AC_MSG_ERROR([OpenSSL libraries required])])
AC_CHECK_LIB([ssl], [SSL_library_init], [], [AC_MSG_ERROR([OpenSSL libraries required])])

AC_CHECK_HEADERS([openssl/crypto.h openssl/x509.h openssl/pem.h openssl/ssl.h openssl/err.h], [], [AC_MSG_ERROR([OpenSSL headers required])])

#### Check for libcurl
LIBCURL_CHECK_CONFIG([yes], [], [], [AC_MSG_ERROR([libcurl development files required])])

### Check for math
AC_CHECK_LIB([m], pow, [], [AC_MSG_ERROR([Math libraries required - libm not found])])
libm=$ac_status
AC_CHECK_HEADERS([math.h], [], [AC_MSG_ERROR([Math headers required])])


### Check for jwt 
AC_CHECK_LIB([jwt], jwt_set_alg, [], [AC_MSG_ERROR([JWT libraries required])])

AC_CONFIG_FILES(Makefile)
AC_OUTPUT

echo "
  $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debug
  C Compiler.....: $CC $CFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
"
