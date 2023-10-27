#include <elonef-communication/keys/genKeys.hpp>
#include <elonef-communication/utils.hpp>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp-pem/pem.h>
#include <iostream>
#include <elonef-communication/types.hpp>

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