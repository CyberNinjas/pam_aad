extern "C" {
#include "jansson.h"
}

using namespace std;

namespace jwtcpp {

	/**
	 * Simple wrapper around the cryptopp base64 facilities.
	 *
	 * BrowserID likes to strip padding characters off of base64-encoded
	 * strings, meaning we can't use the stdlib routines to encode them
	 * directly.  This is a simple wrapper that strips the padding.
	 *
	 * @param string the base64 string to decode
	 *
	 * @return string the decoded value
	 **/
	string decodeBase64(string value);

	/**
	 * Simple wrapper around the cryptopp base64 facilites.
	 *
	 * BrowserID likes to strip padding characters off of base64-encoded
	 * strings, meaning we can't use the stdlib routines to encode them
	 * directly.  This is a simple wrapper that strips the padding.
	 *
	 * @param string the normal string to encode to base64.
	 *
	 * @return the encoded value
	 **/
	string encodeBase64(string value);

	/**
	 * Decode a base64 value and convert it to a json object.
	 *
	 * @param string the base64-encoded input value
	 * 
	 * @return json_t* the json object
	 **/
	json_t* decodeJSONBytes(string input);

	/**
	 * Base64-encode a json object.
	 *
	 * @param json_t* the json object
	 * @return string the encoded value
	 **/
	string encodeJSONBytes(json_t* input);
}
