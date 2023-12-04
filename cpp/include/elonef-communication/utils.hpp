#pragma once

#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

namespace Elonef {
    extern CryptoPP::AutoSeededRandomPool rng;

    
    std::string toHex(const CryptoPP::ByteQueue& bytes);
    CryptoPP::ByteQueue fromHex(std::string str);
    std::string toBase64(CryptoPP::ByteQueue& bytes);
    CryptoPP::ByteQueue fromBase64(std::string str);
    std::string toString(const CryptoPP::ByteQueue& bytes);
    CryptoPP::SecByteBlock toSecBlock(CryptoPP::ByteQueue& bytes);

    CryptoPP::ByteQueue toQueue(std::string str);
    CryptoPP::ByteQueue toQueue(const CryptoPP::SecByteBlock& str);

    CryptoPP::ByteQueue get_message(const std::string& data);
}