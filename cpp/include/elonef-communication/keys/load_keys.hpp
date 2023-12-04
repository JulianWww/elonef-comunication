#pragma once

#include <cryptopp/rsa.h>
#include <elonef-communication/types.hpp>

namespace Elonef {
    CryptoPP::RSA::PrivateKey load_private_rsa(const std::string& key);
    CryptoPP::RSA::PublicKey load_public_rsa(const std::string& key);

    Elonef::ECDSA::PrivateKey load_private_ecdsa(const std::string& key);
    Elonef::ECDSA::PublicKey load_public_ecdsa(const std::string& key);
}