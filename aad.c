#define PAM_SM_AUTH
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define MODULE_NAME "pam_azure_authenticator"
#define CODE_PROMPT "Enter the following code at login"

static 

static int azure_authenticator(pam_handle_t *pamh, int flags,
                               int argc, const char **argv){
  const char *username;
  const char *prompt = CODE_PROMPT

  username = get_user_name(pamh, &params);
  pw = request_pass(pamh, params.echocode, prompt);
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags,
                                   int argc, const char **argv){
  return azure_authenticator(pamh, flags, arc, argv);                                
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc,
                                    const char **argv){
  return PAM_SUCCESS;
}

