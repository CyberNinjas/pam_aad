#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <time.h>
#include <stdlib.h>

#include "cJSON.h"

#define HOST "login.microsoftonline.com"
#define PORT "443"

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

int find_json_bounds(char *json_buf, int &start, int &end){
    int i;
    int j;
    for(i = 0; json_buf[i] != '{'; i++){
    }
    printf("first assignment\n");
    start = i;
    printf("after first assignment\n");
    for(j = i; json_buf[j] != '}'; j++){
    }
    end = j;
    printf("\nthe start is %d\n", start);
    printf("\nthe end is %d\n", end);
    return 0;
}

int read_code_from_microsoft(const char *resource_id, const char *client_id, const char *tenant, char *response_buf){
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
    strcpy(post_buf, "text");
    strcat(post_buf, resource_id);
    strcat(post_buf, "&client_id=");
    strcat(post_buf, client_id);
    strcat(post_buf, "&client_request_id=");
    /* TODO: Change the below for a legitimate client request id, randomly generated */
    strcat(post_buf, "5929459294929");

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
            printf("\n\n");
            break;
        }
        buf[size] = 0;
        strcat(response_buf, buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}

/* purely for testing, takes no command line args */
int main(){
    /* initialize variables */
    const char *resource_id;
    const char *client_id;
    const char *tenant;
    char response_buf[2048];
    char code_buf[100];

    int start;
    int end;

    char client_id_buf[15];
    /* Provide hardcoded values for testing */
    resource_id = "00000002-0000-0000-c000-000000000000";
    client_id = "7262ee1e-6f52-4855-867c-727fc64b26d5";
    tenant = "digipirates.onmicrosoft.com";

    read_code_from_microsoft(resource_id, client_id, tenant, response_buf);
    find_json_bounds(response_buf, start, end);
    printf("\nthe start is %d\n", start);
    printf("\nthe end is %d\n", end);
    
    return 0;
}
