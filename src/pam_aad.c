#include <cjson/cJSON.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT

#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

#if !defined(LOG_AUTHPRIV) && defined(LOG_AUTH)
#define LOG_AUTHPRIV LOG_AUTH
#endif

#ifdef sun
#define PAM_CONST
#else
#define PAM_CONST const
#endif

#define MODULE_NAME "pam_azure_authenticator"
#define CODE_PROMPT "Enter the following code at https://aka.ms/devicelogin : "

typedef struct Params {
    int echocode;
    int allowed_perm;
    int debug;
    const char *tenant;
    const char *client_id;
    const char *required_group_id;
} Params;

int azure_token_validate(char *raw_token);
int fill_json_buffer(char *json_buf, char *raw_response, int *start,
		     int *end);
int find_json_bounds(char *json_buf, int *start, int *end);
int get_microsoft_graph_groups(char *user_object_id, char *response_buf,
			       char *token, const char *tenant,
			       const char *group_object_id);
int get_microsoft_graph_userprofile(char *token, char *response_buf,
				    const char *tenant);
int jwt_username_matches(char *raw_token, const char *claimed_username);
int poll_microsoft_for_token(char *code, const char *client_id,
			     char *response_buf);
int read_code_from_microsoft(const char *client_id, const char *tenant,
			     char *response_buf);

static void log_message(int priority, pam_handle_t * pamh,
			const char *format, ...)
{
    char logname[80];
    snprintf(logname, sizeof(logname), "%s(" MODULE_NAME ")");

    va_list args;
    va_start(args, format);
#if !defined(DEMO) && !defined(TESTING)
    openlog(logname, LOG_CONS | LOG_PID, LOG_AUTHPRIV);
    vsyslog(priority, format, args);
    closelog();
#else
    if (!error_msg) {
	error_msg = strdup("");
    }
    {
	char buf[1000];
	vsnprintf(buf, sizeof buf, format, args);
	const int newlen = strlen(error_msg) + 1 + strlen(buf) + 1;
	char *n = malloc(newlen);
	if (n) {
	    snprintf(n, newlen, "%s%s%s", error_msg,
		     strlen(error_msg) ? "\n" : "", buf);
	    free(error_msg);
	    error_msg = n;
	} else {
	    fprintf(stderr, "Failed to malloc %d bytes for log data.\n",
		    newlen);
	}
    }
#endif

    va_end(args);
}

static int converse(pam_handle_t * pamh, int nargs,
		    PAM_CONST struct pam_message **message,
		    struct pam_response **response)
{
    struct pam_conv *conv;
    int retval = pam_get_item(pamh, PAM_CONV, (void *) &conv);
    if (retval != PAM_SUCCESS) {
	return retval;
    }
    return conv->conv(nargs, message, response, conv->appdata_ptr);
}

static const char *get_user_name(pam_handle_t * pamh,
				 const Params * params)
{
    //Obtain user's name 
    const char *username;
    if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS ||
	!username || !*username) {
	log_message(LOG_ERR, pamh,
		    "pam_get_user() failed to get a user name");
	return NULL;
    }
    if (params->debug) {
	log_message(LOG_INFO, pamh,
		    "debug: start of azure_authenticator for %s",
		    username);
    }
    return username;
}

int azure_token_user_match(const char *claimed_username, char *token)
{
    return jwt_username_matches(token, claimed_username);
}

/*
 * Function: request_token
 * -----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *client_id: char array containing the client id 
 */

int request_token(char *code, const char *client_id, char *token_buf)
{
    int start, end;
    char response_buf[9000];
    char json_buf[9000];
    cJSON *json, *access_token;
    poll_microsoft_for_token(code, client_id, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    access_token = cJSON_GetObjectItem(json, "access_token");
    if (access_token == NULL) {
	/* Something failed. */
	strcpy(token_buf, "FAILURE");
	return EXIT_FAILURE;
    }
    strcpy(token_buf,
	   cJSON_GetObjectItem(json, "access_token")->valuestring);
    return EXIT_SUCCESS;
}

static int parse_user_groups(char *response_buf,
			     cJSON * group_membership_value)
{
    char json_buf[4000];
    int start, end;
    cJSON *json;
    strcat(response_buf, "\0");
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    if (json == NULL) {
	return EXIT_FAILURE;
    }
    int checkval = cJSON_GetObjectItem(json, "value")->type;
    return checkval;
}


static int parse_user_object_id(char *response_buf,
				char *user_object_id_buf)
{
    char json_buf[4000];
    int start, end;
    cJSON *json;

    find_json_bounds(response_buf, &start, &end);*

    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    cJSON *object = cJSON_GetObjectItem(json, "objectId");
    if (object == NULL) {
	/* Something failed. */
	printf("Something failed.\n");
	return EXIT_FAILURE;
    }
    strcpy(user_object_id_buf,
	   cJSON_GetObjectItem(json, "objectId")->valuestring);

    return EXIT_SUCCESS;
}

/*
 * Function: *request_code
 *-----------------------------------
 * *code: character buffer that will have the code inside of it by the function's end.
 *
 * *client_id: contains client id of application as registered with Azure.
 *
 * *tenant: the MS tenant. 
 * 
 * TODO: Improve checking if this function succeeded. Should be some more error 
 * handling and there will need to be some way to log failures. 
*/
static int request_code(char *user_code, const char *client_id,
			const char *tenant, char *device_code)
{
    char response_buf[2048];
    char json_buf[2048];
    cJSON *json;
    int start, end;

    read_code_from_microsoft(client_id, tenant, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    strcpy(user_code, cJSON_GetObjectItem(json, "user_code")->valuestring);
    strcpy(device_code,
	   cJSON_GetObjectItem(json, "device_code")->valuestring);
    if (user_code[0] == '\0' || device_code[0] == '\0') {
	/* string is empty, we have failed somewhere */
	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int azure_user_in_group(char *token,
			       const char *required_group_id,
			       const char *tenant)
{
//    int success = 2;
    char user_profile_buf[1000];
    char user_object_id_buf[100];
    char raw_group_buf[10000];
//    cJSON *group_membership_value; 

    get_microsoft_graph_userprofile(token, user_profile_buf, tenant);
    parse_user_object_id(user_profile_buf, user_object_id_buf);
    get_microsoft_graph_groups(user_object_id_buf, raw_group_buf, token,
			       tenant, required_group_id);
//     int is_in_group = parse_user_groups(raw_group_buf, group_membership_value);
//     if(is_in_group == success){
//         return EXIT_SUCCESS;
//     }
    return EXIT_FAILURE;
}

int request_azure_auth(pam_handle_t * pamh, int echocode,
		       const char *client_id, const char *tenant,
		       char *token_buf)
{
    char prompt[1000], code[1000], code_buf[1000], device_code[10000];
    strcpy(prompt, CODE_PROMPT);
    request_code(code_buf, client_id, tenant, device_code);
    strcpy(code, code_buf);
    strcat(prompt, code);
    strcat(prompt, "\nPlease hit enter after you have logged in.");
    PAM_CONST char *message = prompt;
    PAM_CONST struct pam_message msg = {.msg_style = echocode,
	.msg = message
    };
    PAM_CONST struct pam_message *msgs = &msg;
    struct pam_response *resp = NULL;
    int retval = converse(pamh, 1, &msgs, &resp);
    request_token(device_code, client_id, token_buf);
    if (azure_token_validate(token_buf) == 0) {
	return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

static int parse_args(pam_handle_t * pamh, int argc, const char **argv,
		      Params * params)
{
    params->debug = 1;
    params->echocode = PAM_PROMPT_ECHO_ON;
    int i;
    for (i = 0; i < argc; ++i) {
	if (!memcmp(argv[i], "client_id=", 10)) {
	    params->client_id = argv[i] + 10;
	} else if (!memcmp(argv[i], "tenant=", 7)) {
	    params->tenant = argv[i] + 7;
	} else if (!memcmp(argv[i], "required_group_id=", 18)) {
	    params->required_group_id = argv[i] + 18;
	} else {
	    log_message(LOG_ERR, pamh, "Unrecognized option \"%s\"",
			argv[i]);
	    return EXIT_FAILURE;
	}
    }
    return EXIT_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t * pamh, int flags,
				   int argc, const char **argv)
{
    const char *username;
    int valid_token;
    char token_buf[3000];

    Params params = { 0 };
    params.allowed_perm = 0600;
    if (parse_args(pamh, argc, argv, &params) < 0) {
	return PAM_AUTH_ERR;
    }

    username = get_user_name(pamh, &params);
    int auth = request_azure_auth(pamh, params.echocode,
				  params.client_id, params.tenant,
				  token_buf);
    if (auth == 0 && azure_token_user_match(username, token_buf) == 0) {
	return PAM_AUTH_ERR;
    }
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t * pamh, int flags, int argc,
			      const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t * pamh, int flags, int argc,
				const char **argv)
{
    return PAM_SUCCESS;
}
