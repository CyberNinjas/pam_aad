int request_azure_signin_code(char *code, const char *resource_id, const char *client_id, const char *tenant, char *device_code);

int request_azure_oauth_token(char *code, const char *resource_id, const char *client_id, const char *token_buf);

int request_azure_group_membership(const char *token, const char *required_group_id, const char *tenant);
