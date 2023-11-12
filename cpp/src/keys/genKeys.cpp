#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/utils.hpp>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp-pem/pem.h>
#include <iostream>
#include <elonef-communication/types.hpp>

std::string buildIndenter(size_t indent) {
    std::string indenter;
    for (size_t idx=0; idx<indent; idx++) {
        indenter = indenter + "\t";
    }
    return indenter;
}

std::string Elonef::SignedKey::print(size_t indent) {
    std::string out;
    std::string indenter = buildIndenter(indent);

    out = out + indenter + "{\n"
              + indenter + "\tkey: " + this->key + "\n"
              + indenter + "\tca_keys: {\n";
    
    for (auto key: this->signatures) {
        out = out + indenter + "\t\t" + key.first + ": " + key.second + "\n";
    }

    out = out + indenter + "\t}\n"
              + indenter + "}\n";

    return out;
}

std::string Elonef::PublicClientKey::print(size_t indent) {
    std::string out;
    std::string indenter = buildIndenter(indent);
    out = out + indenter + "{\n"
              + indenter + "\tdata_key:\n"
              + this->data_key.print(indent + 1)
              + indenter + "\tsign_key:\n"
              + this->sign_key.print(indent+1)
              + indenter + "\tuid: " + this->id + "\n"
              + indenter + "}\n";


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
    keyPair.privateKey = toHex(privateKey);
    keyPair.publicKey  = toHex(publicKey);

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