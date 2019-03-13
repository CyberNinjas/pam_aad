#include <curl/curl.h>
#include <jansson.h>
#include <jwt.h>
#include <sds/sds.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define AUTH_ERROR "authorization_pending"
#define CODE_PROMPT "Enter the following code at https://aka.ms/devicelogin: "
#define CONFIG_FILE "/etc/pam-test.conf"
#define HOST "https://login.microsoftonline.com/"
#define RESOURCE_ID "00000002-0000-0000-c000-000000000000"
#define TTW 5
#define USER_AGENT "azure_authenticator_pam/1.0"

struct response 
{
	char *data;
	size_t size;
};

struct ret_data
{
	const char *u_code, *d_code, *auth_bearer;
};

static size_t response_callback(void *contents, size_t size, size_t nmemb,
				void *userp)
{
	size_t realsize = size * nmemb;
	struct response *resp = (struct response *)userp;
	char *ptr = realloc(resp->data, resp->size + realsize + 1);
	if (ptr == NULL) {
		// Out of memory
		printf("Not enough memory (realloc returned NULL)\n");

		return 0;
	}

	resp->data = ptr;
	memcpy(&(resp->data[resp->size]), contents, realsize);
	resp->size += realsize;
	resp->data[resp->size] = 0;

	return realsize;
}

static json_t *curl(const char *endpoint, const char *post_body,
			const char *headers)
{
	CURL *curl;
	CURLcode res;
	json_t *data;
	json_error_t error;

	struct response resp;

	resp.data = malloc(1);
	resp.size = 0;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &resp);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);
	if (headers)
		curl_easy_setopt(curl, CURLOPT_HEADER, headers);

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	
	res = curl_easy_perform(curl);

	if (res != CURLE_OK){
		fprintf(stderr, "curl_easy_perform() failed: %s\n", 
				curl_easy_strerror(res));
	} else {
		data = json_loads(resp.data, 0, &error);

		if (!data) {
			fprintf(stderr, "json_loads() failed: %s\n", error.text);

			return NULL;
		}
	}

	curl_easy_cleanup(curl);
	free(resp.data);

	return (data) ? data : NULL;
}

static char *auth_bearer_request(struct ret_data *data, const char *client_id,
				const char *tenant, const char *code,
				json_t *json_data)
{
	const char *auth_bearer;

	sds endpoint = sdsnew(HOST);
	endpoint = sdscat(endpoint, "common/oauth2/token");

	sds post_body = sdsnew("resource=" RESOURCE_ID);
	post_body = sdscat(post_body, "&code=");
	post_body = sdscat(post_body, code);
	post_body = sdscat(post_body, "&client_id=");
	post_body = sdscat(post_body, client_id);
	post_body = sdscat(post_body, "&grant_type=device_code");
	
	for (;;) {
		nanosleep((const struct timespec[]){{TTW, 0}}, NULL);
		json_data = curl(endpoint, post_body, NULL);

		if (json_object_get(json_data, "access_token")) {
			auth_bearer = json_string_value(
					json_object_get(json_data, "access_token"));
		} else {
			auth_bearer = json_string_value(
					json_object_get(json_data, "error"));
		}

		if (strcmp(auth_bearer, AUTH_ERROR) != 0)
			break;

	}

	sdsfree(endpoint);
	sdsfree(post_body);

	data->auth_bearer = auth_bearer;
}

static char *oauth_request(struct ret_data *data, const char *client_id,
			const char *tenant, json_t *json_data)
{
	const char *d_code, *u_code;

	sds endpoint = sdsnew(HOST);
	endpoint = sdscat(endpoint, tenant);
	endpoint = sdscat(endpoint, "/oauth2/devicecode/");

	sds post_body = sdsnew("resource=" RESOURCE_ID);
	post_body = sdscat(post_body, "&client_id=");
	post_body = sdscat(post_body, client_id);
	post_body = sdscat(post_body, "&scope=profile");

	json_data = curl(endpoint, post_body, NULL);

	if (json_object_get(json_data, "device_code") && json_object_get(json_data, "user_code")) {
		d_code = json_string_value(json_object_get(json_data, "device_code"));
		u_code = json_string_value(json_object_get(json_data, "user_code"));
	} else { 
		fprintf(stderr, "json_object_get() failed: device_code & user_code NULL\n");
		
		exit(1);
	}

	data->d_code = d_code;
	data->u_code = u_code;

	sdsfree(endpoint);
	sdsfree(post_body);
}

static int *verify_user(jwt_t *jwt, const char *user,
			const char *domain)
{
	const char *upn = jwt_get_grant(jwt, "upn");

	sds username = sdsnew(user);
	username = sdscat(username, domain);

	if (strcmp(upn, username) == 0 ) {
		return 0;
	} else { 
		return (int *) 1;
	}
}

static int converse(pam_handle_t *pamh, int nargs,
		const struct pam_message **message, 
		struct pam_response **response)
{
	struct pam_conv *conv;
	int retval = pam_get_item(pam, PAM_CONV, (void *) &conv);
	if (retval != PAM_SUCCESS) 
		return retval;
	return conv->conv(nargs, message, response, conv->appdata_ptr);
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, 
					int argc, const char **argv)
{
	jwt_t *jwt;

	const char *username, *client_id, *tenant, 
	      		*domain, *u_code, *d_code, *ab_token;

	struct ret_data data;
	json_t *json_data, *config;
	json_error_t error;

	config = json_load_file(CONFIG_FILE, 0, &error);
	if (!config) {
		fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);

		return PAM_AUTH_ERR;
	}
	
	if (json_object_get(json_object_get(config, "client"), "id")) {
		client_id = json_string_value(
				json_object_get(json_object_get(config, "client"), "id"));
	} else {
		printf("Error with ID in JSON\n");
		
		return PAM_AUTH_ERR;
	}

	if (json_object_get(config, "domain")) {
		domain = json_string_value(
				json_object_get(config, "domain"));
	} else {
		printf("Error with Domain in JSON\n");

		return PAM_AUTH_ERR;
	}

	if (json_object_get(config, "tenant")) {
		tenant = json_string_value(
				json_object_get(config, "tenant"));
	} else {
		printf("Error with tenant in JSON\n");

		return PAM_AUTH_ERR;
	}

	if (pam_get_user(pamh, &username, NULL) != PAM_SUCCESS) {
		printf("pam_get_user(): failed to get a username");

		return PAM_AUTH_ERR;
	}

	oauth_request(&data, client_id, tenant, json_data);

	u_code = data.u_code;
	d_code = data.d_code;

	sds prompt = sdsnew(CODE_PROMPT);
	prompt = sdscat(prompt, u_code);
	prompt = sdscat(prompt, "\nPress enter to begin polling...\n");

	const struct pam_message msg = {
		.msg_style = PAM_PROMPT_ECHO_OFF,
		.msg = prompt
	};
	const struct pam_message *msgs = &msg;
	struct pam_response *resp = NULL;

	converse(pamh, 1, &msgs, &resp);

	//printf(CODE_PROMPT "%s\n", u_code);
	//printf("Polling until code is entered...\n");

	auth_bearer_request(&data, client_id, tenant, d_code, json_data);
	ab_token = data.auth_bearer;
	jwt_decode(&jwt, ab_token, NULL, 0);

	if (verify_user(jwt, username, domain) == 0) {
		printf("Username supplied matches UPN! Success!\n");

		json_decref(json_data);
		json_decref(config);
		jwt_free(jwt);
		return PAM_SUCCESS;
	} else {
		printf("Imposter detected! Failure!\n");

		json_decref(json_data);
		json_decref(config);
		jwt_free(jwt);
		return PAM_AUTH_ERR;
	}


	return PAM_AUTH_ERR;
}


PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags,
				int argc, const char **argv)
{
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags,
				int argc, const char **argv)
{
	return PAM_SUCCESS;
}
