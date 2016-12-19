#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

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

using namespace std;
using namespace jwtcpp;


string getClientRequestId()
{
  string client_request_id = "12345";
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
    cout << "couldnt get user from payload" << endl;
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
  bool gotToken = false;
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
  string token = pullTokenFromResponse(response);
  string username = pullUsernameFromIdToken(response);
  return PAM_SUCCESS; 
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv){
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv){
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv){
  const char *user = NULL;
  int pgu_ret;
  pgu_ret = pam_get_user(pamh, &user, "Enter 0365 credentials...");
  CSimpleIniA ini; 
  ini.SetUnicode();
  ini.LoadFile("etc/security/oauth.config.ini");
  string tenant = ini.GetValue("oauth", "tenant");
  string resource = ini.GetValue("oauth", "resource_id");
  string client_id = ini.GetValue("oauth", "client_id");
  pgu_ret = AuthenticateToMicrosoft(tenant, resource, client_id); 
  if (pgu_ret != PAM_SUCCESS){
    cout << "Yer failin" << std::endl;
    return(PAM_AUTH_ERR);
} 
  return(PAM_SUCCESS);
}


int main(int args, char* argv[])
{
  CSimpleIniA ini;
  ini.SetUnicode();
  ini.LoadFile("/etc/security/oauth.config.ini");
  string tenant = ini.GetValue("oauth", "tenant");
  string resource_id = ini.GetValue("oauth", "resource_id");
  string client_id = ini.GetValue("oauth", "client_id");
  int response;
  response = AuthenticateToMicrosoft(tenant, resource_id, client_id);
  if (response == PAM_SUCCESS){
    cout << "Congratulations, you're logged in!" << std::endl;
    return PAM_SUCCESS;
  }else{
    return PAM_AUTH_ERR;
  } 
}
