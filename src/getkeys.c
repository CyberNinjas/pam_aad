#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <time.h>
#include <stdlib.h>

#include "cJSON.h"

#define HOST "login.microsoftonline.com"
#define PORT "443"

int get_microsoft_public_keys(char *response_buf){
     /* initialize variables */
    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char get_buf[1024];

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

    strcpy(write_buf, "GET /");
    strcat(write_buf, "common/discovery/keys HTTP/1.1\r\n");
    strcat(write_buf, "Host: " HOST "\r\n");
    strcat(write_buf, "Connection: close \r\n");
    strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    strcat(write_buf, "\r\n");
    
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
    for(j = i; json_buf[j] != '\0'; j++){
        if (json_buf[j] == '}') {*end = j;}
    }
    return 0;
}

int get_jwk(char *jwt_kid, cJSON *jwk){
    char response_buf[10000];
    char json_buf[10000];
    int start, end;
    cJSON *root;
    cJSON *key;
    char *kid_candidate;

    /* Go get the public keys and turn them into a json object.*/
    get_microsoft_public_keys(response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    root = cJSON_Parse(json_buf);
    cJSON *keys = cJSON_GetObjectItem(root, "keys");
    cJSON_ArrayForEach(root, keys){
        kid_candidate = cJSON_GetObjectItem(root, "kid")->valuestring;
        if(!(strcmp(kid_candidate, jwt_kid))){
            /* our desired object in the array is found, break the loop */
            break;
         }
    }
    if (strcmp(kid_candidate, jwt_kid)){
        /* they aren't equal and we got through the whole loop. None of these keys will validate */ 
        return 1;
    }
    jwk = root;
    return 0;
}

int main(){
    char response_buf[10000];
    char json_buf[10000];
    int start, end;
    cJSON *root;
    cJSON *key;
    char *jwt_kid;
    jwt_kid = "Y4ueK2oaINQiQb5YEBSYVyDcpAU";
    char *kid_candidate;

    /* Go get the public keys and turn them into a json object.*/
    get_microsoft_public_keys(response_buf);
    printf("the value of response_buf is %s\n", response_buf);
    find_json_bounds(response_buf, &start, &end);
    fill_json_buffer(json_buf, response_buf, &start, &end);
    root = cJSON_Parse(json_buf);
    cJSON *keys = cJSON_GetObjectItem(root, "keys");
    cJSON_ArrayForEach(root, keys){
        kid_candidate = cJSON_GetObjectItem(root, "kid")->valuestring;
        if(!(strcmp(kid_candidate, jwt_kid))){
            /* our desired object in the array is found, break the loop */
            break;
         }
    }
    key = root;
    printf("the correct kid is %s\n", cJSON_GetObjectItem(key, "kid")->valuestring);
    return 0;
}
