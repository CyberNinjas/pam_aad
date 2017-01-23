char *base64encode (const void *b64_encode_this, int encode_this_many_bytes);

char *base64decode (const void *b64_decode_this, int decode_this_many_bytes);

char *str_replace ( const char *string, const char *substr, const char *replacement );

int split (const char *str, char c, char ***arr);

char* load_file(char const* path);