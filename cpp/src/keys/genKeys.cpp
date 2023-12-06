#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/utils.hpp>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp-pem/pem.h>
#include <elonef-communication/types.hpp>
#include <elonef-communication/encoding/encoding.hpp>
#include <elonef-communication/encoding/decoding.hpp>
#include <iostream>

std::string buildIndenter(size_t indent) {
    std::string indenter;
    for (size_t idx=0; idx<indent; idx++) {
        indenter = indenter + "\t";
    }
    return indenter;
}

Elonef::SignedKey::SignedKey(CryptoPP::ByteQueue& data) {
    this->key = Elonef::toDynamicSizeString(data);
    this->signatures = Elonef::toMap<std::string, std::string>(data, toDynamicSizeString_long, toDynamicSizeString_long);
}

Elonef::SignedKey::SignedKey() {}

CryptoPP::ByteQueue Elonef::SignedKey::toQueue() const {
    CryptoPP::ByteQueue out = Elonef::toBytes(this->key);
    Elonef::toBytes(signatures.begin(), signatures.end()).TransferAllTo(out);
    return out;
}

Elonef::PublicClientKey::PublicClientKey(CryptoPP::ByteQueue& data) {
    this->id = Elonef::toDynamicSizeString(data);
    this->data_key = Elonef::SignedKey(data);
    this->sign_key = Elonef::SignedKey(data);
}

Elonef::PublicClientKey::PublicClientKey() {}

CryptoPP::ByteQueue Elonef::PublicClientKey::toQueue() const {
    CryptoPP::ByteQueue out = Elonef::toBytes(this->id);
    this->data_key.toQueue().TransferAllTo(out);
    this->sign_key.toQueue().TransferAllTo(out);
    return out;
}

Elonef::PrivateClientKey::PrivateClientKey(CryptoPP::ByteQueue& data) {
    this->sign_key = Elonef::toDynamicSizeString(data);
    this->data_key = Elonef::toDynamicSizeString(data);
    this->server_key = Elonef::toDynamicSizeString(data);
    this->ca_keys = Elonef::toMap<std::string, std::string>(data, toDynamicSizeString_long, toDynamicSizeString_long);
    this->uid = Elonef::toDynamicSizeString(data);
}

Elonef::PrivateClientKey::PrivateClientKey() {}

CryptoPP::ByteQueue Elonef::PrivateClientKey::toQueue() const {
    CryptoPP::ByteQueue out = Elonef::toBytes(this->sign_key);
    Elonef::toBytes(data_key).TransferAllTo(out);
    Elonef::toBytes(server_key).TransferAllTo(out);
    Elonef::toBytes(ca_keys.begin(), ca_keys.end()).TransferAllTo(out);
    Elonef::toBytes(uid).TransferAllTo(out);   
    return out;
}

Elonef::KeyPair Elonef::generateKeyPairRSA() {
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(Elonef::rng, 3072);

    CryptoPP::RSA::PrivateKey privateKey(params);
    CryptoPP::RSA::PublicKey publicKey(params);

    Elonef::KeyPair out;
    {
        CryptoPP::StringSink sink(out.publicKey);
        CryptoPP::PEM_Save(sink, publicKey);
    };
    {
        CryptoPP::StringSink sink(out.privateKey);
        CryptoPP::PEM_Save(sink, privateKey);
    };

    return out;
}

Elonef::KeyPair Elonef::generateKeyPairECDSA() {
    Elonef::ECDSA::PrivateKey priv;
    Elonef::ECDSA::PublicKey  pub;

    priv.Initialize( Elonef::rng, CryptoPP::ASN1::secp256k1() );
    CryptoPP::ByteQueue privateKey;
    priv.Save(privateKey);

    priv.MakePublicKey(pub);
    CryptoPP::ByteQueue publicKey;
    pub.Save(publicKey);

    Elonef::KeyPair keyPair;
    keyPair.privateKey = toBase64(privateKey);
    keyPair.publicKey  = toBase64(publicKey);

    return keyPair;    
}

Elonef::ClientKeys Elonef::generateClientKeys(std::string server_key, std::unordered_map<std::string, std::string> ca_keys, std::string id) {
    KeyPair ecdsa_keys = generateKeyPairECDSA();
    KeyPair rsa_key = generateKeyPairRSA();

    ClientKeys out;
    out.private_key.data_key   = rsa_key.   privateKey;
    out.private_key.sign_key   = ecdsa_keys.privateKey;
    out.private_key.server_key = server_key;
    out.private_key.ca_keys    = ca_keys;
    out.private_key.uid        = id;

    out.public_key.data_key.key = rsa_key.   publicKey;
    out.public_key.sign_key.key = ecdsa_keys.publicKey;
    out.public_key.id           = id;
    return out;
}