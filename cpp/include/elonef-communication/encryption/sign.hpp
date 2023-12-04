#pragma once

#include <cryptopp/queue.h>
#include <elonef-communication/types.hpp>

namespace Elonef {
    struct VerificationResult {
        bool safe;
        CryptoPP::ByteQueue data;
    };

    CryptoPP::ByteQueue sign_nonstreamable(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PrivateKey& key);
    CryptoPP::ByteQueue sign(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PrivateKey& key);

    bool verify_nonstreamable(CryptoPP::ByteQueue& data, CryptoPP::ByteQueue& signature, const Elonef::ECDSA::PublicKey& key);
    VerificationResult verify(CryptoPP::ByteQueue& data, const Elonef::ECDSA::PublicKey& key);
}