#pragma once

#include <crypto++/hex.h>
#include <cryptopp/osrng.h>

namespace Elonef {
    extern CryptoPP::AutoSeededRandomPool rng;

    
    std::string toHex(CryptoPP::ByteQueue& bytes);
    CryptoPP::ByteQueue fromHex(std::string str);
    std::string toString(CryptoPP::ByteQueue& bytes);
    CryptoPP::SecByteBlock toSecBlock(CryptoPP::ByteQueue& bytes);

    CryptoPP::ByteQueue toQueue(std::string str);
}