//Unittest for the Azure PAM module.
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#if !defined(PAM_BAD_ITEM)
//FreeBSD does not know about PAM_BAD_ITEM. And PAM_SYMBOL_ERR is an "enum",
// we can only test for this at run-time
#define PAM_BAD_ITEM PAM_SYMBOL_ERR
#endif

