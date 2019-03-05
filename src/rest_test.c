#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <sds/sds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLIENT_ID ""
#define HOST "https://login.microsoftonline.com/"
#define RESOURCE_ID "00000002-0000-0000-c000-000000000000"
#define TENANT ""
#define USER_AGENT "azure_authenticator_pam/1.0"

struct response 
{
	char *data;
	size_t size;
};

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

static cJSON *read_code_from_microsoft(const char *client_id, const char *tenant)
{
	CURL *curl_handle;
	CURLcode res;
	cJSON *data, *device_code;
	struct response resp;

	resp.data = malloc(1);
	resp.size = 0;

	sds endpoint = sdsnew(HOST);
	endpoint = sdscat(endpoint, tenant);
	endpoint = sdscat(endpoint, "/oauth2/devicecode/");

	sds post_buf = sdsnew("resource=" RESOURCE_ID);
	post_buf = sdscat(post_buf, "&client_id=");
	post_buf = sdscat(post_buf, client_id);
	post_buf = sdscat(post_buf, "&scope=profile");
	
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_buf);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, response_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &resp);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
	
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	res = curl_easy_perform(curl_handle);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	} else {
		data = cJSON_Parse(resp.data);

		if (data == NULL) {
			fprintf(stderr, "cJSON_Parse() failed\n");
		}
	}

	curl_easy_cleanup(curl_handle);
	sdsfree(endpoint);
	sdsfree(post_buf);
	free(resp.data);

	device_code = cJSON_GetObjectItem(data, "user_code");
	return (device_code) ? device_code : NULL;
}

static cJSON *get_oauth2_token(char *code, const char *client_id)
{
	CURL *curl_handle;
	CURLcode res;
	cJSON *token_data, *token;
	struct response resp;

	resp.data = malloc(1);
	resp.size = 0;

	sds endpoint = sdsnew(HOST);
	endpoint = sdscat(endpoint, "common/oauth2/token");

	sds post_body = sdsnew("resource=" RESOURCE_ID);
	post_body = sdscat(post_body, "&code=");
	post_body = sdscat(post_body, code);
	post_body = sdscat(post_body, "&client_id=");
	post_body = sdscat(post_body, client_id);
	post_body = sdscat(post_body, "&grant_type=device_code");

	printf("\n\n %s \n\n", post_body);

	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_body);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, response_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &resp);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);	
	
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

	res = curl_easy_perform(curl_handle);

	printf("\n\n %s \n\n", res);

	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	} else {
		token_data = cJSON_Parse(resp.data);

		if (token_data == NULL) {
			fprintf(stderr, "cJSON_Parse() failed: token_data NULL\n");
			
			return NULL;
		}
	}

	curl_easy_cleanup(curl_handle);
	sdsfree(endpoint);
	sdsfree(post_body);
	free(resp.data);

	token = cJSON_GetObjectItem(token_data, "access_code");

	printf("\n %s \n", token->valuestring);

	return (token) ? token : NULL;
}



int main()
{
	cJSON *device_code, *auth_code;
	sds client_id = sdsnew(CLIENT_ID);
	sds tenant = sdsnew(TENANT);
	
	int ch;

	device_code = read_code_from_microsoft(client_id, tenant);
	sds code = sdsnew(device_code->valuestring);
	
	printf("%s\n", code);
	
	printf("Press enter to continue...");
	ch = getchar();
	
	auth_code = get_oauth2_token(code, client_id);
	sds auth_token = sdsnew(auth_code->valuestring);
	
	printf("%s\n", auth_token);
	

	sdsfree(client_id);
	sdsfree(tenant);
	sdsfree(code);
	return 0;
}


