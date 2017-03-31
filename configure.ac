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

AC_CONFIG_FILES(Makefile)
AC_OUTPUT

echo "
  $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  Debug Build....: $debug
  C Compiler.....: $CC $CFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
"
