struct jwt {
    const char *token;
    cJSON *payload;
    cJSON *header;
};

struct jwt parse_token(const char *raw_token);

int verify_token(struct jwt base_jwt, const char *raw_token, const unsigned char *key);