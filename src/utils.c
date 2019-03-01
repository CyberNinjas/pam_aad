#include <cjson/cJSON.h>
#include <openssl/pem.h>
#include <jwt.h>
#include <stdio.h>
#include <string.h>

char *base64encode(const void *b64_encode_this, int encode_this_many_bytes)
{
    BIO *b64_bio, *mem_bio;	//Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    BUF_MEM *mem_bio_mem_ptr;	//Pointer to a "memory BIO" structure holding our base64 data.
    b64_bio = BIO_new(BIO_f_base64());	//Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());	//Initialize our memory sink BIO.
    BIO_push(b64_bio, mem_bio);	//Link the BIOs by creating a filter-sink BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);	//No newlines every 64 characters or less.
    BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes);	//Records base64 encoded data.
    BIO_flush(b64_bio);		//Flush data.  Necessary for b64 encoding, because of pad characters.
    BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);	//Store address of mem_bio's memory structure.
    BIO_set_close(mem_bio, BIO_NOCLOSE);	//Permit access to mem_ptr after BIOs are destroyed.
    BIO_free_all(b64_bio);	//Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);	//Makes space for end null.
    (*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = '\0';	//Adds null-terminator to tail.
    return (*mem_bio_mem_ptr).data;	//Returns base-64 encoded data. (See: "buf_mem_st" struct).
}

char *base64decode(const void *b64_decode_this, int decode_this_many_bytes)
{
    BIO *b64_bio, *mem_bio;	//Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    char *base64_decoded = calloc((decode_this_many_bytes * 3) / 4 + 1, sizeof(char));	//+1 = null.
    b64_bio = BIO_new(BIO_f_base64());	//Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());	//Initialize our memory source BIO.
    BIO_write(mem_bio, b64_decode_this, decode_this_many_bytes);	//Base64 data saved in source.
    BIO_push(b64_bio, mem_bio);	//Link the BIOs by creating a filter-source BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);	//Don't require trailing newlines.
    int decoded_byte_index = 0;	//Index where the next base64_decoded byte should be written.
    while (0 < BIO_read(b64_bio, base64_decoded + decoded_byte_index, 1)) {	//Read byte-by-byte.
	decoded_byte_index++;	//Increment the index until read of BIO decoded data is complete.
    }				//Once we're done reading decoded data, BIO_read returns -1 even though there's no error.
    BIO_free_all(b64_bio);	//Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    return base64_decoded;	//Returns base-64 decoded data with trailing null terminator.
}

char *str_replace(const char *string, const char *substr,
		  const char *replacement)
{
    char *tok = NULL;
    char *newstr = NULL;
    char *oldstr = NULL;
    char *head = NULL;

    /* if either substr or replacement is NULL, duplicate string a let caller handle it */
    if (substr == NULL || replacement == NULL)
	return strdup(string);
    newstr = strdup(string);
    head = newstr;
    while ((tok = strstr(head, substr))) {
	oldstr = newstr;
	newstr =
	    malloc(strlen(oldstr) - strlen(substr) + strlen(replacement) +
		   1);
	/*failed to alloc mem, free old string and return NULL */
	if (newstr == NULL) {
	    free(oldstr);
	    return NULL;
	}
	memcpy(newstr, oldstr, tok - oldstr);
	memcpy(newstr + (tok - oldstr), replacement, strlen(replacement));
	memcpy(newstr + (tok - oldstr) + strlen(replacement),
	       tok + strlen(substr),
	       strlen(oldstr) - strlen(substr) - (tok - oldstr));
	memset(newstr + strlen(oldstr) - strlen(substr) +
	       strlen(replacement), 0, 1);
	/* move back head right after the last replacement */
	head = newstr + (tok - oldstr) + strlen(replacement);
	free(oldstr);
    }
    return newstr;
}

int split(char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0') {
	if (*p == c)
	    count++;
	p++;
    }

    *arr = (char **) malloc(sizeof(char *) * count);
    if (*arr == NULL)
	exit(1);

    p = str;
    while (*p != '\0') {
	if (*p == c) {
	    (*arr)[i] = (char *) malloc(sizeof(char) * token_len);
	    if ((*arr)[i] == NULL)
		exit(1);

	    token_len = 0;
	    i++;
	}
	p++;
	token_len++;
    }
    (*arr)[i] = (char *) malloc(sizeof(char) * token_len);
    if ((*arr)[i] == NULL)
	exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0') {
	if (*p != c && *p != '\0') {
	    *t = *p;
	    t++;
	} else {
	    *t = '\0';
	    i++;
	    t = ((*arr)[i]);
	}
	p++;
    }

    return count;
}

char *load_file(char const *path)
{
    char *buffer;
    long length;
    FILE *f = fopen(path, "rb");
    if (f) {
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (char *) malloc((length + 1) * sizeof(char));
	if (buffer) {
	    fread(buffer, sizeof(char), length, f);
	}
	fclose(f);
    }
    buffer[length] = '\0';
    return buffer;
}

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

int jwt_username_matches(char *raw_token, const char *claimed_username)
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
