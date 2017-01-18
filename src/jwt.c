#include <stdio.h>
#include <openssl/pem.h>
#include <string.h> //Only needed for strlen().

char *base64encode (const void *b64_encode_this, int encode_this_many_bytes){
    BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    BUF_MEM *mem_bio_mem_ptr;    //Pointer to a "memory BIO" structure holding our base64 data.
    b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());                           //Initialize our memory sink BIO.
    BIO_push(b64_bio, mem_bio);            //Link the BIOs by creating a filter-sink BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);  //No newlines every 64 characters or less.
    BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes); //Records base64 encoded data.
    BIO_flush(b64_bio);   //Flush data.  Necessary for b64 encoding, because of pad characters.
    BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);  //Store address of mem_bio's memory structure.
    BIO_set_close(mem_bio, BIO_NOCLOSE);   //Permit access to mem_ptr after BIOs are destroyed.
    BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);   //Makes space for end null.
    (*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = '\0';  //Adds null-terminator to tail.
    return (*mem_bio_mem_ptr).data; //Returns base-64 encoded data. (See: "buf_mem_st" struct).
}

char *base64decode (const void *b64_decode_this, int decode_this_many_bytes){
    BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
    char *base64_decoded = calloc( (decode_this_many_bytes*3)/4+1, sizeof(char) ); //+1 = null.
    b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
    mem_bio = BIO_new(BIO_s_mem());                         //Initialize our memory source BIO.
    BIO_write(mem_bio, b64_decode_this, decode_this_many_bytes); //Base64 data saved in source.
    BIO_push(b64_bio, mem_bio);          //Link the BIOs by creating a filter-source BIO chain.
    BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);          //Don't require trailing newlines.
    int decoded_byte_index = 0;   //Index where the next base64_decoded byte should be written.
    while ( 0 < BIO_read(b64_bio, base64_decoded+decoded_byte_index, 1) ){ //Read byte-by-byte.
        decoded_byte_index++; //Increment the index until read of BIO decoded data is complete.
    } //Once we're done reading decoded data, BIO_read returns -1 even though there's no error.
    BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
    return base64_decoded;        //Returns base-64 decoded data with trailing null terminator.
}

char *
str_replace ( const char *string, const char *substr, const char *replacement ){
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;
  char *head = NULL;
 
  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL || replacement == NULL ) return strdup (string);
  newstr = strdup (string);
  head = newstr;
  while ( (tok = strstr ( head, substr ))){
    oldstr = newstr;
    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      free (oldstr);
      return NULL;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    /* move back head right after the last replacement */
    head = newstr + (tok - oldstr) + strlen( replacement );
    free (oldstr);
  }
  return newstr;
}

int split (const char *str, char c, char ***arr)
{
    int count = 1;
    int token_len = 1;
    int i = 0;
    char *p;
    char *t;

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
            count++;
        p++;
    }

    *arr = (char**) malloc(sizeof(char*) * count);
    if (*arr == NULL)
        exit(1);

    p = str;
    while (*p != '\0')
    {
        if (*p == c)
        {
            (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
            if ((*arr)[i] == NULL)
                exit(1);

            token_len = 0;
            i++;
        }
        p++;
        token_len++;
    }
    (*arr)[i] = (char*) malloc( sizeof(char) * token_len );
    if ((*arr)[i] == NULL)
        exit(1);

    i = 0;
    p = str;
    t = ((*arr)[i]);
    while (*p != '\0')
    {
        if (*p != c && *p != '\0')
        {
            *t = *p;
            t++;
        }
        else
        {
            *t = '\0';
            i++;
            t = ((*arr)[i]);
        }
        p++;
    }

    return count;
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
    }
    printf("Found %d tokens.\n", num_tokens);

    for(i = 0; i < num_tokens; i++){
        printf("string #%d: %s\n", i, arr[i]);
    }

    int bytes_to_decode = strlen(arr[0]);
    
    char *base64_decoded = base64decode(arr[0], bytes_to_decode);

    printf("base64 decoded string is %s\n", base64_decoded);
    return 0;
}
