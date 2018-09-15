//
// Created by James on 2018/09/13.
//

#include "Crypto.h"
#include "Web3.h"
#include "Util.h"
#include "Log.h"
#include "Trezor/secp256k1.h"
#include "Trezor/ecdsa.h"
#include <vector>

#define SIGNATURE_LENGTH 64

Crypto::Crypto(Web3* _web3) 
{
    web3 = _web3;
}

bool Crypto::Sign(BYTE* digest, BYTE* result) 
{
    const ecdsa_curve *curve = &secp256k1;
    uint8_t pby;
    int res = ecdsa_sign_digest(curve, privateKey, digest, result, &pby, NULL);

    result[65] = pby;

    Serial.println("SIGN");
    Serial.println(pby);
    Serial.println(res);
	return (res == 1);
}

void Crypto::SetPrivateKey(const uint8_t *key) 
{
    privateKey = key;
}

void Crypto::ECRecover(BYTE* signature, BYTE *public_key, BYTE *message_hash)
{
    uint8_t v = signature[64]; //recover 'v' from end of signature
	uint8_t pubkey[65];
    uint8_t signature64[64];
    if (v > 26) v -= 27; //correct value of 'v' if required
    memcpy(signature64, signature, 64);
	const ecdsa_curve *curve = &secp256k1;
	ecdsa_recover_pub_from_sig (curve, pubkey, signature, message_hash, v);
    memcpy(public_key, pubkey+1, 64);
}

bool Crypto::Verify(const uint8_t *public_key, const uint8_t *message_hash, const uint8_t *signature)
{
    const ecdsa_curve *curve = &secp256k1;
    BYTE publicKey4[65];
    memcpy(publicKey4+1, public_key, 64);
    publicKey4[0] = 4;
    int success = ecdsa_verify_digest(curve, publicKey4, signature, message_hash);
    return (success == 0);
}

void Crypto::PrivateKeyToPublic(const uint8_t *privateKey, uint8_t *publicKey)
{
    uint8_t buffer[65];
    const ecdsa_curve *curve = &secp256k1;
    ecdsa_get_public_key65(curve, privateKey, buffer);
    memcpy(publicKey, buffer+1, 64);
}

void Crypto::PublicKeyToAddress(const uint8_t *publicKey, uint8_t *address)
{
    uint8_t hashed[32];
    Keccak256(publicKey, 64, hashed);
    memcpy(address, &hashed[12], 20);
}

void Crypto::Keccak256(const uint8_t *data, uint16_t length, uint8_t *result)
{
    keccak_256(data, length, result);
}