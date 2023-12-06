#pragma once

#include <cryptopp/eccrypto.h>

namespace Elonef {
    namespace ECDSA {
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PrivateKey PrivateKey;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::PublicKey PublicKey;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Signer Signer;
        typedef CryptoPP::ECDSA<CryptoPP::ECP, CryptoPP::SHA256>::Verifier Verifier;
    }

    struct Message {
        CryptoPP::ByteQueue message;
        std::string sender_id;
        CryptoPP::byte type;
        size_t send_time;
        size_t upload_time;
        size_t msg_id;
        
        // set to true if one of the decoding steps failes
        bool invalid = false;
    };
}

std::ostream& operator<<(std::ostream& stream, const Elonef::Message& msg);