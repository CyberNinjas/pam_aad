#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#define HOST "login.microsoftonline.com"
#define PORT "443"

int read_code_from_microsoft(const char *resource_id, const char *client_id, const char *tenant){
    /* initialize variables */

    BIO* bio;
    /* SSL* ssl; */
    SSL_CTX* ctx;
    
    char post_buf[1024];

    /* Variables used to read the response from the server */
    int size;
    char buf[1024];

    char write_buf[1024];


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
    printf("Directly before strcpy\n");
    strcpy(post_buf, "text");
    printf("strcpy ran fine\n");
    strcat(post_buf, resource_id);
    printf("first strcat\n");
    //strcat(post_buf, "&client_id=");
    //strcat(post_buf, client_id);
    /* TODO: Change the below for a legitimate client request id, randomly generated */
    //strcat(post_buf, "5929459294929");

    printf("post buf initialized"\n);
    /* Data to create a HTTP request */
    strcpy(write_buf, "POST /");
    // strcat(write_buf, tenant);
    // strcat(write_buf, "/oauth2/devicecode/ HTTP/1.1\r\n");
    // strcat(write_buf, "Host: " HOST "\r\n");
    // strcat(write_buf, "Connection: close \r\n");
    // strcat(write_buf, "User-Agent: azure_authenticator_pam/1.0 \r\n");
    // strcat(write_buf, "Content-Length: ");
    // strcat(write_buf, (char*)sizeof(post_buf));
    // strcat(write_buf, "\r\n");
    // strcat(write_buf, "\r\n");
    // strcat(write_buf, post_buf);
    // strcat(write_buf, "\r\n");

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
        printf("%s", buf);
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

    /* Provide hardcoded values for testing */
    resource_id = "00000002-0000-0000-c000-000000000000";
    client_id = "7262ee1e-6f52-4855-867c-727fc64b26d5";
    tenant = "virtualvikings.onmicrosoft.com";

    read_code_from_microsoft(resource_id, client_id, tenant);
    return 0;
}
