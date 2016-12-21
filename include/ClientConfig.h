#include "SimpleIni.h"
#include <iostream>
#include <string>
using namespace std;

class ClientConfig
{
 
 public:
  bool loadData(const char*);

  string getTenant();

  string getClient_id();

  string getResource_id();

};
