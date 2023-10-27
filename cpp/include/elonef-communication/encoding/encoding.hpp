#pragma once

#include <cryptopp/queue.h>
#include <iterator>
#include <iostream>

namespace Elonef {
    CryptoPP::ByteQueue toBytes(size_t number);
    CryptoPP::ByteQueue toBytes_short(size_t number);
    CryptoPP::ByteQueue toBytes(std::string str, CryptoPP::ByteQueue(*number_encoder)(size_t num) = & toBytes);
    CryptoPP::ByteQueue toBytes(CryptoPP::ByteQueue queue, CryptoPP::ByteQueue(*number_encoder)(size_t num) = & toBytes);
    
    template <typename T>
    inline CryptoPP::ByteQueue toBytes(T start, T end) {
        CryptoPP::ByteQueue out;
        toBytes(std::distance(start, end)).TransferAllTo(out);
        for (; start!=end; start++)
            toBytes(*start).TransferAllTo(out);
        return out;
    }
}