#include <cjson/cJSON.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sds/sds.h>
#include <curl/curl.h>

#define HOST "https://login.microsoftonline.com/"
#define PORT "443"
#define RESOURCE_ID "00000002-0000-0000-c000-000000000000"
#define USER_AGENT "azure_authenticator_pam/1.0"

struct response {
	char *data;
	size_t size;
};

static size_t response_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct response *resp = (struct response *) userp;

	char *ptr = realloc(resp->data, resp->size + realsize + 1);
	if (ptr == NULL) {
		// Out of memory
		printf("Not enough memory (realloc returned NULL\n");
		return 0;
	}

	resp->data = *ptr;
	memcpy(&(resp->data[resp->size]), contents, realsize);
	resp->size += realsize;
	resp->data[resp->size] = 0; 

	return realsize;
}



/*
 * Function: fill_json_buffer
 * --------------------------
 * *json_buf: char array that will hold the json message contained in raw_response
 * 
 * *raw_response: char array that holds the original raw http response from microsoft. 
 * 
 * *start holds the index of the '{' 
 *
 * *end holds the index of '}'
*/

int fill_json_buffer(char *json_buf, char *raw_response, int *start,
		     int *end)
{
    memcpy(json_buf, &raw_response[*start], *end - *start + 1);
    json_buf[*end + 1] = '\0';
    return EXIT_SUCCESS;
}

/*
 *
 * Function: find_json_bounds
 *---------------------------
 * json_buf: contains a full http response that includes json.
 * 
 * start:    pointer that will contain the index of the start of the json body. 
 *
 * end:      pointer that will contain the index of the end of the json body.
 */

int find_json_bounds(char *json_buf, int *start, int *end)
{
    int i, j;
    for (i = 0; json_buf[i] != '{'; i++) {
    }
    *start = i;
    for (j = i; json_buf[j] != '\0'; j++) {
    }
    *end = j;
    return EXIT_SUCCESS;
}

/*
 * Function: poll_microsoft_for_token
 * ----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *client_id: char array containing the client id 
 *
 * *response_buf: empty buffer to include the response in.
 */

static cJSON *poll_microsoft_for_token(char *code, const char *client_id,
			     char *response_buf)
{

    CURL *curl_handle;
    CURLcode res;
    cJSON *token_data, *token;
	struct response resp;

	resp.data = malloc(1);
	resp.size = 0;

	sds post_buf = sdsnew("resource=" RESOURCE_ID);
	post_buf = sdscat(post_buf, "&code=");
	post_buf = sdscat(post_buf, code);
	post_buf = sdscat(post_buf, "&client_id=");
	post_buf = sdscat(post_buf, client_id);
	post_buf = sdscat(post_buf, "&grant_type=device_code");

	sds endpoint = sdsnew(HOST);
	endpoint = sdscat(endpoint, "common/oauth2/token");
	
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_body);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, response_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &resp);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);

	// https://curl.haxx.se/libcurl/c/CURLOPT_VERBOSE.html
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	
	res = curl_easy_perform(curl_handle);

	if (res != NULL) {
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

	token = cJSON_GetObjectItem(token_data, "access_token");

	return (token) ? token : NULL;

}

int read_code_from_microsoft(const char *client_id,
			     const char *tenant, char *response_buf)
{
    /* initialize variables */
    BIO *bio;
    /* SSL* ssl; */
    SSL_CTX *ctx;

    char post_buf[2048];

    /* Variables used to read the response from the server */
    int size;
    char buf[2048];

    char write_buf[2048];

    strcpy(response_buf, " ");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL) {
	printf("Ctx is null\n");
    }

    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if (BIO_do_connect(bio) <= 0) {
	printf("Failed connection\n");
	return EXIT_FAILURE;
    } else {
	printf("Connected\n");
    }
    strcpy(post_buf, "resource=00000002-0000-0000-c000-000000000000");
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&scope=profile");

    /* Data to create a HTTP request */
    strcpy(write_buf, "POST /");
    strcat(write_buf, tenant);
    strcat(write_buf, "/oauth2/devicecode/ HTTP/1.1\r\n");
    strcat(write_buf, "Host: " HOST "\r\n");
    strcat(write_buf, "Connection: close \r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    strcat(write_buf, "Content-Length: 100\r\n");
    strcat(write_buf, "\r\n");
    strcat(write_buf, post_buf);
    strcat(write_buf, "\r\n");

    /* Attempts to write len bytes from buf to BIO */
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0) {
	/* handle failed write here */
	if (!BIO_should_retry(bio)) {
	    printf("Do retry\n");
	}

	printf("Failed write\n");
    }

    /* Read the response */
    for (;;) {
	size = BIO_read(bio, buf, 1023);

	/* If no more data, than exit the loop */
	if (size <= 0) {
	    break;
	}
	buf[size] = 0;
	strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return EXIT_SUCCESS;
}


int get_microsoft_graph_groups(char *user_object_id, char *response_buf,
			       char *token, const char *tenant,
			       const char *group_object_id)
{
    /* initialize variables */
    char secondary_buf[204800];

    BIO *bio;
    /* SSL* ssl; */
    SSL_CTX *ctx;

    /* Variables used to read the response from the server */
    int size;
    char buf[2048];

    strcpy(response_buf, "");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL) {
	printf("Ctx is null\n");
    }

    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, "graph.windows.net:443");

    if (BIO_do_connect(bio) <= 0) {
	printf("Failed connection\n");
	return EXIT_FAILURE;
    } else {
	printf("Connected\n");
    }

    /* Data to create a HTTP request */
    strcat(secondary_buf, "GET /");
    strcat(secondary_buf, tenant);
    strcat(secondary_buf, "/isMemberOf?api-version=1.6 HTTP/1.1\r\n");
    strcat(secondary_buf, "Authorization: Bearer ");
    strcat(secondary_buf, token);
    strcat(secondary_buf, "\r\n");
    strcat(secondary_buf, "Host: graph.windows.net\r\n");
    strcat(secondary_buf, "User-Agent: azure_authenticator_pam/1.0\r\n");
    strcat(secondary_buf, "Connection: close\r\n");
    strcat(secondary_buf, "Content-Type: application/json\r\n");
    strcat(secondary_buf, "Content-Length: 114\r\n");
    strcat(secondary_buf, "\r\n");
    strcat(secondary_buf, "{\r\n\r\n");
    strcat(secondary_buf, "\"groupId\":");
    strcat(secondary_buf, "\"");
    strcat(secondary_buf, group_object_id);
    strcat(secondary_buf, "\",\n");
    strcat(secondary_buf, "\"memberId\":");
    strcat(secondary_buf, "\"");
    strcat(secondary_buf, user_object_id);
    strcat(secondary_buf, "\"\n");
    strcat(secondary_buf, "\r\n}\r\n\r\n\r\n");

    /* Attempts to write len bytes from buf to BIO */
    if (BIO_write(bio, secondary_buf, strlen(secondary_buf)) <= 0) {
	/* handle failed write here */
	if (!BIO_should_retry(bio)) {
	    printf("Do retry\n");
	}

	printf("Failed write\n");
    }

    /* Read the response */
    for (;;) {
	size = BIO_read(bio, buf, 1023);

	/* If no more data, than exit the loop */
	if (size <= 0) {
	    break;
	}
	buf[size] = 0;
	strcat(response_buf, buf);
    }
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return EXIT_SUCCESS;
}

int get_microsoft_graph_userprofile(char *token, char *response_buf,
				    const char *tenant)
{
    /* initialize variables */
    BIO *bio;
    /* SSL* ssl; */
    SSL_CTX *ctx;

    /* Variables used to read the response from the server */
    int size;
    char buf[2048];

    char write_buf[204800];

    strcpy(response_buf, "");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL) {
	printf("Ctx is null\n");
    }

    /* Creates a new BIO chain consisting of an SSL BIO */
    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, "graph.windows.net:443");

    if (BIO_do_connect(bio) <= 0) {
	printf("Failed connection\n");
	return EXIT_FAILURE;
    } else {
	printf("Connected\n");
    }

    /* Data to create a HTTP request */
    strcat(write_buf, "GET /");
    strcat(write_buf, tenant);
    strcat(write_buf, "/me?api-version=1.6 HTTP/1.1\r\n");
    strcat(write_buf, "Authorization: Bearer ");
    strcat(write_buf, token);
    strcat(write_buf, "\r\n");
    strcat(write_buf, "Host: graph.windows.net\r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0\r\n");
    strcat(write_buf, "Connection: close\r\n");
    strcat(write_buf, "\r\n");

    /* Attempts to write len bytes from buf to BIO */
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0) {
	/* handle failed write here */
	if (!BIO_should_retry(bio)) {
	    printf("Do retry\n");
	}

	printf("Failed write\n");
    }

    /* Read the response */
    for (;;) {
	size = BIO_read(bio, buf, 1023);

	/* If no more data, than exit the loop */
	if (size <= 0) {
	    break;
	}
	buf[size] = 0;
	strcat(response_buf, buf);
    }
    strcpy(response_buf, "\0");
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return EXIT_SUCCESS;
}
