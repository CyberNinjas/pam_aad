#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"
int main(int args, char* argv[])
{
  RestClient::Response r = RestClient::get("https://github.com");
  return 0;
}

