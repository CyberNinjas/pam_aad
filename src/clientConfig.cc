/* clientConfig.cc This code defines an object that, when instantiated, reads a configuration file for all client specific details of the Oauth flow.  If it cannot find the config file, 
 *the object will throw a warning and rely on default values which are hardcoded in.
 * @example
 * int main(){
 *  String *tenant;
 *  String *client_id;
 *  String *resource_id;
 *  String cfgfilepath = "/etc/security/config.ini";
 *  readConfigFile(cfgfilepath, tenant, client_id, resource_id); 
 *  }
 *  Find an example configuration file in examples/example.config.ini
 */
#include<string>
#include<iostream>
#include<glib.h>
typedef struct {
        std::string *client_id, *resource_id, *tenant;
} Settings;


static void ClientConfig::readConfigFile(std::string cfgfile, std::string*& client_id, std::string*& resource_id, std::string*& tenant) {

        Settings *conf;
        GKeyFile *keyfile;
        GKeyFileFlags flags;
        gsize length;

        keyfile = g_key_file_new ();
        //flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

        cout << "Looking for config file " << cfgfile << endl;

        if (!g_key_file_load_from_file (keyfile, cfgfile, flags, &error)) {
                std::runtime_error("Can't find config file... no default value exists for client-id");
        } else {
                cout << "config file loaded." << endl;
                conf = g_slice_new (Settings);
                conf->client_id    = g_key_file_get_string(keyfile, "oauth", "client_id", NULL);
                client_id = conf->client_id;
                if (client_id == NULL){
			client_id = "default_value";
			std::runtime_error("Can't read client_id from inf file... no default exists.");
		}
                conf->resource_id      = g_key_file_get_string(keyfile, "oauth", "resource_id", NULL);
		resource_id = conf->resource_id;
	        if (resource_id == NULL){
			resource_id = "00000002-0000-0000-c000-000000000000";
		        cout << "Can't read resource id -- using default value"	
		}
		conf-> resource_id     = g_key_file_get_string(keyfile, "oauth", "tenant", NULL);
		tenant = conf->tenant;
		if (tenant == NULL){
			tenant = "common";
			cout << "Can't read tenant -- using default value"
			}	
               }
}
