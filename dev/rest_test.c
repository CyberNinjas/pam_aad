// too lazy to add usage /rest <client-id> <tenant> <username> <domain>

#include <curl/curl.h>
#include <jansson.h>
#include <jwt.h>
#include <sds/sds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AUTH_ERROR "authorization_pending"
#define CONFIG_FILE "/etc/pam-test.conf"
#define HOST "https://login.microsoftonline.com/"
#define RESOURCE_ID "00000002-0000-0000-c000-000000000000"
#define USER_AGENT "azure_authenticator_pam/1.0"

struct response 
{
	char *data;
	size_t size;
};

//struct && debugginig 24-121
struct ret_data
{
	const char *u_code, *d_code, *auth_bearer;

};

static void dump(const char *text,
				          FILE *stream, unsigned char *ptr, size_t size)
{
  size_t i;
  size_t c;
  unsigned int width=0x10;
			   
  fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
								          text, (long)size, (long)size);
		 
  for(i=0; i<size; i+= width) {
      fprintf(stream, "%4.4lx: ", (long)i);
						   
      /* show hex to the left */
      for(c = 0; c < width; c++) {
	        if(i+c < size)
		        fprintf(stream, "%02x ", ptr[i+c]);
  	        else
				fputs("   ", stream);
	  }
								   
	  /* show data on the right */
	  for(c = 0; (c < width) && (i+c < size); c++) {
	        char x = (ptr[i+c] >= 0x20 && ptr[i+c] < 0x80) ? ptr[i+c] : '.';
	        fputc(x, stream);
	  }
									   
	  fputc('\n', stream); /* newline */
  }
}
 
static
int debug_callback(CURL *handle, curl_infotype type,
				             char *data, size_t size,
							              void *userp)
{
  const char *text;
  (void)handle; /* prevent compiler warning */
  (void)userp;
			   
  switch (type) {
  	case CURLINFO_TEXT:
		fprintf(stderr, "== Info: %s", data);
	default: /* in case a new one is introduced to shock us */
	   	return 0;
										   
	case CURLINFO_HEADER_OUT:
	   	text = "=> Send header";
	    break;
	case CURLINFO_DATA_OUT:
	    text = "=> Send data";
	    break;
	case CURLINFO_SSL_DATA_OUT:
	    text = "=> Send SSL data";
	    break;
	case CURLINFO_HEADER_IN:
	    text = "<= Recv header";
	    break;
	case CURLINFO_DATA_IN:
	    text = "<= Recv data";
	    break;
	case CURLINFO_SSL_DATA_IN:
	    text = "<= Recv SSL data";
	    break;
  }
				 
  dump(text, stderr, (unsigned char *)data, size);
  return 0;
}

static size_t response_callback(void *contents, size_t size, size_t nmemb,
								void *userp)
{
	size_t realsize = size * nmemb;
	struct response *resp = (struct response *)userp;

	char *ptr = realloc(resp->data, resp->size + realsize + 1);
	if (ptr == NULL) {
		// Out of Memory
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
	CURL *curl_handle;
	CURLcode res;
	json_t *data;
	json_error_t error;

	struct response resp;

	resp.data = malloc(1);
	resp.size = 0;

	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_body);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, response_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &resp);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);	
	if (headers != NULL)
			curl_easy_setopt(curl_handle, CURLOPT_HEADER, headers);
		
	curl_easy_setopt(curl_handle, CURLOPT_DEBUGFUNCTION, debug_callback);
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	
	res = curl_easy_perform(curl_handle);
	
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

	curl_easy_cleanup(curl_handle);
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
	
	for(;;){
		json_data = curl(endpoint, post_body, NULL);

		if (json_object_get(json_data, "access_token") != NULL){
			auth_bearer = json_string_value(json_object_get(json_data, 
									"access_token"));
		} else {
			auth_bearer = json_string_value(json_object_get(json_data, 
									"error"));
		}
		if (strcmp(auth_bearer, AUTH_ERROR) != 0)
				break;
	}

	sdsfree(endpoint);
	sdsfree(post_body);

	data->auth_bearer = auth_bearer;
}

void *oauth_request(struct ret_data *data, const char *client_id, 
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

	if (json_object_get(json_data, "device_code") != NULL &&
					json_object_get(json_data, "user_code") != NULL) {
		d_code = json_string_value(json_object_get(json_data, "device_code"));
		u_code = json_string_value(json_object_get(json_data, "user_code"));
	} else {
		fprintf(stderr, 
				"json_object_get() failed: device_code & user_code NULL\n");

		return NULL;
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

	if (strcmp(upn, username) == 0){
			return 0;
	} else { 
			return (int *) 1;
	}
}


int main(int argc, char *argv[])
{

	jwt_t *jwt;
	const char *user = argv[1],
		   	*client, *ten, *domain,
		   	*u_code, *d_code, *ab_token;
	struct ret_data data;
	json_t *json_data, *config; 
	json_error_t error;

	config = json_load_file(CONFIG_FILE, 0, &error);
	if (!config) {
		fprintf(stderr, "error: on line %d: %s\n", 
						error.line, error.text);
		return 1;
	}

	if (json_object_get(
		json_object_get(config, "client"), "id") != NULL) {
		client = json_string_value(
						json_object_get(
								json_object_get(config, "client"), "id"));
	} else {
		printf("Error with Id in JSON.\n");
		return 0;
	}

	if (json_object_get(config, "domain") != NULL) {
		domain = json_string_value(json_object_get(config, "domain"));
	} else {
		printf("Error with Domain in JSON.\n");
	   return 0;	
	}

	if (json_object_get(config, "tenant") != NULL) {
		ten = json_string_value(json_object_get(config, "tenant"));
	} else {
		printf("Error with Tenant in JSON.\n");
		return 0;
	}

	printf("Pam Auth Test\n");

	oauth_request(&data, client, ten, json_data);

	u_code = data.u_code;	
	d_code = data.d_code;

	printf("%s\n", u_code);
	printf("Polling until code is entered...\n");
	
	auth_bearer_request(&data, client, ten, d_code, json_data);
	ab_token = data.auth_bearer;
	jwt_decode(&jwt, ab_token, NULL, 0);

	if (verify_user(jwt, user, domain) == 0) {
			printf("Username supplied matches UPN! Sucess!\n");
	} else {
			printf("Imposter detected! Failure!\n");
	}
	

	json_decref(json_data);
	jwt_free(jwt);
	return 0;
}


