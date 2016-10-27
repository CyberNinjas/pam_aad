#include "cryptopp/base64.h"

using namespace CryptoPP;
using namespace std;

namespace jwtcpp {

string decodeBase64(string value){
    string out;

    // Add some chars to the input so it works as expected
    int pad = value.size() % 4;

    if(pad == 2){
        value += "==";
    } else if (pad == 3){
        value += "=";
    }

    StringSource(value, true, new Base64Decoder(new StringSink(out))); 
    return out;
}

string encodeBase64(string value){
    string out;
    StringSource(value, true, new Base64Encoder(new StringSink(out))); 

    // remove the extra "=" appended by the base64 convertion
    out.erase(out.find("="));
    return out;
}

}