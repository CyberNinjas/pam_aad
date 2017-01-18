
int request_azure_signin_code(char *code, const char *resource_id, const char *client_id, const char *tenant, char *device_code);

int request_azure_oauth_token(char *code, const char *resource_id, const char *client_id, char *token_buf);