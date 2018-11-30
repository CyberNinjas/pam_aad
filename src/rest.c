#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <time.h>
#include <stdlib.h>

#include "cJSON.h"
#include "utils.h"

#define HOST "login.microsoftonline.com"
#define PORT "443"

/*
 * Function: poll_microsoft_for_token
 * ----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *resource_id: char array containing the resource id
 *
 * *client_id: char array containing the client id 
 *
 * *response_buf: empty buffer to include the response in.
 *
 * returns 0 if completion is successful, 1 if it fails.
 */

int poll_microsoft_for_token(char *code, char *resource_id, char *client_id, char *response_buf){
     /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[1024];

    /* Variables used to read the response from the server */
    int size;
    char buf[1024];

    char write_buf[2048];

    strcpy(response_buf, " ");
    /* Registers the available SSL/TLS ciphers */
    /* Starts security layer */

    SSL_library_init();

    /* creates a new SSL_CTX object as framework to establish TLS/SSL enabled connections */

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
        printf("Connected\n");
    }

    strcpy(post_buf, "resource=");
    strcat(post_buf, resource_id);
    strcat(post_buf, "&code=");
    strcat(post_buf, code);
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&grant_type=device_code");

    /* Data to create a HTTP request */
    strcpy(write_buf, "POST /");
    strcat(write_buf, "common/oauth2/token/ HTTP/1.1\r\n");
    strcat(write_buf, "Host: " HOST "\r\n");
    strcat(write_buf, "Connection: close \r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    strcat(write_buf, "Content-Length: 307\r\n");
    strcat(write_buf, "\r\n");
    strcat(write_buf, post_buf);
    strcat(write_buf, "\r\n");

    /* Attempts to write len bytes from buf to BIO */ 
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}

/*
 * Function: request_azure_oauth_token
 * -----------------------------------
 * *code: char array containing the device code used in the user's prompt.
 *
 * *resource_id: char array containing the resource id
 *
 * *client_id: char array containing the client id 
 *
 * returns a 0 if the function completes successfully, and 0 otherwise.
 */
int request_azure_oauth_token(char *code, char *resource_id, char *client_id, char *token_buf){
    int start, end;
    char response_buf[9000];
    char json_buf[9000];
    cJSON *json; 
    char *access_token;
    poll_microsoft_for_token(code, resource_id, client_id, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    cJSON *access = cJSON_GetObjectItem(json, "access_token");
    if (access == NULL){
        /* Something failed. */
        strcpy(token_buf, "FAILURE");
        return 1;
   }
   strcpy(token_buf, cJSON_GetObjectItem(json, "access_token")->valuestring);
    return 0;
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
 *
*/

int fill_json_buffer(char *json_buf, char *raw_response, int *start, int *end){
    memcpy(json_buf, &raw_response[*start], *end - *start + 1);
    json_buf[*end + 1] = '\0'; 
    return 0;
}

/*
 * Function: get_client_id
 *------------------------
 * client_id: an empty buffer that will be filled with the client ID.
 *
 * returns a 0, which indicates the function completed correctly. This function is simple
 * so it is assumed there is no alternative return. 
 *
*/

int get_client_id(char *client_id){
    srand(time(NULL));
    int random = rand();
    sprintf(client_id, "%ld", random);
    return 0;
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
 *
 * returns an integer that indicates that the function successfully completed. 
 */ 

int find_json_bounds(char *json_buf, int *start, int *end){
    int i, j;
    for(i = 0; json_buf[i] != '{'; i++){}
    *start = i;
    for(j = i; json_buf[j] != '\0'; j++){}
    *end = j;
    return 0;
}

int read_code_from_microsoft(char *resource_id, char *client_id, char *tenant, char *response_buf){
    /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
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

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
        printf("Connected\n");
    }
    strcpy(post_buf, "resource=");
    strcat(post_buf, resource_id);
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&client_request_id=");
    strcat(post_buf, "5929459294929");
    strcat(post_buf,"&scope=profile");

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
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}


int get_microsoft_graph_groups(char *user_object_id, char *response_buf, char *token, char *tenant, char *group_object_id){
    /* initialize variables */
    char secondary_buf[204800];

    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[2048];

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

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */

    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, "graph.windows.net:443");

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
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
    if (BIO_write(bio, secondary_buf, strlen(secondary_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return 0;
}

int get_microsoft_graph_userprofile(char *token, char *response_buf, char *tenant){
    /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[2048];

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

    if (ctx == NULL)
    {
        printf("Ctx is null\n");
    }
    
    /* Creates a new BIO chain consisting of an SSL BIO */
    bio = BIO_new_ssl_connect(ctx);

    /* uses the string name to set the hostname */

    BIO_set_conn_hostname(bio, "graph.windows.net:443");

    if(BIO_do_connect(bio) <= 0)
    {
        printf("Failed connection\n");
        return 1;
    }
    else{
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
    if (BIO_write(bio, write_buf, strlen(write_buf)) <= 0)
    {
        /* handle failed write here */ 
        if (!BIO_should_retry(bio))
        {
            printf("Do retry\n");
        }

        printf("Failed write\n");
    }

    /* Read the response */
    for (;;)
    {
        size = BIO_read(bio, buf, 1023);

        /* If no more data, than exit the loop */
        if(size <= 0)
        {
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }
    strcpy(response_buf, "\0");
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    return 0;
}

int parse_user_groups(char *response_buf, cJSON* group_membership_value){
    char garbage[1000];
    char json_buf[4000];
    char doublegarbage[10000];
    int start, end;
    cJSON *json;
    strcat(response_buf, "\0");
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    if (json == NULL){
        return 1;
    }
    int checkval = cJSON_GetObjectItem(json, "value")->type;
    return checkval;
}


int parse_user_object_id(char *response_buf, char* user_object_id_buf){
    char json_buf[4000];    
    int start, end;
    cJSON *json;

    find_json_bounds(response_buf, &start, &end);
    
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    cJSON *object = cJSON_GetObjectItem(json, "objectId");
    if (object == NULL){
        /* Something failed. */
        printf("Something failed.\n");
        return 1;
   }
    strcpy(user_object_id_buf, cJSON_GetObjectItem(json, "objectId")->valuestring);
    
    return 0;
}

/*
 * Function: request_azure_signin_code
 *-----------------------------------
 * *code: character buffer that will have the code inside of it by the function's end.
 *
 * *resource_id: char array containing MS resource id
 *
 * *clientid: contains client id of application as registered with Azure.
 *
 * *tenant: the MS tenant. 
 *
 * returns EXIT_FAILURE if the code buffer is empty at the end of the function
 * and EXIT_SUCCESS if the code buffer is anything but empty. 
 * 
 * TODO: Improve checking if this function succeeded. Should be some more error 
 * handling and there will need to be some way to log failures. 
*/

int request_azure_signin_code(char *user_code, char *resource_id, char *client_id, char *tenant, char *device_code){
    char response_buf[2048];
    char code_buf[100];
    char json_buf[2048];
    cJSON *json;
    int start, end;

    read_code_from_microsoft(resource_id, client_id, tenant, response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    json = cJSON_Parse(json_buf);
    strcpy(user_code, cJSON_GetObjectItem(json, "user_code")->valuestring);
    strcpy(device_code, cJSON_GetObjectItem(json, "device_code")->valuestring);
    if (user_code[0] == '\0' || device_code[0] == '\0'){
        /* string is empty, we have failed somewhere */
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int request_azure_group_membership(char *token, char *required_group_id, char *tenant){
    int success = 2;
    char user_profile_buf[1000];
    char user_object_id_buf[100];
    char raw_group_buf[10000];
    cJSON *group_membership_value; 

     get_microsoft_graph_userprofile(token, user_profile_buf, tenant);
     parse_user_object_id(user_profile_buf, user_object_id_buf);
     get_microsoft_graph_groups(user_object_id_buf, raw_group_buf, token, tenant, required_group_id);
     int is_in_group = parse_user_groups(raw_group_buf, group_membership_value);
     if(is_in_group == success){
         return 0;
     }
     return 1;
}
