#pragma once

#include <cryptopp/queue.h>
#include "../keys/genKeys.hpp"
#include <iterator>
#include <concepts>

template<typename T>
concept HasToQueue = requires(T a)
{
    { a.toQueue() } -> std::convertible_to<CryptoPP::ByteQueue>;
};

namespace Elonef {
    template<HasToQueue T>
    CryptoPP::ByteQueue toBytes(const T& val);
    CryptoPP::ByteQueue toBytes(size_t number);
    CryptoPP::ByteQueue toBytes_short(size_t number);
    CryptoPP::ByteQueue toBytes(const std::string& str, CryptoPP::ByteQueue(*number_encoder)(size_t num) = & toBytes);
    CryptoPP::ByteQueue toBytes(CryptoPP::ByteQueue queue, CryptoPP::ByteQueue(*number_encoder)(size_t num) = & toBytes);

    CryptoPP::ByteQueue toBytes_static_size(const CryptoPP::ByteQueue& queue);
    
    template <typename T>
    CryptoPP::ByteQueue toBytes(T start, T end);

    CryptoPP::ByteQueue listToBytes(const std::list<std::string>& keys);

    template<typename T, typename F>
    CryptoPP::ByteQueue toBytes(std::pair<T, F> value);

    template<typename T>
    CryptoPP::ByteQueue toBytes(T* value);

    CryptoPP::ByteQueue stringQueuePairListToBuffer(const std::list<std::pair<std::string, CryptoPP::ByteQueue>>& value);
    CryptoPP::ByteQueue signedKeyToBytes(const SignedKey& key);
}

template<HasToQueue T>
inline CryptoPP::ByteQueue Elonef::toBytes(const T& val) {
    return val.toQueue();
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

template<typename T>
CryptoPP::ByteQueue Elonef::toBytes(T* value) {
    return toBytes(*value);    
}