#include "jwt.h"
#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
#include <security/pam_appl.h> 
#include <security/pam_modules.h>
#include "json.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include "SimpleIni.h"
#include "ClientConfig.h"
#include <unistd.h>
#include <stdio.h>

using namespace std;
using namespace jwtcpp;


string getClientRequestId()
{
  srand(time(NULL));
  std::string client_request_id = std::to_string(rand() % 10000000 + 1);
  return client_request_id;
}

string  getDeviceCode(string tenant, string resource, string client_id)
{
  string client_request_id = getClientRequestId();
  string url = "https://login.microsoftonline.com/" + tenant + "/oauth2/devicecode?resource=" + resource + "&client_id=" + client_id + "&client-request-id" + client_request_id;
  RestClient::Response r = RestClient::get(url);
  if (r.code == 200)
  { 
    return r.body;
  }
  else
  {
    cout << "Error of some kind, non 200 response.\n";
    cout <<"\nerror is: " + r.body + "\n";
    cout <<"\nAnd the code is:";
    cout << r.code;
    return r.body;
  }
}

nlohmann::json getUriMessage(string rawMessage, string resource, string client_id){
  //spit out json object
  auto parsed = nlohmann::json::parse(rawMessage);
  nlohmann::json message;
  message["poll_uri"] = "https://login.microsoftonline.com/common/oauth2/token";
  message["resource"] = resource;
  message["client_id"] = client_id;
  message["grant_type"] = "device_code";
  message["code"] = parsed["device_code"];
  return message;
}

string pollForToken(nlohmann::json request_dictionary){
  string uri = request_dictionary["poll_uri"];
  string resource = request_dictionary["resource"];
  string client_id = request_dictionary["client_id"];
  string code = request_dictionary["code"];
  string body = "resource=" + resource + "&client_id=" + client_id + "&grant_type=device_code&code=" + code;
  RestClient::Response r = RestClient::post(uri, "application/x-www-form-urlencoded", body);
  return r.body;
}

bool providedToken(string response_body){
  auto parsed = nlohmann::json::parse(response_body);
  if (parsed["error"] == NULL){
  return true;
} 
  return false;
}

string pullTokenFromResponse(string response){
  auto parsed = nlohmann::json::parse(response);
  string token = parsed["access_token"];
  return token;
}

string decodeJWT(string id_token){
  JWT* jwt = parse(id_token);
  string user = jwt->fromPayload("upn");
  if (user.empty()){
    cout << "couldnt parse user from payload" << endl;
  }
  cout << user << endl;
  return user; 
}

string pullUsernameFromIdToken(string response){
  auto parsed = nlohmann::json::parse(response);
  string email = decodeJWT(parsed["id_token"]);
  return email;
}

int AuthenticateToMicrosoft(string tenant, string resource, string client_id){
  bool gotToken = true;
  string response;
  string deviceCodeMessage = getDeviceCode(tenant, resource, client_id);
  nlohmann::json request_dictionary = getUriMessage(deviceCodeMessage, resource, client_id);
  auto microsoft = nlohmann::json::parse(deviceCodeMessage);
  string message = microsoft["message"];
  cout << message << std::endl;
  while (!gotToken){
    sleep(5);
    response = pollForToken(request_dictionary);
    gotToken = providedToken(response);
  } 
  return PAM_SUCCESS;
}
//   string token = pullTokenFromResponse(response);
//   string username = pullUsernameFromIdToken(response);
//   return PAM_SUCCESS; 
// }

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv){
  printf("set cred\n");
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv){
  printf("account mgmt");
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv){
  const char *user = NULL;
  int pgu_ret;
  pgu_ret = pam_get_user(pamh, &user, "Enter 0365 username...");
  printf("Welcome %s\n!", user);
  CSimpleIniA ini; 
  ini.SetUnicode();
  ini.LoadFile("etc/security/oauth.config.ini");
  string tenant = ini.GetValue("oauth", "tenant");
  string resource = ini.GetValue("oauth", "resource_id");
  string client_id = ini.GetValue("oauth", "client_id");
  pam_tty_conv(1, );
  pgu_ret = AuthenticateToMicrosoft(tenant, resource, client_id); 
  if (pgu_ret != PAM_SUCCESS){
    cout << "Yer failin" << std::endl;
    return(PAM_AUTH_ERR);
} 
  return(PAM_SUCCESS);
}

int converse(pam_handle_t *pamh, int nargs,
                    PAM_CONST struct pam_message **message,
                    struct pam_response **response) {
  struct pam_conv *conv;
  int retval = pam_get_item(pamh, PAM_CONV, (void *)&conv);
  if (retval != PAM_SUCCESS) {
    return retval;
  }
  return conv->conv(nargs, message, response, conv->appdata_ptr);
}

char *request_signin(pam_handle_t *pamh, int echocode, PAM_CONST char *prompt){
  PAM_CONST struct pam_message msg = {.msg_style = echocode, 
                                           .msg = prompt};

  PAM_CONST struct pam_message *msgs = &msg;

  struct pam_response *resp = NULL;

  int retval = converse(pamh, 1, &msgs, &resp);

  char *ret = NULL;
  
  ret = resp->resp;

  return (ret);
}
