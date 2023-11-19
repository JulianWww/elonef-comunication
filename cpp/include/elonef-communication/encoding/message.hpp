#pragma once

#include <cryptopp/queue.h>
#include <elonef-communication/types.hpp>

namespace Elonef {
    size_t get_current_time();

    CryptoPP::ByteQueue encode_message(
        const CryptoPP::ByteQueue& message, 
        const CryptoPP::byte& message_type, 
        const std::string& chat_id,
        const CryptoPP::ByteQueue& key_uuid, 
        const CryptoPP::ByteQueue& aes_key, 
        const Elonef::ECDSA::PrivateKey& signature_key
    );
}
