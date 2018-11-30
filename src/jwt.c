#include <stdio.h>
#include <openssl/pem.h>
#include <string.h> //Only needed for strlen().
#include <jwt.h>

#include "cJSON.h"
#include "utils.h"

#define HEADER 0
#define PAYLOAD 1  

struct jwt {
    const char *token;
    cJSON *payload;
    cJSON *header;
    const char *user;
};

struct jwt parse_token(const char *raw_token){
    struct jwt temp;
    const char *url_token;
    
    url_token = str_replace(raw_token, "-", "+");
    url_token = str_replace(url_token, "_", "/");
    
    int i;
    int num_tokens = 0;
    char **arr = NULL;
    num_tokens = split(url_token, '.', &arr);

    int num_header_bytes =  strlen(arr[HEADER]);
    int num_payload_bytes = strlen(arr[PAYLOAD]);

    char *base64_decoded1 = base64decode(arr[HEADER], num_header_bytes);
    char *base64_decoded2 = base64decode(arr[PAYLOAD], num_payload_bytes);
    strcat(base64_decoded2, "\"}");
    temp.header  = cJSON_Parse(base64_decoded1);
    temp.payload = cJSON_Parse(base64_decoded2);
    temp.token = raw_token;
    cJSON *user = cJSON_GetObjectItem(temp.payload, "upn");
    if (user == NULL){
        printf("no upn?\n");
        return temp;
    }
    temp.user  = cJSON_GetObjectItem(temp.payload, "upn")->valuestring;
    printf("parsed \n");
    return temp;
}

int jwt_username_matches(const char* raw_token, char* claimed_username){
    char user[200];
    int claimed_length = strlen(claimed_username);
    struct jwt token = parse_token(raw_token);
    strncpy(user, token.user, claimed_length);
    user[claimed_length] = '\0';
    return strcmp(claimed_username, user);
}

int azure_token_validate(char *raw_token){
    int ret;
    struct jwt token = parse_token(raw_token);
    ret = verify_token(token, token.token, "");
    return ret;
}

int verify_token(struct jwt base_jwt, const char *raw_token, const unsigned char *key){
    int ret;
    int key_len;
    jwt_t **jwt;
    key_len = strlen(key);
    ret = jwt_decode(jwt, raw_token, key, key_len);
    return ret;
}
