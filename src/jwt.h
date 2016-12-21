#include <iostream>
#include <map>
#include "json.hpp"

using namespace std;

namespace jwtcpp{

	/**
	 * A class representing a JSON Web Token.
	 *
	 * JWT are defined in http://self-issued.info/docs/draft-jones-json-web-token.html
	 * This class
	 *
	 **/
	class JWT{
		private:
			string algorithm;
			nlohmann::json payload;
			string signature;
			string signed_data;

		public:
			/**
			 * JWT class constructor.
			 *
			 * @param string the name of the algorithm to be used to sign or
			 *               verify the signature of the token.
			 * @param map<string, string> the payload contained in the token
			 * @param string the signature
			 * @param string the signed data
			 **/
			JWT(const string algorithm, nlohmann::json payload, const string signature,
				const string signed_data);

			/**
			 * Check the current token against the given public key.
			 *
			 * @param string the public key data.
			 *
			 * @return bool True if the signature is correct, false otherwise.
			 **/
			bool checkSignature(const string& key);
                      
                       /**
                         * Getter for the stuff that makes up the jwt token.
                         *
                         * @return string the algorithm used for encryption
                         **/
                        string getAlgorithm();

                       /**
                         * Getter for the payload found in the token.
                         * 
                         * @return nlohmann::json the payload
                         **/
                        nlohmann::json getPayload();

					  /*
					   * Getter for specific values from the payload.
					   * 
					   * @param string key the slot you're accessing from the payload
					   *
					   * @return string the value of the value assoiate with the provided key.
					   **/
					   string fromPayload(string key);
	};

    /**
     * Parse a string into a JSON Web Token.
     *
	 * @param string the text to parse (the encoded and signed JWT)
	 *
	 * @return *JWT a JWT object.
	 * @throws ParsingError if an error occurs during the parsing
	 **/
	JWT* parse(const string& jwt);

	/**
	 * Generates and sign a JWT from a map of <string, string> (dict).
	 *
	 * @param string the name of the algorithm used to sign the token.
	 * @param string the key that will be used to sign the token.
	 * @param map<string, string> a map containing the payload.
	 *
	 * @return string the encoded and signed JSON Web Token.
	 **/
	string generate(const string& algorithm, const string& key,
			        map<string, string>* payloadMap);
}
