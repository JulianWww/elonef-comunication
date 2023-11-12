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
    CryptoPP::ByteQueue toBytes(T start, T end);

    CryptoPP::ByteQueue vectorToBytes(const std::vector<std::string>& keys);

    template<typename T, typename F>
    CryptoPP::ByteQueue toBytes(std::pair<T, F> value);
}

template<typename T>
inline CryptoPP::ByteQueue Elonef::toBytes(T start, T end) {
    CryptoPP::ByteQueue out;
    toBytes(std::distance(start, end)).TransferAllTo(out);
    for (; start!=end; start++)
        toBytes(*start).TransferAllTo(out);
    return out;
}

template<typename T, typename F>
inline CryptoPP::ByteQueue Elonef::toBytes(std::pair<T, F> value) {
    CryptoPP::ByteQueue out = toBytes(value.first);
    toBytes(value.second).TransferAllTo(out);
    return out;
}