#include "SimpleIni.h"
#include <iostream>
#include <string>
using namespace std;

class ClientConfig{
	string tenant;
	string resource_id;
	string client_id;

public:
	//returns true on success
	bool loadData (const char* configFileName){
        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(configFileName);
        tenant = ini.GetValue("oauth", "tenant");
        resource_id = ini.GetValue("oauth", "resource_id");
        client_id = ini.GetValue("oauth", "client_id");
        return true;
	}

	string getTenant(){
		return tenant;
	}

	string getResource_id(){
		return resource_id;
	}

	string getClient_id(){
		return client_id;
	}

};