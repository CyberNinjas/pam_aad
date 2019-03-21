#include <curl/curl.h>
#include <jansson.h>
#include <jwt.h>
#include <sds/sds.h>
#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>

#define AUTH_ERROR "authorization_pending"
#define CODE_PROMPT "Please use this one-time passcode (OTP) to sign in to your account: "
#define CONFIG_FILE "/etc/pam_aad.conf"
#define HOST "https://login.microsoftonline.com/"
#define RESOURCE_ID "00000002-0000-0000-c000-000000000000"
#define SUBJECT "Your one-time passcode for signing in via Azure Active Directory"
#define TTW 5			/* time to wait in seconds */
#define USER_AGENT "azure_authenticator_pam/1.0"
#define USER_PROMPT "An email with a one-time passcode was sent to your email." \
	            "\nEnter the code at https://aka.ms/devicelogin, then press enter.\n"

struct message {
    size_t lines_read;
    char *data[];
};

struct response {
    char *data;
    size_t size;
};

struct ret_data {
    const char *u_code, *d_code, *auth_bearer;
};

static size_t read_callback(void *ptr, size_t size, size_t nmemb,
			    void *userp)
{
    struct message *msg = (struct message *) userp;
    char *data;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
	return 0;
    }
    data = msg->data[msg->lines_read];

    if (data) {
	size_t len = strlen(data);
	memcpy(ptr, data, len);
	msg->lines_read++;
	return len;
    }

    return 0;
}

static size_t response_callback(void *contents, size_t size, size_t nmemb,
				void *userp)
{
    size_t realsize = size * nmemb;
    struct response *resp = (struct response *) userp;
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

static char *get_date(void)
{
    char *date;
    int len = 56;
    struct tm *tm_info;
    time_t timer;

    date = (char *) malloc(len * sizeof(char));
    time(&timer);
    tm_info = localtime(&timer);
    /* RFC 822 POSIX time stamp */
    strftime(date, len, "Date: %a, %d %b %Y %H:%M:%S %z\r\n", tm_info);
    return date;
}

static char *get_message_id(void)
{
    char domainname[64], hostname[64], msg_id[37], *message_id;
    int len = 192;
    size_t i;
    uuid_t uuid;

    message_id = (char *) malloc(len * sizeof(char));
    uuid_generate(uuid);

    for (i = 0; i < sizeof(uuid); i++) {
	if (i == 0) {
	    snprintf(msg_id, sizeof(msg_id) - strlen(msg_id), "%02x",
		     uuid[i]);
	} else if (i == 3 || i == 5 || i == 7 || i == 9) {
	    snprintf(msg_id + strlen(msg_id),
		     sizeof(msg_id) - strlen(msg_id) + 1, "%02x-",
		     uuid[i]);
	} else {
	    snprintf(msg_id + strlen(msg_id),
		     sizeof(msg_id) - strlen(msg_id), "%02x", uuid[i]);
	}
    }

    getdomainname(domainname, sizeof(domainname));
    gethostname(hostname, sizeof(hostname));
    snprintf(message_id, len, "Message-ID: <%s@%s.%s>\r\n", msg_id,
	     hostname, domainname);
    return message_id;
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

    /* curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); */

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
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

static void auth_bearer_request(struct ret_data *data,
				const char *client_id, const char *tenant,
				const char *code, json_t * json_data)
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
	nanosleep((const struct timespec[]) { {
		  TTW, 0}}, NULL);
	json_data = curl(endpoint, post_body, NULL);

	if (json_object_get(json_data, "access_token")) {
	    auth_bearer =
		json_string_value(json_object_get
				  (json_data, "access_token"));
	} else {
	    auth_bearer =
		json_string_value(json_object_get(json_data, "error"));
	}

	if (strcmp(auth_bearer, AUTH_ERROR) != 0)
	    break;

    }

    sdsfree(endpoint);
    sdsfree(post_body);

    data->auth_bearer = auth_bearer;
}

static void oauth_request(struct ret_data *data, const char *client_id,
			  const char *tenant, json_t * json_data)
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

    if (json_object_get(json_data, "device_code")
	&& json_object_get(json_data, "user_code")) {
	d_code =
	    json_string_value(json_object_get(json_data, "device_code"));
	u_code =
	    json_string_value(json_object_get(json_data, "user_code"));
    } else {
	fprintf(stderr,
		"json_object_get() failed: device_code & user_code NULL\n");

	exit(1);
    }

    data->d_code = d_code;
    data->u_code = u_code;

    sdsfree(endpoint);
    sdsfree(post_body);
}

static int verify_user(jwt_t * jwt, const char *username)
{
    const char *upn = jwt_get_grant(jwt, "upn");
    return (strcmp(upn, username) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int notify_user(pam_handle_t * pamh, const char *to_addr,
		       const char *from_addr, const char *message,
		       const char *smtp_server)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    int msg_len = 9;
    struct curl_slist *recipients = NULL;
    struct message *msg;

    sds to_str = sdsnew("To: ");
    to_str = sdscat(to_str, to_addr);
    to_str = sdscat(to_str, "\r\n");

    sds from_str = sdsnew("From: ");
    from_str = sdscat(from_str, from_addr);
    from_str = sdscat(from_str, "\r\n");

    sds msg_str = sdsempty();
    msg_str = sdscat(msg_str, message);
    msg_str = sdscat(msg_str, "\r\n");

    sds smtp_url = sdsempty();
    smtp_url = sdscat(smtp_url, "smtp://");
    smtp_url = sdscat(smtp_url, smtp_server);

    msg = malloc(sizeof(struct message) + (msg_len * sizeof(char *)));
    msg->lines_read = 0;

    msg->data[0] = get_date();
    msg->data[1] = to_str;
    msg->data[2] = from_str;
    msg->data[3] = get_message_id();
    msg->data[4] = "Subject: " SUBJECT "\r\n";
    msg->data[5] = "\r\n";
    msg->data[6] = msg_str;
    msg->data[7] = "\r\n";
    msg->data[8] = NULL;

    curl = curl_easy_init();
    if (curl) {
	curl_easy_setopt(curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL);
	curl_easy_setopt(curl, CURLOPT_URL, smtp_url);
	curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from_addr);
	recipients = curl_slist_append(recipients, to_addr);
	curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
	curl_easy_setopt(curl, CURLOPT_READDATA, msg);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	/* curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); */

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
	    fprintf(stderr, "curl_easy_perform() failed: %s\n",
		    curl_easy_strerror(res));

	curl_slist_free_all(recipients);
	curl_easy_cleanup(curl);

	(void) pam_prompt(pamh, PAM_PROMPT_ECHO_OFF, NULL, USER_PROMPT);
    }

    free(msg->data[0]);
    free(msg->data[3]);
    return (int) res;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t * pamh, int flags,
				   int argc, const char **argv)
{
    jwt_t *jwt;

    const char *user, *client_id, *tenant,
	*domain, *u_code, *d_code, *ab_token, *tenant_addr, *smtp_server;

    struct ret_data data;
    json_t *json_data, *config;
    json_error_t error;

    config = json_load_file(CONFIG_FILE, 0, &error);
    if (!config) {
	fprintf(stderr, "error in config on line %d: %s\n", error.line,
		error.text);
	return PAM_AUTH_ERR;
    }

    if (json_object_get(json_object_get(config, "client"), "id")) {
	client_id =
	    json_string_value(json_object_get
			      (json_object_get(config, "client"), "id"));
    } else {
	fprintf(stderr, "error with ID in JSON\n");
	return PAM_AUTH_ERR;
    }

    if (json_object_get(config, "domain")) {
	domain = json_string_value(json_object_get(config, "domain"));
    } else {
	fprintf(stderr, "error with Domain in JSON\n");
	return PAM_AUTH_ERR;
    }

    if (json_object_get(config, "tenant")) {
	tenant =
	    json_string_value(json_object_get
			      (json_object_get(config, "tenant"), "name"));
	if (json_object_get(json_object_get(config, "tenant"), "address")) {
	    tenant_addr =
		json_string_value(json_object_get
				  (json_object_get(config, "tenant"),
				   "address"));
	} else {
	    fprintf(stderr, "error with tenant address in JSON\n");
	    return PAM_AUTH_ERR;
	}
    } else {
	fprintf(stderr, "error with tenant in JSON\n");
	return PAM_AUTH_ERR;
    }

    if (json_object_get(config, "smtp_server")) {
	smtp_server =
	    json_string_value(json_object_get(config, "smtp_server"));
    } else {
	fprintf(stderr, "error with Domain in JSON\n");
	return PAM_AUTH_ERR;
    }

    if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS) {
	fprintf(stderr, "pam_get_user(): failed to get a username\n");
	return PAM_AUTH_ERR;
    }

    sds user_addr = sdsnew(user);
    user_addr = sdscat(user_addr, "@");
    user_addr = sdscat(user_addr, domain);

    curl_global_init(CURL_GLOBAL_ALL);

    oauth_request(&data, client_id, tenant, json_data);

    u_code = data.u_code;
    d_code = data.d_code;

    sds prompt = sdsnew(CODE_PROMPT);
    prompt = sdscat(prompt, u_code);
    notify_user(pamh, user_addr, tenant_addr, prompt, smtp_server);

    auth_bearer_request(&data, client_id, tenant, d_code, json_data);

    curl_global_cleanup();

    ab_token = data.auth_bearer;
    jwt_decode(&jwt, ab_token, NULL, 0);

    if (verify_user(jwt, user_addr) == 0) {
	json_decref(json_data);
	json_decref(config);
	jwt_free(jwt);
	return PAM_SUCCESS;
    } else {
	json_decref(json_data);
	json_decref(config);
	jwt_free(jwt);
	return PAM_AUTH_ERR;
    }

    return PAM_AUTH_ERR;
}


PAM_EXTERN int pam_sm_setcred(pam_handle_t * pamh, int flags,
			      int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t * pamh, int flags,
				int argc, const char **argv)
{
    return PAM_SUCCESS;
}
