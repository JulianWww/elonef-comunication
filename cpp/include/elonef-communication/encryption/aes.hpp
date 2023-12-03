#pragma once

#include <cryptopp/queue.h>

namespace Elonef {
    CryptoPP::ByteQueue encrypt(CryptoPP::ByteQueue& to_encrypt, const CryptoPP::SecByteBlock& key);
    CryptoPP::ByteQueue decrypt(CryptoPP::ByteQueue& to_decrypt, const CryptoPP::SecByteBlock& key);


    CryptoPP::SecByteBlock randomBytes(const size_t& size);
    CryptoPP::SecByteBlock randomKey();
}

