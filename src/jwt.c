#include <stdio.h>

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
    return 0;
}
