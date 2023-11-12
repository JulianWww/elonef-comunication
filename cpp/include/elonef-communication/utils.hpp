#pragma once

#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

namespace Elonef {
    extern CryptoPP::AutoSeededRandomPool rng;

    
    std::string toHex(CryptoPP::ByteQueue& bytes);
    CryptoPP::ByteQueue fromHex(std::string str);
    std::string toString(const CryptoPP::ByteQueue& bytes);
    CryptoPP::SecByteBlock toSecBlock(CryptoPP::ByteQueue& bytes);

    CryptoPP::ByteQueue toQueue(std::string str);
    CryptoPP::ByteQueue toQueue(CryptoPP::SecByteBlock str);

    CryptoPP::ByteQueue get_message(const uint8_t* data, size_t size);
}