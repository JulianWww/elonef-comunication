#include <elonef-communication/keys/load_keys.hpp>
#include <elonef-communication/utils.hpp>
#include <elonef-communication/types.hpp>
#include <cryptopp-pem/pem.h>
#include <cryptopp/osrng.h>

template <typename Key>
inline Key load_rsa(std::string key_str) {
    CryptoPP::StringSource source(key_str, true);
    Key key;
    CryptoPP::PEM_Load(source, key);
    return key;
}

template <typename Key>
inline Key load_ecdsa(std::string key_str) {
    auto priv_exp = Elonef::fromHex(key_str);
    Key priv_key;
    priv_key.Load(priv_exp);
    return priv_key;
}

CryptoPP::RSA::PrivateKey Elonef::load_private_rsa(std::string key_str) {
    return load_rsa<CryptoPP::RSA::PrivateKey>(key_str);
}

CryptoPP::RSA::PublicKey Elonef::load_public_rsa(std::string key_str) {
    return load_rsa<CryptoPP::RSA::PublicKey>(key_str);
}

Elonef::ECDSA::PrivateKey Elonef::load_private_ecdsa(std::string key) {
    return load_ecdsa<Elonef::ECDSA::PrivateKey>(key);
}

Elonef::ECDSA::PublicKey Elonef::load_public_ecdsa(std::string key) {
    return load_ecdsa<Elonef::ECDSA::PublicKey>(key);
}