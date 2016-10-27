#include <iostream>
#include "string.h"

#include "utils.h"
#include "jwt.h"

#include "cryptopp/dsa.h"
#include "cryptopp/rsa.h"
#include "cryptopp/osrng.h"

#include "unittest++/UnitTest++.h"

extern "C" {
#include "jansson.h"
}

using namespace std;
using namespace jwtcpp;
using namespace CryptoPP;

/**
 * This file contains tests for the JWT generation and parsing.
 **/

TEST(DecodeBase64)
{
	// extra == should be added on the fly
    string decoded = decodeBase64("eyJ0ZXN0IjogInllYWgifQ");
	CHECK_EQUAL("{\"test\": \"yeah\"}", decoded);
}

TEST(EncodeBase64)
{
	// when encoding, the extra "=" should be removed
    string base64json = encodeBase64("{\"test\": \"yeah\"}");
	CHECK_EQUAL("eyJ0ZXN0IjogInllYWgifQ", base64json);
}

TEST(DecodeJSONBytes)
{
	// An encoded b64 encoded JSON value should decode successfully
    json_t* root = decodeJSONBytes("eyJ0ZXN0IjogInllYWgifQ");
	// check that the returned object is a json object. It should contain the
	// "test" chain.
    CHECK_EQUAL("yeah", json_string_value(json_object_get(root, "test")));
}

TEST(EncodeJSONBytes)
{
	json_error_t* errors;
	json_t* json = json_loads("{\"key\":\"value\"}", 0, errors);
	CHECK_EQUAL("eyJrZXkiOiAidmFsdWUifQ", encodeJSONBytes(json));
}

TEST(JWT_generation_dsa)
{
	// generate the keys
	AutoSeededRandomPool rnd;

	DSA::PrivateKey privateKey;
	privateKey.GenerateRandomWithKeySize(rnd, 1024);
	string encodedPrivateKey;
	privateKey.Save(StringSink(encodedPrivateKey).Ref());

	DSA::PublicKey publicKey;
	privateKey.MakePublicKey(publicKey);
	string encodedPublicKey;
	publicKey.Save(StringSink(encodedPublicKey).Ref());

	// and generate some tokens
	map<string, string> map; // empty map should work
	string token = generate("DSA", encodedPrivateKey, &map);

	JWT* parsedToken = parse(token);
	CHECK_EQUAL(true, parsedToken->checkSignature(encodedPublicKey));
}

TEST(JWT_generation_rsa)
{
	// generate the keys
	AutoSeededRandomPool rnd;

	RSA::PrivateKey rsaPrivate;
	rsaPrivate.GenerateRandomWithKeySize(rnd, 3072);

	RSA::PublicKey rsaPublic(rsaPrivate);
}

TEST(JWT_Extraction)
{
	// We should be able to load a JSON Web Token. This means being able to
	// extract the information from the token.
}

TEST(JWT_Signature)
{
	// We should also be able to check that the certificates bundled in the
	// token are valid for it.
}

TEST(Load_Algorithm)
{
	// JWT handle a bunch of algorithms. We should be able to load the right
	// one depending on some text.
	// In case of a failure, we should throw an exception
}

int main(int argc, const char *argv[])
{
    return UnitTest::RunAllTests();
}
