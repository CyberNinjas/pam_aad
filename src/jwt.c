#include <cjson/cJSON.h>
#include <openssl/pem.h>
#include <jwt.h>
#include <stdio.h>
#include <string.h>

char *base64decode(const void *b64_decode_this,
		   int decode_this_many_bytes);
int split(char *str, char c, char ***arr);
char *str_replace(const char *string, const char *substr,
		  const char *replacement);

struct jwt {
    const char *token;
    const char *user;
    cJSON *payload;
    cJSON *header;
};

struct jwt parse_token(char *raw_token)
{
    struct jwt temp;
    char *url_token;

    url_token = str_replace(raw_token, "-", "+");
    url_token = str_replace(url_token, "_", "/");

    int i;
    int num_tokens = 0;
    char **arr = NULL;
    num_tokens = split(url_token, '.', &arr);

    int num_header_bytes = strlen(arr[0]);
    int num_payload_bytes = strlen(arr[1]);

    char *base64_decoded1 = base64decode(arr[0], num_header_bytes);
    char *base64_decoded2 = base64decode(arr[1], num_payload_bytes);
    strcat(base64_decoded2, "\"}");
    temp.header = cJSON_Parse(base64_decoded1);
    temp.payload = cJSON_Parse(base64_decoded2);
    temp.token = raw_token;
    cJSON *user = cJSON_GetObjectItem(temp.payload, "upn");
    if (user == NULL) {
	printf("no upn?\n");
	return temp;
    }
    temp.user = cJSON_GetObjectItem(temp.payload, "upn")->valuestring;
    printf("parsed \n");
    return temp;
}

int jwt_username_matches(char *raw_token,
			 const char *claimed_username)
{
    char user[200];
    int claimed_length = strlen(claimed_username);
    struct jwt token = parse_token(raw_token);
    strncpy(user, token.user, claimed_length);
    user[claimed_length] = '\0';
    return strcmp(claimed_username, user);
}

int verify_token(struct jwt base_jwt, const char *raw_token,
		 const unsigned char *key)
{
    int key_len;
    jwt_t *jwt;
    key_len = (key == NULL) ? 0 : strlen(key);
    return jwt_decode(&jwt, raw_token, key, key_len);
}

int azure_token_validate(char *raw_token)
{
    struct jwt token = parse_token(raw_token);
    return verify_token(token, token.token, NULL);
}
