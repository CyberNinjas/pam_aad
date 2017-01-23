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

    printf("This is the header: %s\n", base64_decoded1);

    temp.header  = cJSON_Parse(base64_decoded1);
    temp.payload = cJSON_Parse(base64_decoded2);
    temp.token = raw_token;

    return temp;
}

int verify_token(struct jwt base_jwt, const char *raw_token, const unsigned char *key){
    int ret;
    int key_len;
    jwt_t **jwt;
    key_len = strlen(key);
    ret = jwt_decode(jwt, raw_token, key, key_len);
    return ret;
}

int main(){
    const char *raw_token;
    raw_token = "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiIsIng1dCI6Ilk0dWVLMm9hSU5RaVFiNVlFQlNZVnlEY3BBVSIsImtpZCI6Ilk0dWVLMm9hSU5RaVFiNVlFQlNZVnlEY3BBVSJ9.eyJhdWQiOiIwMDAwMDAwMi0wMDAwLTAwMDAtYzAwMC0wMDAwMDAwMDAwMDAiLCJpc3MiOiJodHRwczovL3N0cy53aW5kb3dzLm5ldC80ZGU2YThiZC03MGNiLTRhZGYtODkzNC02YzZjZmI4YTU4OTUvIiwiaWF0IjoxNDg0NzU5MzE2LCJuYmYiOjE0ODQ3NTkzMTYsImV4cCI6MTQ4NDc2MzIxNiwiYWNyIjoiMSIsImFtciI6WyJwd2QiXSwiYXBwaWQiOiI3MjYyZWUxZS02ZjUyLTQ4NTUtODY3Yy03MjdmYzY0YjI2ZDUiLCJhcHBpZGFjciI6IjAiLCJmYW1pbHlfbmFtZSI6IkNhbGR3ZWxsIiwiZ2l2ZW5fbmFtZSI6IlNoYW5lIiwiaXBhZGRyIjoiOTYuODUuMjQ0LjI1IiwibmFtZSI6IlNoYW5lIENhbGR3ZWxsIiwib2lkIjoiMzZkOTBjNzUtYTUxZC00YzFlLThkZDgtY2QyYThkZWE2MjBiIiwicGxhdGYiOiIzIiwicHVpZCI6IjEwMDMwMDAwOUI0OEQzOTUiLCJzY3AiOiJVc2VyLlJlYWQiLCJzdWIiOiJESk11MTgtSXo2cGFYeEk2eWJFVFVwTXJKMFhQcmxVRXhzbWE0SC03RW5jIiwidGlkIjoiNGRlNmE4YmQtNzBjYi00YWRmLTg5MzQtNmM2Y2ZiOGE1ODk1IiwidW5pcXVlX25hbWUiOiJjYXB0YWluQGRpZ2lwaXJhdGVzLm9ubWljcm9zb2Z0LmNvbSIsInVwbiI6ImNhcHRhaW5AZGlnaXBpcmF0ZXMub25taWNyb3NvZnQuY29tIiwidmVyIjoiMS4wIn0.oG4uPhD45gnqhx4H6IOpG7zyXgq2_frTgP0Y5LsaiJKRym5MXPkCBHRtRAe8BYegC5ejMOxSorvtgGZZH8TssO8yhKLv7mHvyK-5ynerKMfUdEiJsIwT3Q_3Ad_CsfLZOgUZq8l5NZPiU2beaFn8MLE2oWXi15NdygBFZEkD-7ZUJNBRv83OyyABkqSdVKoQ_2Yu0Uf9CGHJuOSL51lCvAeHzZy7TTBjx6gU1kyBOoNLMLdqDN4KC1uppWQqWD7R_F75jZY0IfV6mwJbtAVL8Ane4euBh3KqnuYPnWSzGMHjEwpVooxJe0aj5TmGj424EF5LyLMTGA86K6diN4Tj7Al";

    const char *url_token;
    
    url_token = str_replace(raw_token, "-", "+");
    url_token = str_replace(url_token, "_", "/");
    
    int i;
    int num_tokens = 0;
    char **arr = NULL;
    num_tokens = split(url_token, '.', &arr);
    if (num_tokens != 3){
        printf("jwt was malformed, erroring out.\n");
        return 1;
    }

    for(i = 0; i < num_tokens; i++){
    }

    int bytes_to_decode_1 = strlen(arr[0]);
    int bytes_to_decode_2 = strlen(arr[1]);

    char *base64_decoded1 = base64decode(arr[0], bytes_to_decode_1);
    char *base64_decoded2 = base64decode(arr[1], bytes_to_decode_2);

    jwt_t **jwt;
    const unsigned char *key = load_file("key.pem");
    int key_len = strlen(key);
    int ret = jwt_decode(jwt, raw_token, key, key_len);
    printf("ret value is %d\n", ret);
    struct jwt oldjwt = parse_token(raw_token);
    int wow = verify_token(oldjwt, oldjwt.token, key);
    printf("the ret value is %d\n", wow);
    return 0;
}