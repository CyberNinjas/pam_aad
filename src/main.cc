#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
#include "json.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include "SimpleIni.h"
#include "ClientConfig.h"

using namespace std;

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
  cout << "response is...";
  cout << response;
}

int AuthenticateToMicrosoft(string tenant, string resource, string client_id){
  bool gotToken = false;
  string response;
  string deviceCodeMessage = getDeviceCode(tenant, resource, client_id);
  nlohmann::json request_dictionary = getUriMessage(deviceCodeMessage, resource, client_id);
  cout << "\n";
  cout << "The following message was given from microsoft...";
  cout << deviceCodeMessage;
  cout << request_dictionary["message"];
  while (!gotToken){
    sleep(5); //wait for 5 seconds before polling
    response = pollForToken(request_dictionary);
    gotToken = providedToken(response);
  } 
  string token = pullTokenFromResponse(response);
  return 1; 
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
  string username; 
  string password;
  response = AuthenticateToMicrosoft(tenant, resource_id, client_id);
  cout <<"\nReturned from AuthenticateToMicrosoft call...";
  return response;
}
