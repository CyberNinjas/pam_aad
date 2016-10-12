#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
#include <string>
#include <iostream>

using namespace std;

int authenticateToMicrosoft(string username, string password)
{
  string client_id = "someid";
  string redirect_uri = "theuri";
  string tenant = "digipirates.onmicrosoft.com";
  string url = "https://login.microsoftonlinecom/" + tenant + "/oauth/v2.0/authorize?client_id=" + client_id + "&response_type=code&redirect_uri=" + redirect_uri;
  RestClient::Response r = RestClient::get(url);
  if (r.code == 200)
  {
    cout << "Successful first request!\n";
    return 0;
  }
  else
  {
    cout << "Error of some kind, non 200 response.\n";
    return 0;
  }
}


int main(int args, char* argv[])
{
  int response;
  string username; 
  string password;
  username = "filler";
  password = "filler";
  response = authenticateToMicrosoft(username, password); 
  return response;
}